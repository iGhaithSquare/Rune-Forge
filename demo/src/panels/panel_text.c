
#include "panel_text.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
void panel_text_destroy(panel_element* Self){
    panel_text* Text =(panel_text*)Self;
    destroy_sprite(&Text->Sprite);
    free(Text);
}
void panel_text_render(panel_element* Self){
    panel_text* Text =(panel_text*)Self;
    if(Text->Is_Dirty){
        destroy_sprite(&Text->Sprite);
        short Width =Text->Base.Parent->Data.Width;
        if(Width>Text->Padding){
            size_t max= Width-Text->Padding;
            size_t len = strlen(Text->Text);
            if(len>max) len=max;
            Text->Sprite=create_text(Text->Text,len);
            Text->Is_Dirty=0;
        }
        else Text->Is_Dirty=2;
    }
    if(!Text->Is_Dirty)
        draw_game_overlay_sprite(Text->Sprite,Self->X,Self->Y,1);
}
void panel_text_update(panel_element *Self){
    panel_text* Text=(panel_text*)Self;
    if (Text->Base.Parent->Is_Dirty)
        Text->Is_Dirty=1;
}
panel_text* create_panel_text(const char* Text,short X, short Y,short Padding){
    panel_text* Element =(panel_text*)malloc(sizeof(panel_text));
    Element->Sprite.Data=NULL;
    Element->Padding=Padding;
    Element->Is_Dirty=1;
    Element->Text=strdup(Text);
    init_panel_element_base(&Element->Base,X,Y,panel_text_update,panel_text_render,panel_text_destroy);
    return Element;
}