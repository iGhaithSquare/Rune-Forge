#include "background_panel.h"
#include "../panel_button.h"
#include <stdlib.h>
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
    draw_game_sprite(get_game_sprite(be->main_sprite_id),0,0,0);
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
void background_panel_show_create_prj_impl(panel_button* Self){
    return;
}
panel* create_background_panel(void){
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
    return Background;
}