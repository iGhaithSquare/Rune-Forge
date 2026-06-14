#include "scene_explorer.h"
#include <stdlib.h>
#include <string.h>
#include "inspector.h"
#include "../context_panel.h"
#include "../panel_button.h"
#include "../popup_panel.h"
#include "../panel_input_text.h"
#include "../panel_text.h"
#include "file_explorer.h"
#include <stdio.h>
typedef struct scene_expo_make_scene{
    entity* Selected;
    panel_input_text* Path;
}scene_expo_make_scene;
typedef struct scene_expo_rename{
    entity* Selected;
    panel_input_text* Path;
    entity_registry* Reg;
}scene_expo_rename;
typedef struct entity_node{
    sprite Text_Sprite;
    entity* Entity;
    int depth;
    uint8_t Is_Dirty;
}entity_node;
typedef struct scene_add_entity_button{
    panel* Inspector;
    type_info* Type;
    entity* Selected;
    entity_registry* Reg;
}scene_add_entity_button;
typedef struct scene_explorer_element{
    panel_element Base;
    entity_node* Selected;
    entity_node *Selected_Right;
    entity_node** Entities;
    panel* Inspector;
    size_t Count;
    size_t Cap;
    scene_add_entity_button* Add_Entity_Button;
    size_t Add_Entity_Cap;
    uint8_t Is_Dirty;
    size_t Version;
    entity_registry* Registry;
}scene_explorer_element;
void scene_explorer_delete_button_impl(panel_button* Self){
    scene_explorer_element* E=(scene_explorer_element*)Self->Button_Data;
    if(E&&E->Inspector){
        uninspect_inspector_panel(E->Inspector);
    }
    E->Selected=NULL;
    entity* Entity=E->Selected_Right->Entity;
    free_child(Entity->Parent,Entity);
    E->Selected_Right=NULL;
    E->Registry->Version++;
    panel* P=Self->Base.Parent;
    P->Remove=1;
}
void scene_add_child_impl(panel_button* Self){
    scene_add_entity_button *Data=(scene_add_entity_button*)Self->Button_Data;
    entity *e =create_entity(Data->Selected,Data->Type->Name,NULL,NULL);
    if(Data->Inspector) inspect_entity(e,Data->Inspector);
    Data->Reg->Version++;
    panel* P=Self->Base.Parent;
    P->Remove=1;
}
void scene_cancle_popup_panel(panel_button* Self){
    scene_add_entity_button *Data=(scene_add_entity_button*)Self->Button_Data;
    panel* P=Self->Base.Parent;
    P->Remove=1;
}
void scene_explorer_add_child_impl(panel_button* Self){
    scene_explorer_element* E=(scene_explorer_element*)Self->Button_Data;
    panel* Panel=Self->Base.Parent;
    size_t Count=0;
    type_info** types= Get_Entity_Types(&Count);
    if(!Count) {
        GAVEN_WARN("COULDNT FIND ANY ENTITY TYPES");
        return;
    }
    if(Count>=E->Add_Entity_Cap){
        E->Add_Entity_Cap=E->Add_Entity_Cap?E->Add_Entity_Cap*2:16;
        scene_add_entity_button* temp = (scene_add_entity_button*)realloc(E->Add_Entity_Button,E->Add_Entity_Cap*sizeof(scene_add_entity_button));
        GAVEN_ASSERT(temp,"Couldnt allocate enough memory to button data");
        E->Add_Entity_Button=temp;
    }
    panel* P=create_popup_panel("Add Entity",30,2,50,30);
    panel_registry* Reg = Self->Base.Parent->Registry;
    for(size_t i=0;i<Count;i++){
        scene_add_entity_button* Button_Data = &E->Add_Entity_Button[i];
        Button_Data->Type=types[i];
        Button_Data->Inspector=E->Inspector;
        Button_Data->Selected=E->Selected_Right?E->Selected_Right->Entity:E->Registry->Root;
        Button_Data->Reg=E->Registry;
        panel_button* B=create_panel_button(1,i+1,types[i]->Name,48,Button_Data,scene_add_child_impl);
        add_element_to_panel(P,&B->Base);
    }
    panel_button* Button = create_panel_button(20,28,"Cancle",10,P,scene_cancle_popup_panel);
    add_element_to_panel(P,&Button->Base);
    add_panel_to_registry(P,Reg);
    Panel->Remove=1;
}
void cancel_scene_explorer_make_scene(panel_button* Self){
    Self->Base.Parent->Remove=1;
}
void save_scene_explorer_make_scene(panel_button* Self){
    scene_expo_make_scene* Data= (scene_expo_make_scene*)Self->Button_Data;
    entity_registry* E=create_entity_registry();
    entity* Ent=Data->Selected;
    free_child(NULL,E->Root);
    E->Root=Ent;
    char scene_file[512];
    snprintf(scene_file,sizeof(scene_file),"%s%s%s",get_asset_dir(),Data->Path->Real_Text,".jsonscn");
    serialize_entity_registry(scene_file,E);
    E->Root=NULL;
    Ent->Path =strdup(scene_file);
    destroy_entity_registry(E);
    save_scene(NULL);
    unload_scene();
    load_scene(scene_file);
    Self->Base.Parent->Remove=1;
}

void rename_entity_scene_explorer(panel_button* Self){
    scene_expo_rename* Data= (scene_expo_rename*)Self->Button_Data;
    Data->Selected->Name=Data->Path->Real_Text;
    Self->Base.Parent->Remove=1;
    Data->Reg->Version++;
}

void scene_explorer_show_rename_entity_impl(panel_button* Self){
    scene_explorer_element* E=(scene_explorer_element*)Self->Button_Data;
    panel* Panel=Self->Base.Parent;
    panel* P=create_popup_panel("Rename Entity",7,10,110,6);
    scene_expo_rename* Rename= (scene_expo_rename*)malloc(sizeof(scene_expo_make_scene));
    panel_text* Input_Export = create_panel_text("Rename To:",1,2,99);
    panel_input_text* Input_Path_Field =create_panel_input_text(12,2,97);
    entity *Entity=E->Selected_Right->Entity;
    snprintf(Input_Path_Field->Final_Text,sizeof(Input_Path_Field->Final_Text),"%s",Entity->Name);
    Rename->Path=Input_Path_Field;
    Rename->Selected=Entity;
    Rename->Reg=E->Registry;
    panel_button* Save_Project = create_panel_button(24,4,"Rename",18,Rename,rename_entity_scene_explorer);
    panel_button* Cancle_Save_Project = create_panel_button(64,4,"Cancle",20,Rename,cancel_scene_explorer_make_scene);
    panel_registry* Reg = Self->Base.Parent->Registry;
    add_element_to_panel(P,&Input_Export->Base);
    add_element_to_panel(P,&Input_Path_Field->Base);
    add_element_to_panel(P,&Save_Project->Base);
    add_element_to_panel(P,&Cancle_Save_Project->Base);
    add_panel_to_registry(P,Reg);
    uninspect_inspector_panel(E->Inspector);
    Panel->Remove=1;
}

void scene_add_subscene_as_child_impl(panel_button* Self){
    panel* P=Self->Base.Parent;
    P->Remove=1;
    return;
}
void scene_explorer_add_subscene_impl(panel_button* Self){
    scene_explorer_element* E=(scene_explorer_element*)Self->Button_Data;
    panel* Panel=Self->Base.Parent;
    panel* P=create_popup_panel("Add Scene",7,10,110,6);
    scene_expo_make_scene* Add_Scene= (scene_expo_make_scene*)malloc(sizeof(scene_expo_make_scene));
    panel_text* Input_Export = create_panel_text("Add From:",1,2,99);
    panel_input_text* Input_Path_Field =create_panel_input_text(11,2,98);
    Add_Scene->Path=Input_Path_Field;
    Add_Scene->Selected=E->Selected_Right->Entity;
    panel_button* Load_Project = create_panel_button(24,4,"Add Scene",18,Add_Scene,scene_add_subscene_as_child_impl);
    panel_button* Cancle_Loading_Project = create_panel_button(64,4,"Cancle Loading",20,Add_Scene,scene_cancle_popup_panel);
    panel_registry* Reg = Self->Base.Parent->Registry;
    add_element_to_panel(P,&Input_Export->Base);
    add_element_to_panel(P,&Input_Path_Field->Base);
    add_element_to_panel(P,&Load_Project->Base);
    add_element_to_panel(P,&Cancle_Loading_Project->Base);
    add_panel_to_registry(P,Reg);
    uninspect_inspector_panel(E->Inspector);
    Panel->Remove=1;
}
void scene_explorer_show_make_scene_impl(panel_button* Self){
    scene_explorer_element* E=(scene_explorer_element*)Self->Button_Data;
    panel* Panel=Self->Base.Parent;
    panel* P=create_popup_panel("Make Scene",7,10,110,6);
    scene_expo_make_scene* Make_Scene= (scene_expo_make_scene*)malloc(sizeof(scene_expo_make_scene));
    panel_text* Input_Export = create_panel_text("Save To:",1,2,100);
    panel_input_text* Input_Path_Field =create_panel_input_text(10,2,99);
    Make_Scene->Path=Input_Path_Field;
    Make_Scene->Selected=E->Selected_Right->Entity;
    panel_button* Save_Project = create_panel_button(24,4,"Save Project",18,Make_Scene,save_scene_explorer_make_scene);
    panel_button* Cancle_Save_Project = create_panel_button(64,4,"Cancle Saving",20,Make_Scene,cancel_scene_explorer_make_scene);
    panel_registry* Reg = Self->Base.Parent->Registry;
    add_element_to_panel(P,&Input_Export->Base);
    add_element_to_panel(P,&Input_Path_Field->Base);
    add_element_to_panel(P,&Save_Project->Base);
    add_element_to_panel(P,&Cancle_Save_Project->Base);
    add_panel_to_registry(P,Reg);
    uninspect_inspector_panel(E->Inspector);
    Panel->Remove=1;
}
void add_entity_entity_to_scene_explorer(entity* Entity,scene_explorer_element* se,int depth){
    if(!TypeDB_Get(Entity->Type_Name)) return;
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
    Entity_Node->depth=depth;
    se->Entities[se->Count++]=Entity_Node;
}
void scene_explorer_point_to_inspector(panel* Scene_Explorer,panel* Inspector){
    if(!Scene_Explorer||Scene_Explorer->Cap<=0) return;
    scene_explorer_element* se = (scene_explorer_element*)Scene_Explorer->Elements[0];
    se->Inspector=Inspector;
}
void add_child_entity_to_scene_explorer(entity* Entity,scene_explorer_element* se,int depth){
    for(size_t i=0;i<Entity->Count;i++){
        entity* e=Entity->Children[i];
        add_entity_entity_to_scene_explorer(e,se,depth);
        if(e->Count&&!e->Path)
            add_child_entity_to_scene_explorer(e,se,depth+1);

    }
}
void update_scene_explorer_element(panel_element* Self){
    scene_explorer_element* se = (scene_explorer_element*)Self;
    panel* Panel = Self->Parent;
    if(Panel==Panel->Registry->Focused&&Panel==Panel->Registry->Hovered){
        short MX = get_mouse_X()-Panel->X;
        short MY = get_mouse_Y()-Panel->Y;
        int offset=1;
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
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

        } if (is_key_just_released(RUNEFORGE_MOUSE_BUTTON_RIGHT)){
            entity_node* Node = NULL;
            if(MY-offset>=0&&MY-offset<(short)(se->Count)){
                Node=se->Entities[MY-offset];
            }
            if (Node&&Node->Entity!=se->Registry->Root&&MX>=3&&MX<Node->Text_Sprite.Width+3){
                panel_button *B1,*B2,*B3,*B4,*B5;
                int Count=0;
                se->Selected_Right=Node;
                B1=create_panel_button(1,Count++,"Rename",11,se,scene_explorer_show_rename_entity_impl);
                if(!se->Selected_Right->Entity->Path){
                    B2=create_panel_button(1,Count++,"Add_Child",14,se,scene_explorer_add_child_impl);
                    //B3=create_panel_button(1,Count++,"Add_Subscene",17,se,scene_explorer_add_subscene_impl);
                    B4=create_panel_button(1,Count++,"Make_Scene",15,se,scene_explorer_show_make_scene_impl);
                }
                else{
                    //add_another_button;
                }
                B5=create_panel_button(1,Count++,"Delete",14,se,scene_explorer_delete_button_impl);
                panel* P=create_context_panel("Entity Context",17,Count);
                add_element_to_panel(P,&B1->Base);
                add_element_to_panel(P,&B5->Base);
                if(!se->Selected_Right->Entity->Path){
                    add_element_to_panel(P,&B2->Base);
                    //add_element_to_panel(P,&B3->Base);
                    add_element_to_panel(P,&B4->Base);
                }
                add_panel_to_registry(P,Panel->Registry);
            }
            else{
                se->Selected_Right=NULL;
                panel* P=create_context_panel("Entity Context",17,1);
                panel_button* B1=create_panel_button(1,0,"Add_Entity",15,se,scene_explorer_add_child_impl);
                add_element_to_panel(P,&B1->Base);
                add_panel_to_registry(P,Panel->Registry);
            }
            
        }
        
    }
    entity_registry* Registry = se->Registry;
    if(se->Version!=Registry->Version){
        for(size_t i=0;i<se->Count;i++){
            destroy_sprite(&se->Entities[i]->Text_Sprite);
            free(se->Entities[i]);
            se->Entities[i]=NULL;
        }
        se->Count=0;
        entity* Root=Registry->Root;
        add_entity_entity_to_scene_explorer(Root,se,0);
        add_child_entity_to_scene_explorer(Root,se,1);
        se->Version=Registry->Version;
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
            char Name[128];
            if(Node->depth>0&&Node->depth+1<sizeof(Name)){
                memset(Name,'-',Node->depth);
                if(Node->Entity->Path)
                    snprintf(Name+Node->depth,sizeof(Name)-Node->depth,"><%s",Node->Entity->Name);
                else
                    snprintf(Name+Node->depth,sizeof(Name)-Node->depth,">%s",Node->Entity->Name);
            }
            else{
                snprintf(Name,sizeof(Name),"%s",Node->Entity->Name);
            }
            Node->Text_Sprite=create_text(Name,se->Base.Parent->Data.Width-5);
            Node->Is_Dirty=0;
        }   
        draw_game_overlay_sprite(Node->Text_Sprite,3,i+1,Self->Parent->Data.Z_Index+1);
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
    Element->Add_Entity_Cap=0;
    Element->Add_Entity_Button=NULL;
    Element->Count=0;
    Element->Is_Dirty=0;
    Element->Entities=NULL;
    Element->Selected=NULL;
    Element->Selected_Right=NULL;
    Element->Inspector=NULL;
    Element->Version=(size_t)-1;//size_t max
    init_panel_element_base(&Element->Base,0,0,update_scene_explorer_element,render_scene_explorer_element,destroy_scene_explorer_element);
    return Element;
}
void add_entity_registry_to_scene_explorer(entity_registry* Registry,panel* Scene_Explorer){
    if(!Scene_Explorer||Scene_Explorer->Count<=0) return;
    scene_explorer_element* se = (scene_explorer_element*)Scene_Explorer->Elements[0];
    se->Registry=Registry;
    return;
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