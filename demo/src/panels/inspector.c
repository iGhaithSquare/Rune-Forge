#include "inspector.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct inspector_ui{
    short Value_Text_Offset;
    char Final_Text[256];
    sprite Text_Sprite;
    void (*Update)(struct inspector_ui* Self);
    uint8_t Data; //bit 0 for dirty bit 1 for is pressed
}inspector_ui;

typedef struct inspector_element{
    panel_element Base;
    entity* Selected;
    inspector_ui** Inspector_uis;
    size_t Count;
    size_t Cap;
    uint8_t Is_Dirty;
}inspector_element;
void update_inspector_element(panel_element* Self){
    inspector_element* I=(inspector_element*)Self;
    if(I->Base.Parent->Is_Dirty) I->Is_Dirty=1;
    return;
}
void render_inspector_element(panel_element* Self){
    inspector_element* I = (inspector_element*)Self;
    
    for(size_t i=0;i<I->Count;i++){
        inspector_ui* UI =I->Inspector_uis[i];
        if(UI->Data&1||I->Is_Dirty){
            destroy_sprite(&UI->Text_Sprite);
            UI->Text_Sprite=create_text(UI->Final_Text,I->Base.Parent->Data.Width-5);
            if(!I->Is_Dirty) UI->Data&=~1;
        }
        draw_game_overlay_sprite(UI->Text_Sprite,2,2+i,1);
    }
    I->Is_Dirty=0;
    return;
}
void destroy_inspector_element(panel_element* Self){
    inspector_element* I = (inspector_element*)Self;
    for(size_t i =0;i<I->Count;i++){
        inspector_ui* IU = I->Inspector_uis[i];
        destroy_sprite(&IU->Text_Sprite);
        free(IU);
    }
    free(I->Inspector_uis);
    free(I);
    return;
}
inspector_element* create_inspector_element(void){
    inspector_element* Element =(inspector_element*)malloc(sizeof(inspector_element));
    GAVEN_ASSERT(Element,"Couldnt allocat enough memory to Inspector");
    Element->Cap=0;
    Element->Count=0;
    Element->Inspector_uis=NULL;
    Element->Selected=NULL;
    Element->Is_Dirty=0;
    init_panel_element_base(&Element->Base,0,0,update_inspector_element,render_inspector_element,destroy_inspector_element);
    return Element;
}
void add_inspector_ui(inspector_element* Element,char* Type_Text,char* Value_Text,void (*Update)(inspector_ui* Self)){
    inspector_ui* IU=(inspector_ui*)malloc(sizeof(inspector_ui));
    GAVEN_ASSERT(IU,"Couldnt allocate enough memory to Inspector");
    if(Element->Count>=Element->Cap){
        Element->Cap=Element->Cap?Element->Cap*2:8;
        inspector_ui** temp = (inspector_ui**)realloc(Element->Inspector_uis,sizeof(inspector_ui*)*Element->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate enough memory to Inspector");
        Element->Inspector_uis=temp;
    }
    IU->Value_Text_Offset=(short)(strlen(Value_Text)+2);
    snprintf(IU->Final_Text,sizeof(IU->Final_Text),"%s: %s",Type_Text,Value_Text);
    IU->Update=Update;
    IU->Data=1;
    IU->Text_Sprite.Data=NULL;
    Element->Inspector_uis[Element->Count++]=IU;
}
void inspect_entity(entity* Entity,panel* Inspector_Panel){
    if(!Inspector_Panel||Inspector_Panel->Count<=0) return;
    if(!Entity) return;
    inspector_element* I=(inspector_element*)Inspector_Panel->Elements[0];
    if(I->Selected){
        for(size_t i=0;i<I->Count;i++){
            inspector_ui* UI =I->Inspector_uis[i];
            destroy_sprite(&UI->Text_Sprite);
            free(UI);
        }
        I->Count=0;
    }
    I->Selected=Entity;
    type_info *Info=Entity->Type;
    for(size_t i=0;i<Info->Property_Count;i++){
        property_info* P =&Info->Properties[i];
        void *field=(char*)Entity+P->Usage;
        char value_buffer[128];
        char type_buffer[128];

        switch (P->Type){
        case PROPERTY_TYPE_STRING:
            snprintf(value_buffer,128,"%s",*(char**)field?*(char**)field:"");
            break;
        case PROPERTY_TYPE_INT:
            snprintf(value_buffer,128,"%d",*(int*)field);
            break;
        case PROPERTY_TYPE_FLOAT:
            snprintf(value_buffer,128,"%f",*(float*)field);
            break;
        
        case PROPERTY_TYPE_DOUBLE:
            snprintf(value_buffer,128,"%f",*(double*)field);
            break;
        case PROPERTY_TYPE_SIZET:
            snprintf(value_buffer,128,"%zu",*(size_t*)field);
            break;
        default:
            GAVEN_WARN("Type doesnt exist %d",P->Type);
            continue;
            break;
        }
        snprintf(type_buffer,128,P->Name);
        add_inspector_ui(I,type_buffer,value_buffer,NULL);
    }
}

panel* create_inspector(void){
    panel_data Inspect={
        .Name="Inspector",
        .Background_Char='-',
        .Height=5,
        .Width=20,
        .Min_Height=4,
        .Min_Width=10,
        .Anchor=1|2|4|8, //anchored all sides
        .Is_Resizable=1,
        .Is_Viewport=0
    };
    panel *Inspector = create_panel(Inspect);
    inspector_element *E=create_inspector_element();
    add_element_to_panel(Inspector,&E->Base);
    return Inspector;
}