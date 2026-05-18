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
uint8_t check_if_folder_exists(const char* path);
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
            GAVEN_WARN("%s",current_asset_path);
            load_game_asset(current_asset_path,(asset_type)current_asset_type);
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
    size_t len=readlink("/proc/self/exe",buffer,size-1);
    if(len==-1) return 0;
    Buffer[len]='\0';
    #endif
    return 1;
}
const char* get_projecta_file(void){
    return project_file_path;
}
uint8_t get_dll_change(char *dll_path){
    static uint64_t dll_timestamp = 0;
    uint64_t current_timestamp;
    #ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA data;
    if(!GetFileAttributesExA(dll_path,GetFileExInfoStandard,&data)){
        return 0;
    }
    ULARGE_INTEGER t;
    t.HighPart=data.ftLastWriteTime.dwHighDateTime;
    t.LowPart=data.ftLastWriteTime.dwLowDateTime;
    current_timestamp = (uint64_t)t.QuadPart;
    #else
    struct stat st;
    GAVEN_ASSERT(stat(Path,&st),"Couldnt get file attributes %s",dll_path);
    current_timestamp = (uint64_t)st.st_mtime;
    #endif
    if(dll_timestamp==current_timestamp){
        return 0;
    }
    dll_timestamp=current_timestamp;
    return 1;
}
void poll_dll(void){
    char dll_path[512];
    snprintf(dll_path,sizeof(dll_path),"%s%s%s%s%s%s%s",project_root_path,PATH_SEP,"bin",PATH_SEP,"lib",project_name,".dll");
    void (*game_fn)(void) =NULL;
    if(!get_dll_change(dll_path)){
         return;
    }
    #ifdef _WIN32
    HMODULE dll= LoadLibraryA(dll_path);
    if(!dll){
        return;
    }
    game_fn=(void*)GetProcAddress(dll,"game");
    #else
    void* dll= dlopen(dll_path,RTLD_NOW);
    if(!dll){
        return;
    }
    game_fn=dlsym(dll,"game");
    #endif
    GAVEN_ASSERT(game_fn,"Failed to open game function inside %s.dll",project_name);//s
    game_fn();
    #ifdef _WIN32
    FreeLibrary(dll);
    #else
    dlclose(dll);
    #endif
}