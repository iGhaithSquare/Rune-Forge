#include "editor.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "editor_layer.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

static const char* project_file_path;
static const char* project_root_path;
static const char* project_name;
static uint64_t dll_timestamp;
void open_project(layer_registry* Registry,const char* Path){
    project_file_path=strdup(Path);
    FILE* f =fopen(Path,"r");
    if(!f) return;
    char line[512];
    char project_root[512] ={0};
    char name[32] ={0};
    char main_scene_path[512] ={0};
    int current_asset_type=0;
    char current_asset_path[512];
    while(fgets(line,sizeof(line),f)){
        if(strncmp(line,"name=",5)==0){
            strcpy(name,line+5);
            name[strcspn(name,"\r\n")]=0;
        }
        else if(strncmp(line,"root=",5)==0){
            strcpy(project_root,line+5);
            project_root[strcspn(project_root,"\r\n")]=0;
        }
        else if(strncmp(line,"main_scene=",11)==0){
            strcpy(main_scene_path,line+11);
            main_scene_path[strcspn(main_scene_path,"\r\n")]=0;
        }
        else if(strncmp(line,"asset=",6)==0){
            sscanf(line+6,"%d,%511[^\r\n]",&current_asset_type,current_asset_path);
            char* Asset_Path =strdup(current_asset_path);
            for(int i=0;Asset_Path[i];i++)
                if(Asset_Path[i]=='\\')
                    Asset_Path[i]='/';
            load_game_asset(Asset_Path,(asset_type)current_asset_type);
        }
    }
    fclose(f);
    project_name=strdup(name);
    if (project_root[0]=='\0'||main_scene_path[0]=='\0')
        return;
    size_t Root_Len = strlen(project_root);
    char Full_Scene_Path[512];
    if(project_root[Root_Len-1]=='/'||project_root[Root_Len-1]=='\\')
        snprintf(Full_Scene_Path,sizeof(Full_Scene_Path),"%s%s",project_root,main_scene_path);
    else
        snprintf(Full_Scene_Path,sizeof(Full_Scene_Path),"%s%s%s",project_root,PATH_SEP,main_scene_path);
    project_root_path=strdup(project_root);
    char build_dir[512];
    snprintf(build_dir,sizeof(build_dir),"%s%s%s",project_root,PATH_SEP,"build");
    if(!check_if_folder_exists(build_dir)){
        char cmake_config_cmd[1024];
        snprintf(cmake_config_cmd,sizeof(cmake_config_cmd),"cmake -S \"%s\" -B \"%s\" -G \"MinGW Makefiles\" -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++",project_root,build_dir);
        system(cmake_config_cmd);
    }
    char cmake_build_cmd[1024];
    snprintf(cmake_build_cmd,sizeof(cmake_build_cmd),"cmake --build \"%s\" --config Debug",build_dir);
    system(cmake_build_cmd);

    char dll_path[512];
    snprintf(dll_path,sizeof(dll_path),"%s%s%s%s%s%s%s",project_root,PATH_SEP,"bin",PATH_SEP,"lib",name,".dll");
    poll_dll();
    init_input();
    set_main_scene(Full_Scene_Path);
    entity_registry* Reg =load_scene(Full_Scene_Path);
    add_layer(Registry,create_editor_layer(Reg));
}

void write_file(const char* Path,const char* Content){
    FILE *f= NULL;
    f=fopen(Path,"w");
    GAVEN_ASSERT(f,"Couldnt create file in %s",Path);
    fputs(Content,f);
    fclose(f);
}

void append_file(const char* Path,const char* Content){
    FILE *f= NULL;
    f=fopen(Path,"a");
    GAVEN_ASSERT(f,"Couldnt create file in %s",Path);
    fputs(Content,f);
    fclose(f);
}

uint8_t get_current_path(char* Buffer,size_t Size){
    #ifdef _WIN32
    DWORD len = GetModuleFileNameA(NULL,Buffer,(DWORD)Size);
    if(len==0||len==Size)
        return 0;
    #else
    size_t len=readlink("/proc/self/exe",Buffer,size-1);
    if(len==-1) return 0;
    Buffer[len]='\0';
    #endif
    return 1;
}
const char* get_projecta_file(void){
    return project_file_path;
}
void poll_dll(void){
    char dll_old_path[512];
    snprintf(dll_old_path,sizeof(dll_old_path),"%s%s%s%s%s%s%s",project_root_path,PATH_SEP,"bin",PATH_SEP,"lib",project_name,".dll");
    char dll_path[512];
    snprintf(dll_path,sizeof(dll_path),"%s%s%s%s%s%s%s",project_root_path,PATH_SEP,"bin",PATH_SEP,"Rlib",project_name,".dll");
    void (*game_fn)(void) =NULL;
    if(!file_exists(dll_old_path))
        return;
    #ifdef _WIN32
    Sleep(100);
    HMODULE test_dll=NULL;
    test_dll=LoadLibraryA(dll_old_path);
    #else
    usleep(100*1000);
    void* test_dll=NULL;
    test_dll = dlopen(dll_old_path,RTLD_NOW);
    #endif
    if(!test_dll)
        return;
    TypeDB_Clear();
    FreeLibrary(test_dll);
    #ifdef _WIN32
    static HMODULE dll=NULL;
    if(dll)
        FreeLibrary(dll);
    #else
    static void* dll=NULL;
    if(dll)
        dlclose(dll);
    #endif
    remove(dll_path);
    rename(dll_old_path,dll_path);
    #ifdef _WIN32
    dll=LoadLibraryA(dll_path);
    game_fn=(void*)GetProcAddress(dll,"game");
    #else
    dll = dlopen(dll_path,RTLD_NOW);
    game_fn=dlsym(dll,"game");
    #endif
    GAVEN_ASSERT(game_fn,"Failed to open game function inside %s.dll",project_name);
    game_fn();
}
uint8_t file_exists(const char* Path){
    #ifdef _WIN32
    DWORD attr=GetFileAttributesA(Path);
    return (attr!=INVALID_FILE_ATTRIBUTES&&!(attr&FILE_ATTRIBUTE_DIRECTORY));
    #else
    struct stat buffer;
    return (stat(Path,&buffer)==0);
    #endif
}
void get_editor_path(char* Buffer,size_t Size){
    #ifdef _WIN32
    DWORD Len = GetModuleFileNameA(NULL,Buffer,(DWORD)Size);
    if(Len==0||Len>=Size){
        Buffer[0]='\0';
        GAVEN_WARN("Couldnt get editor path");
        return;
    }
    #else
    ssize_t Len=readlink("/proc/self/exe",Buffer,Size-1);
    
    if(Len==-1||Len>=Size){
        Buffer[0]='\0';
        GAVEN_WARN("Couldnt get editor path");
        return;
    }
    Buffer[Len]='\0';
    #endif
    char* last =strrchr(Buffer,PATH_SEP[0]);
    if(last) *last='\0';
}

uint8_t check_if_folder_exists(const char* path){
    #ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES)&&(attrs&FILE_ATTRIBUTE_DIRECTORY);
    #else
    struct stat info;
    return (stat(path,&info)==0)&&(info.st_mode&S_IFDIR);
    #endif
}
void create_new_folder(const char* path){
    if(check_if_folder_exists(path))return;
    #ifdef _WIN32
    CreateDirectoryA(path,NULL);
    #else
    mkdir(path,0755);
    #endif
    GAVEN_ASSERT(check_if_folder_exists(path),"COULD NOT CREATE LOG FOLDER");
}
const char* get_project_name(void){
    return project_name;
}
void copy_file_to_dir(const char* Source,const char* Dir){
    const char* last =strrchr(Source,PATH_SEP[0]);
    if(last) last++;
    else last =Source;
    char Destination[512];
    snprintf(Destination,sizeof(Destination),"%s%s%s",Dir,PATH_SEP,last);
    copy_file(Source,Destination);
}

void copy_file(const char* Source,const char* Destination){
    FILE *src=NULL;
    src=fopen(Source,"rb");
    GAVEN_ASSERT(src,"Couldnt open file %s",Source);
    FILE *dst= NULL;
    dst=fopen(Destination,"wb");
    GAVEN_ASSERT(dst,"Couldnt create file in %s",Destination);
    char Buffer[8192];
    size_t Read;
    while(Read=fread(Buffer,1,sizeof(Buffer),src)){
        fwrite(Buffer,1,Read,dst);
    }
    fclose(src);
    fclose(dst);
}
void copy_dir_to_dir(const char* Source,const char* Destination){
    if(!check_if_folder_exists(Destination)) create_new_folder(Destination);
    #ifdef _WIN32
    char pattern[1024];
    snprintf(pattern,sizeof(pattern),"%s%s%s",Source,PATH_SEP,"*");
    WIN32_FIND_DATAA data;
    HANDLE hFind = FindFirstFileA(pattern,&data);
    if(hFind==INVALID_HANDLE_VALUE)
        return;
    do{
        if(strcmp(data.cFileName,".")==0||strcmp(data.cFileName,"..")==0)
            continue;
        char Src_Path[1024];
        snprintf(Src_Path,sizeof(Src_Path),"%s%s%s",Source,PATH_SEP,data.cFileName);
        char Dst_Path[1024];
        snprintf(Dst_Path,sizeof(Dst_Path),"%s%s%s",Destination,PATH_SEP,data.cFileName);
        if(data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY){
            copy_dir_to_dir(Src_Path,Dst_Path);
        }
        else{
            copy_file(Src_Path,Dst_Path);
        }

    }while(FindNextFileA(hFind,&data));
    FindClose(hFind);
    #else
    DIR* dir=opendir(Source);
    GAVEN_ASSERT(dir,"Couldnt open the dir %s",Source);
    struct dirent* e;
    while((e=readdir(dir))!=NULL){
        if(strcmp(e->d_name,".")==0||strcmp(e->d_name,"..")==0)
            continue;
        char Src_Path[1024];
        snprintf(Src_Path,sizeof(Src_Path),"%s%s%s",Source,PATH_SEP,e->d_name);
        char Dst_Path[1024];
        snprintf(Dst_Path,sizeof(Dst_Path),"%s%s%s",Destination,PATH_SEP,e->d_name);
        struct stat st;
        if(stat(Src_Path,&st)==0&&S_ISDIR(st.st_mode)){
            copy_dir_to_dir(Src_Path,Dst_Path);
        }
        else{
            copy_file(Src_Path,Dst_Path);
        }

    };
    closedir(dir);
    #endif
}
const char* get_project_path(void){
    return project_root_path;
}