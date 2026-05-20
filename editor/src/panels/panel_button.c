
#include "panel_button.h"
#include <string.h>
#include <stdlib.h>

void button_destroy(panel_element* Self){
    panel_button* Button =(panel_button*)Self;
    destroy_sprite(&Button->Sprite);
}
char *create_text_buffer(const char* Text,short *length){
    short len =strlen(Text);
    if(len>*length){
        GAVEN_ASSERT(*length<255,"Length bigger than the limit 255");
        GAVEN_ASSERT(*length>2,"Length smaller than size limit 3");
        char *buffer=malloc(*length+1);

        memcpy(buffer,Text,*length-3);
        buffer[*length-3]='.';
        buffer[*length-2]='.';
        buffer[*length-1]='.';
        buffer[*length]='\0';
        return buffer;
    }
    *length=len;
    return NULL;
}
void button_render(panel_element* Self){
    panel_button* Button =(panel_button*)Self;
    if(Button->Is_Dirty){
        short len = Button->Width-4;
        char* buffer= malloc( Button->Width+1);
        memset(buffer,'`',(size_t) Button->Width);
        buffer[0]=buffer[ Button->Width-1]='|';
        char* temp =create_text_buffer( Button->Text,&len);
        if(temp){
            memcpy(buffer+2,temp,len);
            free(temp);
        }
        else
            memcpy(buffer+2, Button->Text,len);
        Button->Sprite=create_sprite(buffer,Button->Width,1);
        Button->Is_Dirty=0;
    }
    draw_game_overlay_sprite(Button->Sprite,Self->X,Self->Y,Self->Parent->Data.Z_Index+1);
}
void button_update(panel_element *Self){    
    panel_button* Button =(panel_button*)Self;
    short MX= get_mouse_X() - Self->Parent->X;
    short MY= get_mouse_Y() - Self->Parent->Y;
    if(MX<Self->X||MY<Self->Y||MX>=Self->X+Button->Width||MY>=Self->Y+1){
        Button->Is_Held=0;
        return;
    }
    if(is_key_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
        Button->Is_Held=1;
    }
    if(Button->Is_Held&&is_key_just_released(RUNEFORGE_MOUSE_BUTTON_LEFT)){
        Button->Is_Held=0;
        if(Button->On_Click)
            Button->On_Click(Button);
    }
}
panel_button* create_panel_button(short X, short Y,const char* Text,short Width,void* Button_Data,void (*On_Click)(struct panel_button* Self)){
    GAVEN_ASSERT(Width>7,"Button Width Should be atleast 8");
    
    panel_button* Button =(panel_button*)malloc(sizeof(panel_button));
    Button->Sprite.Data=NULL;
    Button->Text = strdup(Text);
    Button->Is_Held=0;
    Button->On_Click=On_Click;
    Button->Width=Width;
    Button->Height=1;
    Button->Button_Data=Button_Data;
    Button->Is_Dirty=1;
    init_panel_element_base(&Button->Base,X,Y,button_update,button_render,button_destroy);
    return Button;
}