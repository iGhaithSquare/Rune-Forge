#include "background_panel.h"
#include "../panel_button.h"
#include "../popup_panel.h"
#include "../panel_input_text.h"
#include "../panel_text.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../editor.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#endif
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
void write_file(const char* Path,const char* Content){
    FILE *f= NULL;
    f=fopen(Path,"w");
    GAVEN_ASSERT(f,"Couldnt create file in %s",Path);
    fputs(Content,f);
    fclose(f);
}
typedef struct background_panel_main_button_data{
    layer_registry* Registry;
    layer* Layer;
}background_panel_main_button_data;
typedef struct background_panel_buttons_data{
    layer_registry* Registry;
    layer* Layer;
    panel* Panel;
    panel_input_text* Input_Name_Field;
    panel_input_text* Input_Path_Field;
}background_panel_buttons_data;
typedef struct background_panel_element{
    panel_element Base;
    size_t main_sprite_id;
    sprite help_sprite_1;
    sprite help_sprite_2;
    sprite help_sprite_3;
    sprite help_sprite_4;
} background_panel_element;
void render_background_panel_element(panel_element* Self){
    background_panel_element* be = (background_panel_element*)Self;
    draw_game_sprite(get_game_sprite(be->main_sprite_id),1,2,0);
    draw_game_sprite(be->help_sprite_1,65,18,1);
    draw_game_sprite(be->help_sprite_2,65,19,1);
    draw_game_sprite(be->help_sprite_3,65,20,1);
    draw_game_sprite(be->help_sprite_4,65,21,1);
}
void destroy_background_panel_element(panel_element* Self){
    background_panel_element* be = (background_panel_element*)Self;
    destroy_sprite(&be->help_sprite_1);
    destroy_sprite(&be->help_sprite_2);
    destroy_sprite(&be->help_sprite_3);
    destroy_sprite(&be->help_sprite_4);
    remove_asset(be->main_sprite_id);
    free(Self);
}
void cancle_create_project_impl(panel_button* Self){
    background_panel_buttons_data* Data = Self->Button_Data;
    panel* P = Data->Panel;
    remove_panel_from_registry(P,P->Registry);
}
uint8_t get_editor_path(char* Buffer,size_t Size){
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
void create_project_impl(panel_button* Self){
    background_panel_buttons_data* Data = Self->Button_Data;
    panel* P = Data->Panel;
    const char* Name = Data->Input_Name_Field->Real_Text;
    const char* Path_Old = Data->Input_Path_Field->Real_Text;
    if(!Name||!Path_Old||!strlen(Name)||!strlen(Path_Old)){
        remove_panel_from_registry(P,P->Registry);
        return;
    }
    size_t Path_Len = strlen(Path_Old);
    char Path[256];
    memcpy(Path,Path_Old,Path_Len+1);
    char last_path_char =Path[Path_Len-1];
    if(last_path_char!='\\'&&last_path_char!='/'&&Path_Len+2<sizeof(Path)){
        memcpy(Path+Path_Len,PATH_SEP,strlen(PATH_SEP));
        Path[strlen(PATH_SEP)+Path_Len]='\0';
    }

    char project_dir[512];
    snprintf(project_dir,sizeof(project_dir),"%s%s%s",Path,PATH_SEP,Name);
    char src_dir[512];
    snprintf(src_dir,sizeof(src_dir),"%s%s%s",project_dir,PATH_SEP,"src");
    char asset_dir[512];
    snprintf(asset_dir,sizeof(asset_dir),"%s%s%s",project_dir,PATH_SEP,"assets");
    create_new_folder(project_dir);
    create_new_folder(src_dir);
    create_new_folder(asset_dir);

    char scene_file[512];
    snprintf(scene_file,sizeof(scene_file),"%s%s%s",asset_dir,PATH_SEP,"main.jsonscn");
    char scene_content[1024];
    snprintf(scene_content,sizeof(scene_content),"{\n\"Scene\":\"Main\",\n\"Entities\":	[]\n}");
    write_file(scene_file,scene_content);

    char project_file[512];
    snprintf(project_file,sizeof(project_file),"%s%s%s",project_dir,PATH_SEP,"project.asciiprj");
    char project_content[1024];
    snprintf(project_content,sizeof(project_content),"name=%s\nroot=%s\nmain_scene=assets%smain.jsonscn",Name,project_dir,PATH_SEP);

    write_file(project_file,project_content);

    char game_file[512];
    snprintf(game_file,sizeof(game_file),"%s%s%s",src_dir,PATH_SEP,"game.c");
    const char* Starter = "#include <runeforge.h>\n"
    "#ifdef _WIN32\n"
    "    #ifdef GAME_BUILD_DLL\n"
    "        #define GAME_API __declspec(dllexport)\n"
    "    #else\n"
    "        #define GAME_API __declspec(dllimport)\n"
    "    #endif\n"
    "#else\n"
    "    #define GAME_API __attribute__((visibility(\"default\")))\n"
    "#endif\n"
    "GAME_API void game(void){\n//Register Types Here\n}\n";
    write_file(game_file,Starter);
    char editor_dir[512];
    GAVEN_ASSERT(get_editor_path(editor_dir,sizeof(editor_dir)),"Couldnt get editor path");
    char* last =strrchr(Path,PATH_SEP[0]);
    if(last) *last='\0';
    char* Last_Slash=NULL;
    #ifdef _WIN32
    Last_Slash=strrchr(editor_dir,'\\');
    #else
    Last_Slash=strrchr(editor_dir,'/');
    #endif
    if(Last_Slash){
        *(Last_Slash+1)='\0';
    }
    for(char* P=editor_dir;*P!='\0';P++){
        if(*P=='\\')
            *P='/';
    }

    char cmake_file[512];
    snprintf(cmake_file,sizeof(cmake_file),"%s%s%s",project_dir,PATH_SEP,"CMakeLists.txt");
    char cmake_content[512];
    snprintf(cmake_content,sizeof(cmake_content),
    "cmake_minimum_required(VERSION 3.10)\n\n"
    "file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS \"src/*.c\")\n"
    "set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin/$<CONFIG>/)\n"
    "add_library(%s SHARED ${SOURCES})\n"
    "target_compile_definitions(%s PRIVATE GAME_BUILD_DLL)\n"
    "target_link_libraries(%s \"%slibruneforge.a\")\n"
    "target_include_directories(%s PRIVATE \"%sinclude\")\n",Name,Name,Name,editor_dir,Name,editor_dir);

    write_file(cmake_file,cmake_content);
    open_project(Data->Registry,project_file);
    remove_panel_from_registry(P,P->Registry);
    remove_layer(Data->Registry,Data->Layer);
}
void background_panel_show_create_project(panel_button* Self){
    panel* P=create_popup_panel("Create Project",5,10,110,12);
    background_panel_main_button_data* Main_Data=(background_panel_main_button_data*)Self->Button_Data;
    background_panel_buttons_data* Data = (background_panel_buttons_data*)malloc(sizeof(background_panel_buttons_data));
    Data->Panel=P;
    panel_registry* Reg=Self->Base.Parent->Registry;
    panel_input_text* Input_Name_Field = create_panel_input_text(42,2,32);
    panel_input_text* Input_Path_Field = create_panel_input_text(8,5,100);
    Data->Input_Name_Field=Input_Name_Field;
    Data->Input_Path_Field=Input_Path_Field;
    Data->Registry=Main_Data->Registry;
    Data->Layer=Main_Data->Layer;
    panel_button* Create_Project = create_panel_button(24,9,"Create Project",18,Data,create_project_impl);
    panel_button* Cancle_Create_Project = create_panel_button(64,9,"Cancle Creating",19,Data,cancle_create_project_impl);
    panel_text* Input_Name = create_panel_text("Name:",36,2,101);
    panel_text* Path_Name = create_panel_text("Path:",2,5,101);
    add_element_to_panel(P,&Input_Name_Field->Base);
    add_element_to_panel(P,&Input_Path_Field->Base);
    add_element_to_panel(P,&Create_Project->Base);
    add_element_to_panel(P,&Cancle_Create_Project->Base);
    add_element_to_panel(P,&Input_Name->Base);
    add_element_to_panel(P,&Path_Name->Base);
    add_panel_to_registry(P,Reg);
}
background_panel_element* create_background_panel_element(void){
    background_panel_element* Element =(background_panel_element*)malloc(sizeof(background_panel_element));
    GAVEN_ASSERT(Element,"Couldnt allocat enough memory to Inspector");
    Element->main_sprite_id=load_game_asset("Runeforge.txt",ASSET_TYPE_SPRITE);
    Element->help_sprite_1=create_text("To open a project in runeforge editor",37);
    Element->help_sprite_2 = create_text("you need to right click and press open with",43);
    Element->help_sprite_3 = create_text("choose another app -> choose an app from your pc",48);
    Element->help_sprite_4 = create_text("then find and choose the runeforge editor",41);
    init_panel_element_base(&Element->Base,0,0,NULL,render_background_panel_element,destroy_background_panel_element);
    return Element;
}
panel* create_background_panel(layer_registry* Registry,layer* Layer){
    panel_data p={
        .Name="Background Panel",
        .Background_Char=' ',
        .Height=34,
        .Width=120,
        .Min_Height=33,
        .Min_Width=119,
        .Anchor=1|2|4|8, //anchored all
        .Is_Resizable=0,
        .Z_Index=0
    };
    panel *Background = create_panel(p);
    background_panel_element *E=create_background_panel_element();
    add_element_to_panel(Background,&E->Base);
    background_panel_main_button_data* Main_Button_Data=(background_panel_main_button_data*)malloc(sizeof(background_panel_main_button_data));
    Main_Button_Data->Registry=Registry;
    Main_Button_Data->Layer=Layer;
    panel_button* Create = create_panel_button(95,0,"Create New Project",22,Main_Button_Data,background_panel_show_create_project);
    add_element_to_panel(Background,&Create->Base);
    return Background;
}