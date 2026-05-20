
#include "panel_input_text.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
void panel_input_text_destroy(panel_element* Self){
    panel_input_text* Text =(panel_input_text*)Self;
    destroy_sprite(&Text->Sprite);
}
void panel_input_text_render(panel_element* Self){
    panel_input_text* Text =(panel_input_text*)Self;
    if(Text->Is_Dirty){
        destroy_sprite(&Text->Sprite);
        char* buffer= malloc( Text->Width*2+1);
        memset(buffer,'`',(size_t) Text->Width*2);
        size_t Len =strlen(Text->Final_Text);
        if(Len){
            if(Len+2>(size_t)(Text->Width))
                memcpy(buffer+1,Text->Final_Text,Text->Width-2);
            else
                memcpy(buffer+1,Text->Final_Text,Len);
        }
        buffer[0]=buffer[ Text->Width-1]='|';
        memset(buffer+Text->Width,'^',Text->Width);
        Text->Sprite=create_sprite(buffer,Text->Width,2);
        Text->Is_Dirty=0;
    }
    draw_game_overlay_sprite(Text->Sprite,Self->X,Self->Y,Self->Parent->Data.Z_Index+1);
}
void panel_input_text_update(panel_element *Self){
    panel_input_text* Text=(panel_input_text*)Self;
    panel* Panel = Text->Base.Parent;
    if(Panel->Is_Focused){
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            short MX = get_mouse_X()-Panel->X;
            short MY = get_mouse_Y()-Panel->Y;
            if (MX>=Text->Base.X&&MX<Text->Base.X+Text->Width&&MY>=Text->Base.Y&&MY<Text->Base.Y+1&&!Text->Editing){
                Text->Final_Text[0]='\0';
                Text->Editing=1;
                Text->Is_Dirty=1;
            }
            else if(Text->Editing){
                snprintf(Text->Final_Text,256,"%s",Text->Real_Text);
                Text->Is_Dirty=1;
                Text->Editing=0;
            }

        }
        
    }
    else if(Text->Editing){
        snprintf(Text->Final_Text,256,"%s",Text->Real_Text);
        Text->Is_Dirty=1;
        Text->Editing=0;
    }
    if(Panel->Is_Dirty) Text->Is_Dirty=1;
    if(Text->Editing){
        size_t len=strlen(Text->Final_Text);
        if(is_key_just_pressed(RUNEFORGE_KEY_ENTER)){
            snprintf(Text->Real_Text,256,"%s",Text->Final_Text);
            Text->Is_Dirty=1;
            Text->Editing=0;
            return;
        }
        else if(is_key_pressed(RUNEFORGE_KEY_ENTER)){
            return;
        }
        if(is_key_just_pressed(RUNEFORGE_KEY_ESCAPE)){
            snprintf(Text->Final_Text,256,"%s",Text->Real_Text);
            Text->Is_Dirty=1;
            Text->Editing=0;
            return;
        }
        else if(is_key_pressed(RUNEFORGE_KEY_ESCAPE)){
            return;
        }
        if(is_key_just_pressed(RUNEFORGE_KEY_BACKSPACE)){
            if(len>0){
                Text->Final_Text[len-1]='\0';
                Text->Is_Dirty=1;
            }
            return;
        }
        else if(is_key_pressed(RUNEFORGE_KEY_BACKSPACE)){
            return;
        }
        if(is_key_just_pressed(RUNEFORGE_KEY_DELETE))
            return;
        else if(is_key_pressed(RUNEFORGE_KEY_DELETE)){
            return;
        }
        if(is_key_just_pressed(RUNEFORGE_KEY_TAB)){
            if(len<255&&len<(size_t)(Text->Width-1)){
                Text->Final_Text[len]=' ';
                Text->Final_Text[len+1]='\0';
                Text->Is_Dirty=1;
            }
            return;
        }
        else if(is_key_pressed(RUNEFORGE_KEY_TAB)){
            return;
        }
        size_t add_len=0;
        const char* c = get_text_input(&add_len);
        if(add_len&&add_len+len<255&&add_len+len<(size_t)(Text->Width-1)){
            memcpy(Text->Final_Text+len,c,add_len);
            Text->Final_Text[len+add_len]='\0';
            Text->Is_Dirty=1;
        }
    }
    return;
}
panel_input_text* create_panel_input_text(short X, short Y,short Width){
    GAVEN_ASSERT(Width>7,"Input Text Width Should be atleast 8");
    panel_input_text* Text =(panel_input_text*)malloc(sizeof(panel_input_text));
    Text->Sprite.Data=NULL;
    Text->Final_Text[0]=Text->Real_Text[0]='\0';
    Text->Editing=0;
    Text->Width=Width;
    Text->Is_Dirty=1;
    init_panel_element_base(&Text->Base,X,Y,panel_input_text_update,panel_input_text_render,panel_input_text_destroy);
    return Text;
}