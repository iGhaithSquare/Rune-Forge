#include "runeforge.h"
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif
void runtime_read_config(const char* Path){
    FILE* f=fopen(Path,"r");
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
    GAVEN_ASSERT(project_root[0]!='\0'&&main_scene_path[0]!='\0',"COULDNT READ PROJEC NAME OR PATH");
    char Full_Scene_Path[512];
    size_t Root_Len = strlen(project_root);
    if(project_root[Root_Len-1]=='/'||project_root[Root_Len-1]=='\\')
        snprintf(Full_Scene_Path,sizeof(Full_Scene_Path),"%s%s",project_root,main_scene_path);
    else
        snprintf(Full_Scene_Path,sizeof(Full_Scene_Path),"%s%s%s",project_root,PATH_SEP,main_scene_path);
    char dll_path[512];
    snprintf(dll_path,sizeof(dll_path),"%s%s%s","Rlib",name,".dll");
     void (*game_fn)(void) =NULL;
    #ifdef _WIN32
    HMODULE dll= LoadLibraryA(dll_path);
    GAVEN_ASSERT(dll,"couldnt load %s",dll_path);
    game_fn=(void*)GetProcAddress(dll,"game");
    #else
    void* dll= dlopen(dll_path,RTLD_NOW);
    GAVEN_ASSERT(dll,"couldnt load %s",dll_path);
    game_fn=dlsym(dll,"game");
    #endif
    GAVEN_ASSERT(game_fn,"Failed to open game function inside %s.dll",name);
    game_fn();
    init_input();
    set_main_scene(Full_Scene_Path);
    load_scene(Full_Scene_Path);
    change_update_state(1);//start the game
}

int main(int argc, char** argv){
    short game_width = 80;
    short game_height = 24;
    set_window_size(game_width,game_height);
    application* app = runeforge_main();
    runtime_read_config("prj.cfg");
    run_application();
    destroy_application();
    return 1;
}