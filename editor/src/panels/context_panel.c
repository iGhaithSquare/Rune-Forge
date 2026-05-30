#include "context_panel.h"
#include "panel_elements.h"
#include <stdlib.h>
void update_context_panel(panel_element* Self){
    panel* P =Self->Parent;
    if(P!=P->Registry->Focused){
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)||is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_RIGHT)){
            P->Remove=1;
        }
    }
}

panel* create_context_panel(const char* Name,short Width,short Panel_Count){
    panel_data Data = {
        .Name=Name,
        .Anchor=0,
        .Background_Char=':',
        .Height=Panel_Count,
        .Width=Width,
        .Is_Resizable=0,
        .Z_Index=2,
        .Min_Height=Panel_Count-1,
        .Min_Width=Width-1
    };
    panel* Context = create_panel(Data);
    Context->X=get_mouse_X();
    Context->Y=get_mouse_Y();
    panel_element* E=malloc(sizeof(panel_element));
    init_panel_element_base(E,0,0,update_context_panel,NULL,NULL);
    add_element_to_panel(Context,E);
    return Context;
}
