#include "scene_explorer.h"
#include <stdlib.h>
#include <string.h>
#include "inspector.h"
typedef struct entity_node{
    sprite Text_Sprite;
    entity* Entity;
    uint8_t Is_Dirty;
}entity_node;
typedef struct scene_explorer_element{
    panel_element Base;
    entity_node* Selected;
    entity_node** Entities;
    panel* Inspector;
    size_t Count;
    size_t Cap;
    uint8_t Is_Dirty;
    //todo add this : entity_registry* Registry;
}scene_explorer_element;
void scene_explorer_point_to_inspector(panel* Scene_Explorer,panel* Inspector){
    if(!Scene_Explorer||Scene_Explorer->Cap<=0) return;
    scene_explorer_element* se = (scene_explorer_element*)Scene_Explorer->Elements[0];
    se->Inspector=Inspector;
}
void update_scene_explorer_element(panel_element* Self){
    scene_explorer_element* se = (scene_explorer_element*)Self;
    panel* Panel = Self->Parent;
    if(Panel->Is_Focused&&Panel->Is_Hovered){
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            short MX = get_mouse_X()-Panel->X;
            short MY = get_mouse_Y()-Panel->Y;
            int offset=1;
            entity_node* Node = NULL;
            if(MY-offset>=0&&MY-offset<(short)(se->Count)){
                Node=se->Entities[MY-offset];
            }
            if (Node&&MX>=3&&MX<Node->Text_Sprite.Width+3&&se->Selected!=Node){
                if(se->Selected==Node){
                    //todo add editing name support
                }
                else{
                    se->Selected = Node;
                    if(se->Inspector){
                        inspect_entity(Node->Entity,se->Inspector);
                    }
                }
            }

        }
        
    }
    if(Panel->Is_Dirty)
        se->Is_Dirty=1;
}
void render_scene_explorer_element(panel_element* Self){
    scene_explorer_element* se = (scene_explorer_element*)Self;
    for (size_t i=0;i<se->Count;i++){
        entity_node *Node=se->Entities[i];
        if(Node->Is_Dirty||se->Is_Dirty){
            destroy_sprite(&Node->Text_Sprite);
            Node->Text_Sprite=create_text(Node->Entity->Name,se->Base.Parent->Data.Width-5);
            Node->Is_Dirty=0;
        }   
        draw_game_overlay_sprite(Node->Text_Sprite,3,i+1,1);
    }
    se->Is_Dirty=0;
}
void destroy_scene_explorer_element(panel_element* Self){
    scene_explorer_element* se = (scene_explorer_element*)Self;
    for(size_t i=0;i<se->Count;i++){
        destroy_sprite(&se->Entities[i]->Text_Sprite);
        free(se->Entities[i]);
    }
    free(se->Entities);
    free(se);
    return;
}
scene_explorer_element* create_scene_explorer_element(void){
    scene_explorer_element* Element =(scene_explorer_element*)malloc(sizeof(scene_explorer_element));
    GAVEN_ASSERT(Element,"Couldnt allocat enough memory to scene explorer");
    Element->Cap=0;
    Element->Count=0;
    Element->Is_Dirty=0;
    Element->Entities=NULL;
    Element->Selected=NULL;
    Element->Inspector=NULL;
    init_panel_element_base(&Element->Base,0,0,update_scene_explorer_element,render_scene_explorer_element,destroy_scene_explorer_element);
    return Element;
}
void add_entity_to_scene_explorer(entity* Entity,panel* Scene_Explorer){
    if(!Scene_Explorer||Scene_Explorer->Count<=0) return;
    scene_explorer_element* se = (scene_explorer_element*)Scene_Explorer->Elements[0];
    if(se->Count>=se->Cap){
        se->Cap=se->Cap?se->Cap*2:16;
        entity_node** temp= (entity_node**)realloc(se->Entities,sizeof(entity_node*)*se->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to entity node");
        se->Entities=temp;
    }
    entity_node* Entity_Node = (entity_node*)malloc(sizeof(entity_node));
    GAVEN_ASSERT(Entity_Node,"Couldnt assign memory to entity node");
    Entity_Node->Entity=Entity;
    Entity_Node->Is_Dirty=1;
    Entity_Node->Text_Sprite.Data=NULL;
    se->Entities[se->Count++]=Entity_Node;
}
panel* create_scene_explorer(void){
    panel_data Scene_Expo={
        .Name="Scene explorer",
        .Background_Char='_',
        .Height=10,
        .Width=20,
        .Min_Height=4,
        .Min_Width=10,
        .Anchor=2|4|8, //anchored top left 
        .Is_Resizable=1,
        .Z_Index=0
    };
    panel *Scene_Explorer = create_panel(Scene_Expo);
    scene_explorer_element *E=create_scene_explorer_element();
    add_element_to_panel(Scene_Explorer,&E->Base);
    return Scene_Explorer;
}