#include "panel.h"
#include <stdlib.h>
#include <string.h>
panel_registry* create_panel_registry(){
    panel_registry* Registry = (panel_registry*)malloc(sizeof(panel_registry));
    GAVEN_ASSERT(Registry,"Couldnt allocate memory to panel registry");
    Registry->Panels=NULL;
    Registry->Cap=0;
    Registry->Count=0;
    return Registry;
}
void destroy_panel_registry(panel_registry* Self){
    if(!Self) return;
    for(size_t i=0;i<Self->Count;i++){
        destroy_sprite(&Self->Panels[i]->Background_Sprite);
        free(Self->Panels[i]->Background_String);
        free(Self->Panels[i]);
    }
    if(Self->Panels) free(Self->Panels);
    free(Self);
}

panel* create_panel(panel_data Data){
    GAVEN_ASSERT(Data.Min_Width>=5,"Pannel minimum width should be atleast 5");
    GAVEN_ASSERT(Data.Min_Height>=3,"Pannel minimum height should be atleast 3");

    GAVEN_ASSERT(Data.Min_Width<Data.Width,"Pannel minimum width should be smaller than width");
    GAVEN_ASSERT(Data.Min_Height<Data.Height,"Pannel minimum height should be smaller than height");
    panel* Panel = (panel*)malloc(sizeof(panel));
    GAVEN_ASSERT(Panel,"Couldnt allocate memory to panel");
    Panel->Data=Data;
    Panel->Is_Dirty=1;
    Panel->Is_Dragging=Panel->Is_Focused=Panel->Is_Hovered=Panel->Is_Resizing=0;
    return Panel;
}
void add_panel_to_registry(panel* Panel,panel_registry* Registry){
    if(Registry->Count>=Registry->Cap){
        Registry->Cap=(Registry->Cap?Registry->Cap*2:4);
        panel** temp = (panel**)realloc(Registry->Panels,sizeof(panel*)*Registry->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to panel registry");
        Registry->Panels=temp;
    }
    Registry->Panels[Registry->Count]=Panel;
    Panel->ID=Registry->Count++;
}
void remove_panel_from_registry(panel* Panel,panel_registry* Registry){
    Registry->Panels[Panel->ID]=Registry->Panels[Registry->Count-1];
    Registry->Panels[Panel->ID]->ID=Panel->ID;
    Registry->Count--;
}
void update_panel(panel* Self){
    panel_data *Data =&Self->Data;
    short mouse_X = get_mouse_X();
    short mouse_Y = get_mouse_Y();
    short end_X = Data->X+Data->Width;
    short end_Y = Data->Y+Data->Height;
    if(Self->Is_Dragging){
        if(is_key_just_released(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            Self->Is_Dragging=0;
        }
        else{
            if(mouse_X!=Data->X){
                Data->X=mouse_X;
                Self->Is_Dirty=1;
            }if(mouse_Y!=Data->Y){
                Data->Y=mouse_Y;
                Self->Is_Dirty=1;
            }

        }
    }
    else if(Self->Is_Resizing){
        if(is_key_just_released(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            Self->Is_Resizing=0;
        }
        else{
            if(Self->Is_Resizing&1&&mouse_X!=Data->Width+Data->X&&mouse_X>=Data->Min_Width+Data->X){
                Data->Width=mouse_X-Data->X;
                Self->Is_Dirty=1;
            }
            if(Self->Is_Resizing&2&&mouse_Y!=Data->Height+Data->Y&&mouse_Y>=Data->Min_Height+Data->Y){
                Data->Height=mouse_Y-Data->Y;
                Self->Is_Dirty=1;
            }

        }
        
    }
    if(mouse_X>=Data->X&&mouse_Y>=Data->Y&&mouse_X<=end_X&&mouse_Y<=end_Y){
        Self->Is_Hovered=1;
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            
            Self->Is_Focused=1;
            if(Data->Is_Draggable&&mouse_X<Data->X+2&&mouse_Y<Data->Y+2){
                Self->Is_Dragging=1;
            }else if(Data->Is_Resizable==1&&!Self->Is_Resizing){
                if(mouse_X>end_X-2)
                    Self->Is_Resizing|=1;
                if(mouse_Y>end_Y-2)
                    Self->Is_Resizing|=2;

            }
        }
    }
    else{
        Self->Is_Hovered=0;
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            Self->Is_Focused=0;
        }
    }
}
void update_panels(panel_registry* Registry){
    for(size_t i =0;i<Registry->Count;i++)
        update_panel(Registry->Panels[i]);
}

void render_panel(panel* Self){
    int Size=(int)Self->Data.Width*Self->Data.Height;
    if(Self->Is_Dirty){
        if(Self->Background_String){
            free(Self->Background_String);
            destroy_sprite(&Self->Background_Sprite);
        }
        Self->Background_String=(char*)malloc(Size+1);
        memset(Self->Background_String,Self->Data.Background_Char,Size);
        Self->Background_String[Size]='\0';
        Self->Background_Sprite=create_sprite(Self->Background_String,Self->Data.Width,Self->Data.Height);
        Self->Is_Dirty=0;
    }
    draw_game_overlay_sprite(Self->Background_Sprite,Self->Data.X,Self->Data.Y,0);
}
void render_panels(panel_registry* Registry){
    for(size_t i =0;i<Registry->Count;i++)
        render_panel(Registry->Panels[i]);
}