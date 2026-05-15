#include "navbar.h"
#include <stdlib.h>
#include <string.h>
#include "../panel_button.h"
#include "scene_explorer.h"
#include "../popup_panel.h"
#include "inspector.h"
typedef struct add_entity_button_data{
    panel* Inspector;
    type_info* Type;
}add_entity_button_data;
typedef struct navbar_data{
    panel* Parent;
    panel* Inspector;
    add_entity_button_data* Button_Data;
    size_t Cap;
    uint8_t State;
} navbar_data;
void navbar_button_save_scene_imple(panel_button* Self){
    if(get_state()&1) return;
    save_scene(NULL,NULL);
}
void navbar_button_start_scene_imple(panel_button* Self){
    uint8_t State = get_state();
    Self->Is_Dirty=1;
    if(State&1){
        Self->Text="Start";
        change_update_state(0);
        return;
    }
    save_scene(NULL,NULL);
    Self->Text="Pause";
    change_update_state(1);
    return;
}
void navbar_add_entity_impl(panel_button* Self){
    add_entity_button_data *Data=(add_entity_button_data*)Self->Button_Data;
    entity *e =create_entity(Data->Type->Name,NULL);
    add_entity(e);
    if(Data->Inspector) inspect_entity(e,Data->Inspector);
    panel* P=Self->Base.Parent;
    remove_panel_from_registry(P,P->Registry);
}
void navbar_show_add_entity_impl(panel_button* Self){
    size_t Count=0;
    type_info** types= Get_Entity_Types(&Count);
    if(!Count) {
        GAVEN_WARN("COULDNT FIND ANY ENTITY TYPES");
        return;
    }
    navbar_data *Data=(navbar_data*)Self->Button_Data;
    if(Count>=Data->Cap){
        Data->Cap=Data->Cap?Data->Cap*2:16;
        add_entity_button_data* temp = (add_entity_button_data*)realloc(Data->Button_Data,Data->Cap*sizeof(add_entity_button_data));
        GAVEN_ASSERT(temp,"Couldnt allocate enough memory to button data");
        Data->Button_Data=temp;
    }
    panel* P=create_popup_panel("Add Entity",40,2,40,30);
    panel_registry* Reg = Self->Base.Parent->Registry;
    for(size_t i=0;i<Count;i++){
        add_entity_button_data* Button_Data = &Data->Button_Data[i];
        Button_Data->Type=types[i];
        Button_Data->Inspector=Data->Inspector;
        panel_button* B=create_panel_button(2,i+1,types[i]->Name,36,Button_Data,navbar_add_entity_impl);
        add_element_to_panel(P,&B->Base);
    }
    add_panel_to_registry(P,Reg);
}
void navbar_add_inspector(panel* Self,panel* Inspector){
    if(!Self||Self->Count<=0) return;
    if(!Inspector) return;
    panel_button* Button = (panel_button*)Self->Elements[0];
    navbar_data *Data = (navbar_data*)Button->Button_Data;
    Data->Inspector=Inspector;
}
panel* create_navbar(void){
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
    Navbar_Data->Button_Data=NULL;
    panel_button *P=create_panel_button(1,0,"Add Entity",14,Navbar_Data,navbar_show_add_entity_impl);
    panel_button *Save=create_panel_button(103,0,"Save Scene",14,Navbar_Data,navbar_button_save_scene_imple);
    panel_button *Start=create_panel_button(55,0,"Start",9,NULL,navbar_button_start_scene_imple);
    add_element_to_panel(Navbar,&P->Base);
    add_element_to_panel(Navbar,&Save->Base);
    add_element_to_panel(Navbar,&Start->Base);
    return Navbar;
}