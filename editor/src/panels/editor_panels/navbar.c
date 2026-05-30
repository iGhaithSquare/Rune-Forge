#include "navbar.h"
#include <stdlib.h>
#include <string.h>
#include "../panel_button.h"
#include "../panel_input_text.h"
#include "../panel_text.h"
#include "scene_explorer.h"
#include "../popup_panel.h"
#include "inspector.h"
#include <stdio.h>
typedef struct navbar_export_buttons_data{
    panel* Panel;
    panel_input_text* Input_Path_Field;
    editor* Editor;
}navbar_export_buttons_data;
typedef struct navbar_data{
    panel* Parent;
    panel* Inspector;
    size_t Cap;
    uint8_t State;
    editor* Editor;
} navbar_data;
void navbar_button_save_scene_imple(panel_button* Self){
    if(get_state()&1) return;
    save_scene(NULL);
}
void navbar_button_start_scene_imple(panel_button* Self){
    navbar_data* Data=(navbar_data*)Self->Button_Data;
    uint8_t State = get_state();
    Self->Is_Dirty=1;
    if(State&1){
        Self->Text="Start";
        change_update_state(0);
        return;
    }
    uninspect_inspector_panel(Data->Inspector);
    save_scene(NULL);
    Self->Text="Pause";
    change_update_state(1);
    return;
}
void navbar_cancle_popup_panel(panel_button* Self){
    navbar_export_buttons_data *Data=(navbar_export_buttons_data*)Self->Button_Data;
    panel* P=Self->Base.Parent;
    P->Remove=1;
}
void navbar_export_impl(panel_button* Self){
    navbar_export_buttons_data *Data=(navbar_export_buttons_data*)Self->Button_Data;
    panel* P=Self->Base.Parent;
    
    const char* Path_Old = Data->Input_Path_Field->Real_Text;
    if(!Path_Old||!strlen(Path_Old)){
        P->Remove=1;
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
    editor* Editor=Data->Editor;
    const char *prj_name = Editor->project_name;
    const char *prj_path = Editor->project_root_path;
    char export_dir[512];
    snprintf(export_dir,sizeof(export_dir),"%s%s%s%s",Path,PATH_SEP,"bin_",prj_name);
    create_new_folder(export_dir);

    char editor_dir[512];
    get_editor_path(editor_dir,sizeof(editor_dir));
    
    char runtime_file[512];
    char prj_runtime_file[512];
    #ifdef _WIN32
    snprintf(runtime_file,sizeof(runtime_file),"%s%s%s%s%s",editor_dir,PATH_SEP,"runtime",PATH_SEP,"runtime.exe");
    snprintf(prj_runtime_file,sizeof(prj_runtime_file),"%s%s%s%s",export_dir,PATH_SEP,prj_name,".exe");
    #else
    snprintf(runtime_file,sizeof(runtime_file),"%s%s%s%s%s",editor_dir,PATH_SEP,"runtime",PATH_SEP,"runtime");
    snprintf(prj_runtime_file,sizeof(prj_runtime_file),"%s%s%s",export_dir,PATH_SEP,prj_name);
    #endif
    copy_file(runtime_file,prj_runtime_file);

    char asset_dir[512];
    snprintf(asset_dir,sizeof(asset_dir),"%s%s%s",prj_path,PATH_SEP,"assets");
    char prj_asset_dir[512];
    snprintf(prj_asset_dir,sizeof(prj_asset_dir),"%s%s%s",export_dir,PATH_SEP,"assets");
    copy_dir_to_dir(asset_dir,prj_asset_dir);

    const char *project_file=Editor->project_file_path;
    char prj_cfg_file[512];
    snprintf(prj_cfg_file,sizeof(prj_cfg_file),"%s%s%s",export_dir,PATH_SEP,"prj.cfg");
    copy_file(project_file,prj_cfg_file);

    char prjdll_file[512];
    char runeforgedll_file[512];
    char runewalldll_file[512];
    char gavendll_file[512];
    #ifdef _WIN32
    snprintf(prjdll_file,sizeof(prjdll_file),"%s%s%s%s%s%s%s",prj_path,PATH_SEP,"bin",PATH_SEP,"Rlib",prj_name,".dll");
    snprintf(runeforgedll_file,sizeof(runeforgedll_file),"%s%s%s",editor_dir,PATH_SEP,"libruneforge.dll");
    snprintf(runewalldll_file,sizeof(runewalldll_file),"%s%s%s",editor_dir,PATH_SEP,"librunewall.dll");
    snprintf(gavendll_file,sizeof(gavendll_file),"%s%s%s",editor_dir,PATH_SEP,"libgaven.dll");
    #else
    snprintf(prjdll_file,sizeof(prjdll_file),"%s%s%s%s%s%s%s",prj_path,PATH_SEP,"bin",PATH_SEP,"Rlib",prj_name,".so");
    snprintf(runeforgedll_file,sizeof(runeforgedll_file),"%s%s%s",editor_dir,PATH_SEP,"libruneforge.so");
    snprintf(runewalldll_file,sizeof(runewalldll_file),"%s%s%s",editor_dir,PATH_SEP,"librunewall.so");
    snprintf(gavendll_file,sizeof(gavendll_file),"%s%s%s",editor_dir,PATH_SEP,"libgaven.so");
    #endif
    copy_file_to_dir(prjdll_file,export_dir);
    copy_file_to_dir(runeforgedll_file,export_dir);
    copy_file_to_dir(runewalldll_file,export_dir);
    copy_file_to_dir(gavendll_file,export_dir);
    P->Remove=1;
}
void navbar_show_export_impl(panel_button* Self){
    panel* P=create_popup_panel("Export",7,10,110,6);
    navbar_export_buttons_data* Data = (navbar_export_buttons_data*)malloc(sizeof(navbar_export_buttons_data));
    navbar_data* Main_Button_Data =(navbar_data*)Self->Button_Data;
    Data->Panel=P;
    panel_registry* Reg=Self->Base.Parent->Registry;
    panel_input_text* Input_Export_Path = create_panel_input_text(12,2,97);
    Data->Input_Path_Field=Input_Export_Path;
    Data->Editor=Main_Button_Data->Editor;
    panel_button* Create_Project = create_panel_button(24,4,"Export Project",18,Data,navbar_export_impl);
    panel_button* Cancle_Create_Project = create_panel_button(64,4,"Cancle Exporting",20,Data,navbar_cancle_popup_panel);
    panel_text* Input_Export = create_panel_text("Export To:",1,2,100);
    add_element_to_panel(P,&Input_Export_Path->Base);
    add_element_to_panel(P,&Create_Project->Base);
    add_element_to_panel(P,&Cancle_Create_Project->Base);
    add_element_to_panel(P,&Input_Export->Base);
    add_panel_to_registry(P,Reg);
}
void navbar_add_inspector(panel* Self,panel* Inspector){
    if(!Self||Self->Count<=0) return;
    if(!Inspector) return;
    panel_button* Button = (panel_button*)Self->Elements[0];
    navbar_data *Data = (navbar_data*)Button->Button_Data;
    Data->Inspector=Inspector;
}
panel* create_navbar(editor* Editor){
    panel_data Top={
        .Name="Navbar",
        .Background_Char=',',
        .Height=1,
        .Width=120,
        .Min_Height=1,
        .Min_Width=119,
        .Anchor=1|4|8, //anchored all
        .Is_Resizable=0,
        .Z_Index=0
    };
    panel *Navbar = create_panel(Top);
    navbar_data* Navbar_Data= (navbar_data*)malloc(sizeof(navbar_data));
    GAVEN_ASSERT(Navbar_Data,"Couldnt create navbar");
    Navbar_Data->Cap=0;
    Navbar_Data->Editor=Editor;
    panel_button *Save=create_panel_button(103,0,"Save Scene",14,Navbar_Data,navbar_button_save_scene_imple);
    panel_button *Start=create_panel_button(55,0,"Start",9,Navbar_Data,navbar_button_start_scene_imple);
    panel_button *Export=create_panel_button(93,0,"Export",10,Navbar_Data,navbar_show_export_impl);
    add_element_to_panel(Navbar,&Save->Base);
    add_element_to_panel(Navbar,&Start->Base);
    add_element_to_panel(Navbar,&Export->Base);
    return Navbar;
}