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
uint8_t check_if_folder_exists(const char* path);
void open_project(layer_registry* Registry,const char* Path){
    FILE* f =fopen(Path,"r");
    if(!f) return;
    char line[512];
    char project_root[512] ={0};
    char name[32] ={0};
    char main_scene_path[512] ={0};
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
    }
    fclose(f);
    if (project_root[0]=='\0'||main_scene_path[0]=='\0')
        return;
    size_t Root_Len = strlen(project_root);
    char Full_Scene_Path[512];
    if(project_root[Root_Len-1]=='/'||project_root[Root_Len-1]=='\\')
        snprintf(Full_Scene_Path,sizeof(Full_Scene_Path),"%s%s",project_root,main_scene_path);
    else
        snprintf(Full_Scene_Path,sizeof(Full_Scene_Path),"%s%s%s",project_root,PATH_SEP,main_scene_path);
    
    char build_dir[512];
    snprintf(build_dir,sizeof(build_dir),"%s%s%s",project_root,PATH_SEP,"build");
    if(!check_if_folder_exists(build_dir)){
        char cmake_config_cmd[1024];
        snprintf(cmake_config_cmd,sizeof(cmake_config_cmd),"cmake -S \"%s\" -B \"%s\"",project_root,build_dir);
        system(cmake_config_cmd);
    }
    char cmake_build_cmd[1024];
    snprintf(cmake_build_cmd,sizeof(cmake_build_cmd),"cmake --build \"%s\" --config Debug",build_dir);
    system(cmake_build_cmd);

    char dll_path[512];
    snprintf(dll_path,sizeof(dll_path),"%s%s%s%s%s%s%s%s",project_root,PATH_SEP,"bin",PATH_SEP,"Debug",PATH_SEP,name,".dll");

    void (*game_fn)(void) =NULL;
    #ifdef _WIN32
    HMODULE dll= LoadLibraryA(dll_path);
    GAVEN_ASSERT(dll,"couldnt load %s.dll",name);
    game_fn=(void*)GetProcAddress(dll,"game");
    #else
    void* dll= dlopen(dll_path,RTLD_NOW);
    GAVEN_ASSERT(dll,"couldnt load %s.dll",name);
    game_fn=dlsym(dll,"game");
    #endif
    GAVEN_ASSERT(game_fn,"Failed to open game function inside %s.dll",name);
    game_fn();
    set_main_scene(Full_Scene_Path);
    entity_registry* Reg =load_scene(Full_Scene_Path);
    add_layer(Registry,create_editor_layer(Reg));
}