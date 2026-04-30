#include "panel_elements.h"
void init_panel_element_base(panel_element* Element,short X,short Y,sprite Sprite,void (*On_Update)(panel_element *Self)){
    Element->X=X;
    Element->Y=Y;
    Element->Sprite=Sprite;
    Element->On_Update=On_Update;
    Element->Width=Sprite.Width;
    Element->Height=Sprite.Height;
}
void destroy_panel_element(panel_element* Self){
    destroy_sprite(&Self->Sprite);
}
void render_panel_element(panel_element* Self){
    draw_game_overlay_sprite(Self->Sprite,Self->X,Self->Y,1);
}