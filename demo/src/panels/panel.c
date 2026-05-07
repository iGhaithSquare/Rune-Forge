#include "panel.h"
#include <stdlib.h>
#include <string.h>
#include "panel_elements.h"


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
    GAVEN_ASSERT(Data.Min_Height>=1,"Pannel minimum height should be atleast 1");

    GAVEN_ASSERT(Data.Min_Width<=Data.Width,"Pannel minimum width should be smaller than or equal to width");
    GAVEN_ASSERT(Data.Min_Height<=Data.Height,"Pannel minimum height should be smaller than or equal to height");
    panel* Panel = (panel*)calloc(1,sizeof(panel));
    GAVEN_ASSERT(Panel,"Couldnt allocate memory to panel");
    
    Panel->Data=Data;
    Panel->Is_Dirty=1;
    Panel->Cap=16;
    Panel->Count=0;
    Panel->Panel_Neighbors.Top_Count=Panel->Panel_Neighbors.Bottom_Count=Panel->Panel_Neighbors.Right_Count=Panel->Panel_Neighbors.Left_Count=0;

    Panel->Elements= (panel_element**)malloc(sizeof(panel_element*)*Panel->Cap);
    Panel->X=40;
    Panel->Y=0;
    GAVEN_ASSERT(Panel->Elements,"Couldnt allocate memory to panel");
    return Panel;
}
void add_panel_to_registry(panel* Panel,panel_registry* Registry){
    if(Registry->Count>=Registry->Cap){
        Registry->Cap=(Registry->Cap?Registry->Cap*2:4);
        panel** temp = (panel**)realloc(Registry->Panels,sizeof(panel*)*Registry->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to panel registry");
        Registry->Panels=temp;
    }
    if(Registry->Count&&Registry->Panels[Registry->Count-1]->Data.Is_Viewport){
        panel* viewport =Registry->Panels[Registry->Count-1];
        Registry->Panels[Registry->Count]=viewport;
        Registry->Panels[Registry->Count-1]=Panel;
        viewport->ID=Registry->Count;
    }
    else
        Registry->Panels[Registry->Count]=Panel;
    Panel->ID=Registry->Count-1;
    Registry->Count++;
    short width = get_window_width();
    short height = get_window_height();
    if(Panel->Data.Anchor&4&&Panel->Panel_Neighbors.Left_Count==0)
        Panel->X=0;
    if(Panel->Data.Anchor&8&&Panel->Panel_Neighbors.Top_Count==0)
        Panel->Y=0;
    if(Panel->Data.Anchor&1&&Panel->Panel_Neighbors.Right_Count==0&&Panel->Data.Is_Resizable&&Panel->X+Panel->Data.Min_Width<width)
        Panel->Data.Width=width-Panel->X;
    if(Panel->Data.Anchor&2&&Panel->Panel_Neighbors.Bottom_Count==0&&Panel->Data.Is_Resizable&&Panel->Y+Panel->Data.Min_Height<height)
        Panel->Data.Height=height-Panel->Y;
}
void remove_panel_from_registry(panel* Panel,panel_registry* Registry){
    if(Panel->ID<Registry->Count-1&&Registry->Panels[Registry->Count-1]->Data.Is_Viewport){
        Registry->Panels[Panel->ID]=Registry->Panels[Registry->Count-2];
    }
    else
        Registry->Panels[Panel->ID]=Registry->Panels[Registry->Count-1];
    Registry->Panels[Panel->ID]->ID=Panel->ID;
    Registry->Count--;
}
/* Direction: 0 for right,1 for bottom 2 for left, 3 for top*/
uint8_t propagate_neighbors(panel* Panel,short new_size,short new_pos,uint8_t direction){
    //first we see how much of the size this panel can take.
    //second we update the panel to the right with the remaining size and new pos, if pos=new_pos dont propagate
    uint8_t check=0;
    switch (direction){
    case 0:
        if(Panel->Data.Anchor&4){
            short min;
            short max= get_window_width()-new_pos;
            if(!Panel->Data.Is_Resizable)
                min=Panel->Data.Width;
            else min=Panel->Data.Min_Width;
            short propagated_size=new_size-Panel->Data.Width;
            if(Panel->Panel_Neighbors.Right_Count==0&&Panel->Data.Is_Resizable){
                if(max<min){
                    return 0;
                }
                Panel->X=new_pos;
                Panel->Data.Width=max;
                Panel->Is_Dirty=1;
                return 1;
            }
            if(Panel->X!=new_pos) {
                if(Panel->Data.Is_Resizable&&new_size>=min){
                    Panel->X=new_pos;
                    Panel->Data.Width=new_size;
                    Panel->Is_Dirty=1;
                    return 1;
                }
                check=0;
            }
            else check=1;
            short neighbor_new_pos = Panel->X+Panel->Data.Width;
            for(int i=0;i<Panel->Panel_Neighbors.Right_Count;i++){
                panel* Neighbor = Panel->Panel_Neighbors.Right[i];
                short neighbor_new_size=Neighbor->Data.Width;
                if(Panel->Data.Is_Resizable)
                    neighbor_new_size-=propagated_size;
                if(!propagate_neighbors(Neighbor,neighbor_new_size,neighbor_new_pos,direction)){
                    return 0;
                }
            }
            
            if (check) Panel->Data.Width=new_size;
            Panel->X=new_pos;
            Panel->Is_Dirty=1;
            return 1;
        }
        return 0;
    case 1:
        if(Panel->Data.Anchor&8){
            short min;
            short max= get_window_height()-new_pos;
            if(!Panel->Data.Is_Resizable)
                min=Panel->Data.Height;
            else min=Panel->Data.Min_Height;
            short propagated_size=new_size-Panel->Data.Height;
            if(Panel->Panel_Neighbors.Bottom_Count==0&&Panel->Data.Is_Resizable){
                if(max<min){
                    max=min;
                    Panel->Y=get_window_height()-Panel->Data.Min_Height;
                    Panel->Data.Height=min;
                    Panel->Is_Dirty=1;
                    return 1;
                }
                Panel->Y=new_pos;
                Panel->Data.Height=max;
                Panel->Is_Dirty=1;
                return 1;
            }
            if(Panel->Y!=new_pos) {
                if(Panel->Data.Is_Resizable&&new_size>=min){
                    Panel->Y=new_pos;
                    Panel->Data.Height=new_size;
                    Panel->Is_Dirty=1;
                    return 1;
                }
                check=0;
            }
            else check=1;
            short neighbor_new_pos = Panel->Y+Panel->Data.Height;
            for(int i=0;i<Panel->Panel_Neighbors.Bottom_Count;i++){
                panel* Neighbor = Panel->Panel_Neighbors.Bottom[i];
                short neighbor_new_size=Neighbor->Data.Height;
                if(Panel->Data.Is_Resizable)
                    neighbor_new_size-=propagated_size;
                if(propagate_neighbors(Neighbor,neighbor_new_size,neighbor_new_pos,direction)){
                    if (check) Panel->Data.Height=new_size;
                    Panel->Y=new_pos;
                    Panel->Is_Dirty=1;
                    return 1;
                }
            }
            return 0;
        }
        return 0;
    case 2:
        if(Panel->Data.Anchor&1){
            short min;
            if(!Panel->Data.Is_Resizable)
                min=Panel->Data.Width;
            else min=Panel->Data.Min_Width;
            short propagated_size=new_size-Panel->Data.Width;
            if(Panel->Panel_Neighbors.Left_Count==0&&Panel->Data.Is_Resizable){
                if(new_size<min){
                    Panel->Data.Width=min;
                }
                else{
                    Panel->Data.Width=new_size;
                }
                Panel->X=0;
                Panel->Is_Dirty=1;
                return 1;
            }
            if(Panel->X!=new_pos) {
                if(Panel->Data.Is_Resizable&&new_size>=min){
                    Panel->X=new_pos;
                    Panel->Data.Width=new_size;
                    Panel->Is_Dirty=1;
                    return 1;
                }
                check=0;
            }
            else check=1;
            for(int i=0;i<Panel->Panel_Neighbors.Left_Count;i++){
                panel* Neighbor = Panel->Panel_Neighbors.Left[i];
                short neighbor_new_size=Neighbor->Data.Width;
                if(Panel->Data.Is_Resizable)
                    neighbor_new_size-=propagated_size;
                short neighbor_new_pos = new_pos-neighbor_new_size;;
                if(propagate_neighbors(Neighbor,neighbor_new_size,neighbor_new_pos,direction)){
                    if (check) Panel->Data.Width=new_size;
                    Panel->X=new_pos;
                    Panel->Is_Dirty=1;
                    return 1;
                }
            }
            return 0;
        }
        return 0;
    }
    
    GAVEN_WARN("Unsupported direction %d",direction);
    return 0;
}
void update_panel(panel* Self){
    panel_data *Data =&Self->Data;
    short mouse_X = get_mouse_X();
    short mouse_Y = get_mouse_Y();
    short end_X = Self->X+Data->Width;
    short end_Y = Self->Y+Data->Height;
    if(Self->Is_Resizing){
        if(is_key_just_released(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            Self->Is_Resizing=0;
        }
        else{
            if(Self->Is_Resizing&1){
                short new_Width=mouse_X-Self->X;
                if(new_Width!=Data->Width&&new_Width>=Data->Min_Width){
                    propagate_neighbors(Self,new_Width,Self->X,0);
                }
            }
            if(Self->Is_Resizing&2){
                short new_Height=mouse_Y-Self->Y;
                if(new_Height!=Data->Height&&new_Height>=Data->Min_Height){
                    Data->Height=new_Height;
                    Self->Is_Dirty=1;
                }
            }
            if(Self->Is_Resizing&4){
                short new_X=mouse_X;
                short new_Width=(Self->X+Data->Width)-mouse_X;
                if(new_Width!=Data->Width&&new_Width>=Data->Min_Width){
                    Self->X=new_X;
                    Data->Width=new_Width;
                    Self->Is_Dirty=1;
                }
            }
            if(Self->Is_Resizing&8){
                short new_Y=mouse_Y;
                short new_Height=(Self->Y+Data->Height)-mouse_Y;
                if(new_Height!=Data->Height&&new_Height>=Data->Min_Height){
                    Self->Y=new_Y;
                    Data->Height=new_Height;
                    Self->Is_Dirty=1;
                }
            }
        }
    }
    if(mouse_X>=Self->X&&mouse_Y>=Self->Y&&mouse_X<=end_X&&mouse_Y<=end_Y){
        Self->Is_Hovered=1;
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            
            Self->Is_Focused=1;
            if(Data->Is_Resizable==1&&!Self->Is_Resizing){
                if(!(Data->Anchor&1)&&mouse_X>end_X-2)
                    Self->Is_Resizing|=1;
                if(!(Data->Anchor&2)&&mouse_Y>end_Y-2)
                    Self->Is_Resizing|=2;
                if(!(Data->Anchor&4)&&mouse_X<Self->X+2)
                    Self->Is_Resizing|=4;
                if(!(Data->Anchor&8)&&mouse_Y<Self->Y+2)
                    Self->Is_Resizing|=8;
            }
        }
    }
    else{
        Self->Is_Hovered=0;
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            Self->Is_Focused=0;
        }
    }
    
    for(size_t i=0;i<Self->Count;i++){
        panel_element* Element = Self->Elements[i];
        GAVEN_ASSERT(Element,"Element not found");
        if(Element->On_Update)
            Element->On_Update(Element);
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
            destroy_sprite(&Self->Background_Sprite);
            free(Self->Background_String);
        }
        Self->Background_String=(char*)malloc(Size+1);
        memset(Self->Background_String,Self->Data.Background_Char,Size);
        Self->Background_String[Size]='\0';
        Self->Background_Sprite=create_sprite(Self->Background_String,Self->Data.Width,Self->Data.Height);
        Self->Is_Dirty=0;
    }
    set_panel_offset(Self->X,Self->Y);
    draw_game_overlay_sprite(Self->Background_Sprite,0,0,0);
    for(size_t i=0;i<Self->Count;i++){
        panel_element* Element = Self->Elements[i];
        if(Element->On_Render) Element->On_Render(Element);
    }
}
void render_panels(panel_registry* Registry){
    for(size_t i =0;i<Registry->Count;i++)
        render_panel(Registry->Panels[i]);
}

void add_element_to_panel(panel* Panel,panel_element* Element){
    if(Panel->Count>=Panel->Cap){
        Panel->Cap=(Panel->Cap?Panel->Cap*2:4);
        panel_element** temp = (panel_element**)realloc(Panel->Elements,sizeof(panel_element*)*Panel->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to panel registry");
        Panel->Elements=temp;
    }
    Element->Parent=Panel;
    Panel->Elements[Panel->Count]=Element;
    Element->ID=Panel->Count++;
}
void remove_element_from_panel(panel* Panel,panel_element* Element){
    Panel->Elements[Element->ID]=Panel->Elements[Panel->Count-1];
    Panel->Elements[Element->ID]->ID=Element->ID;
    Panel->Count--;
    if(Element->On_Destroy) Element->On_Destroy(Element);
}
uint8_t add_panel_neighbor(panel* Panel,panel* Neighbor,uint8_t direction){
    panel_neighbors* Neighbors_P = &Panel->Panel_Neighbors;
    panel_neighbors* Neighbors_N = &Neighbor->Panel_Neighbors;
    switch (direction){
    case 0:
        if(Neighbors_P->Right_Count>=3||Neighbors_N->Left_Count>=3){
            return 0;
        }
        if(Neighbor->Data.Anchor&4)
            Neighbor->X=Panel->X+Panel->Data.Width;
        else if(Panel->Data.Anchor&1&&Panel->Data.Is_Resizable){
            Panel->Data.Width=Neighbor->X-Panel->X;
        }
        else return 0;
        Neighbors_P->Right[Neighbors_P->Right_Count] = Neighbor;
        Neighbors_N->Left[Neighbors_N->Left_Count] = Panel;
        Neighbors_P->Right_Count++;
        Neighbors_N->Left_Count++;
        return 1;
    case 1:
        if(Neighbors_P->Bottom_Count>=3||Neighbors_N->Top_Count>=3){
            return 0;
        }
        if(Neighbor->Data.Anchor&2)
            Neighbor->Y=Panel->Y+Panel->Data.Height;
        else if(Panel->Data.Anchor&8&&Panel->Data.Is_Resizable){
            Panel->Data.Height=Neighbor->Y-Panel->Y;
        }
        else return 0;
        Neighbors_P->Bottom[Neighbors_P->Bottom_Count] = Neighbor;
        Neighbors_N->Top[Neighbors_N->Top_Count] = Panel;
        Neighbors_P->Bottom_Count++;
        Neighbors_N->Top_Count++;
        return 1;
    case 2:
        if(Neighbors_N->Right_Count>=3||Neighbors_P->Left_Count>=3){
            return 0;
        }
        if(Panel->Data.Anchor&4)
            Panel->X=Neighbor->X+Neighbor->Data.Width;
        else if(Neighbor->Data.Anchor&1&&Neighbor->Data.Is_Resizable){
            Neighbor->Data.Width=Panel->X-Neighbor->X;
        }
        else return 0;
        Neighbors_N->Right[Neighbors_N->Right_Count] = Panel;
        Neighbors_P->Left[Neighbors_P->Left_Count] = Neighbor;
        Neighbors_N->Right_Count++;
        Neighbors_P->Left_Count++;
        return 1;
    case 3:
        if(Neighbors_N->Bottom_Count>=3||Neighbors_P->Top_Count>=3){
            return 0;
        }
        if(Panel->Data.Anchor&8)
            Panel->Y=Neighbor->Y+Neighbor->Data.Height;
        else if(Neighbor->Data.Anchor&2&&Neighbor->Data.Is_Resizable){
            Neighbor->Data.Height=Panel->Y-Neighbor->Y;
        }
        else return 0;
        Neighbors_N->Bottom[Neighbors_N->Bottom_Count] = Panel;
        Neighbors_P->Top[Neighbors_P->Top_Count] = Neighbor;
        Neighbors_N->Bottom_Count++;
        Neighbors_P->Top_Count++;
        return 1;
    default:
        GAVEN_WARN("Direction doesnt exist");
        return 0;
    }
}