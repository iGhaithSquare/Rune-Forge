#include "inspector.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct inspector_ui{
    short Value_Text_Offset;
    char Final_Text[256];
    char Real_Value_Buffer[256];
    sprite Text_Sprite;
    property_types Type;
    void* Field;
    uint8_t Is_Dirty; //bit 0 for dirty bit 1 for is pressed
}inspector_ui;

typedef struct inspector_element{
    panel_element Base;
    uint8_t Has_Selection;
    void* Selected;
    uint8_t Inspected_Type;//0 for entitities,1 for asset
    inspector_ui** Inspector_uis;
    inspector_ui* Editing;
    size_t Count;
    size_t Cap;
    uint8_t Is_Dirty;
}inspector_element;
void update_inspector_element(panel_element* Self){
    inspector_element* I=(inspector_element*)Self;
    panel* Panel = I->Base.Parent;
    if(I->Selected){
        //todo add inspected types and switch through them
        entity* E=(entity*)I->Selected;
        if(!TypeDB_Get(E->Type_Name))
            return;
    }
    if(Panel==Panel->Registry->Focused){
        if(is_key_just_pressed(RUNEFORGE_MOUSE_BUTTON_LEFT)){
            short MX = get_mouse_X()-Panel->X;
            short MY = get_mouse_Y()-Panel->Y;
            int offset=2;
            inspector_ui* IU = NULL;
            if(MY>=offset&&MY<(short)(I->Count)+offset){
                IU=I->Inspector_uis[MY-offset];
            }
            if (IU&&MX>=IU->Value_Text_Offset+2&&MX<IU->Text_Sprite.Width+IU->Value_Text_Offset&&I->Editing!=IU){                
                I->Editing = IU;
                IU->Final_Text[IU->Value_Text_Offset]='\0';
                IU->Is_Dirty=1;
            }
            else if(I->Editing){
                inspector_ui* Selected=I->Editing;
                snprintf(Selected->Final_Text,256,"%s",Selected->Real_Value_Buffer);
                Selected->Is_Dirty=1;
                I->Editing=NULL;
            }

        }
        
    }
    else if(I->Editing){
        inspector_ui* Selected=I->Editing;
        snprintf(Selected->Final_Text,256,"%s",Selected->Real_Value_Buffer);
        Selected->Is_Dirty=1;
        I->Editing=NULL;
    }
    inspector_ui* Selected=I->Editing;
    if(Panel->Is_Dirty) I->Is_Dirty=1;
    if(Selected){
        if(!Selected->Field) return;
        size_t len=strlen(Selected->Final_Text);
        if(is_key_just_pressed(RUNEFORGE_KEY_ENTER)){
            uint8_t success=0;
            char *end;
            switch(Selected->Type){
                case PROPERTY_TYPE_DOUBLE:{
                    double value =strtod(Selected->Final_Text+Selected->Value_Text_Offset,&end);
                    if(*end=='\0'){
                        *(double*)Selected->Field=value;
                        success=1;
                    }
                    break;
                }
                case PROPERTY_TYPE_FLOAT:{
                    float value =strtof(Selected->Final_Text+Selected->Value_Text_Offset,&end);
                    if(*end=='\0'){
                        *(float*)Selected->Field=value;
                        success=1;
                    }
                    break;
                }
                case PROPERTY_TYPE_INT:{
                    long value =strtol(Selected->Final_Text+Selected->Value_Text_Offset,&end,10);
                    if(*end=='\0'){
                        *(int*)Selected->Field=(int)value;
                        success=1;
                    }
                    break;
                }
                case PROPERTY_TYPE_SIZET:{
                    long long value =strtoll(Selected->Final_Text+Selected->Value_Text_Offset,&end,10);
                    if(*end=='\0'){
                        *(size_t*)Selected->Field=(size_t)value;
                        success=1;
                    }
                    break;
                }
                case PROPERTY_TYPE_STRING:
                    char** str = (char**)Selected->Field;
                    free(*str);  
                    *str=strdup(Selected->Final_Text+Selected->Value_Text_Offset);
                    success=1;
                    break;
            }
            if(success)
                snprintf(Selected->Real_Value_Buffer,256,"%s",Selected->Final_Text);
            else{
                snprintf(Selected->Final_Text,256,"%s",Selected->Real_Value_Buffer);
                Selected->Is_Dirty=1;
            }
            I->Editing=NULL;
            return;
        }
        if(is_key_just_pressed(RUNEFORGE_KEY_ESCAPE)){
                snprintf(Selected->Final_Text,256,"%s",Selected->Real_Value_Buffer);
            Selected->Is_Dirty=1;
            I->Editing=NULL;
            return;
        }
        if(is_key_just_pressed(RUNEFORGE_KEY_BACKSPACE)){
            if(len>0){
                Selected->Final_Text[len-1]='\0';
                Selected->Is_Dirty=1;
            }
            return;
        }
        if(is_key_just_pressed(RUNEFORGE_KEY_DELETE))
            return;
        if(is_key_just_pressed(RUNEFORGE_KEY_TAB)){
            if(len<255){
                Selected->Final_Text[len]=' ';
                Selected->Final_Text[len+1]='\0';
                Selected->Is_Dirty=1;
            }
            return;
        }
        size_t add_len=0;
        const char* c = get_text_input(&add_len);
        if(add_len&&add_len+len<255){
            memcpy(Selected->Final_Text+len,c,add_len);
            Selected->Final_Text[len+add_len]='\0';
            Selected->Is_Dirty=1;
        }
    }
    return;
}
void render_inspector_element(panel_element* Self){
    inspector_element* I = (inspector_element*)Self;
    
    for(size_t i=0;i<I->Count;i++){
        inspector_ui* UI =I->Inspector_uis[i];
        if(UI->Is_Dirty||I->Is_Dirty){
            destroy_sprite(&UI->Text_Sprite);
            UI->Text_Sprite=create_text(UI->Final_Text,I->Base.Parent->Data.Width-5);
            UI->Is_Dirty=0;
        }
        draw_game_overlay_sprite(UI->Text_Sprite,2,2+i,Self->Parent->Data.Z_Index+1);
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
    Element->Has_Selection=0;
    Element->Editing=NULL;
    Element->Is_Dirty=0;
    init_panel_element_base(&Element->Base,0,0,update_inspector_element,render_inspector_element,destroy_inspector_element);
    return Element;
}
void add_inspector_ui(inspector_element* Element,char* Type_Text,char* Value_Text,void* Field,property_types Type){
    inspector_ui* IU=(inspector_ui*)malloc(sizeof(inspector_ui));
    GAVEN_ASSERT(IU,"Couldnt allocate enough memory to Inspector");
    if(Element->Count>=Element->Cap){
        Element->Cap=Element->Cap?Element->Cap*2:8;
        inspector_ui** temp = (inspector_ui**)realloc(Element->Inspector_uis,sizeof(inspector_ui*)*Element->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate enough memory to Inspector");
        Element->Inspector_uis=temp;
    }
    IU->Value_Text_Offset=(short)(strlen(Type_Text)+2);
    snprintf(IU->Final_Text,sizeof(IU->Final_Text),"%s: %s",Type_Text,Value_Text);
    memcpy(IU->Real_Value_Buffer,IU->Final_Text,sizeof(IU->Real_Value_Buffer));
    IU->Is_Dirty=1;
    IU->Text_Sprite.Data=NULL;
    IU->Field=Field;
    IU->Type=Type;
    Element->Inspector_uis[Element->Count++]=IU;
}
void uninspect_inspector_panel(panel* Inspector_Panel){
    inspector_element* I=(inspector_element*)Inspector_Panel->Elements[0];
    if(I->Has_Selection){
        for(size_t i=0;i<I->Count;i++){
            inspector_ui* UI =I->Inspector_uis[i];
            destroy_sprite(&UI->Text_Sprite);
            free(UI);
        }
        I->Count=0;
    }
    I->Has_Selection=0;
    I->Selected=NULL;
}
void inspect_asset(size_t Asset_ID,panel* Inspector_Panel,asset_type Type){
    if(!Inspector_Panel||Inspector_Panel->Count<=0) return;
    inspector_element* I=(inspector_element*)Inspector_Panel->Elements[0];
    if(I->Has_Selection){
        for(size_t i=0;i<I->Count;i++){
            inspector_ui* UI =I->Inspector_uis[i];
            destroy_sprite(&UI->Text_Sprite);
            free(UI);
            I->Inspector_uis[i]=NULL;
        }
        I->Count=0;
    }
    I->Has_Selection=1;
    char type_buffer[128];
    snprintf(type_buffer,128,"Asset ID: %zu",Asset_ID);
    add_inspector_ui(I,type_buffer,"",NULL,PROPERTY_TYPE_INT);

}
void inspect_entity(entity* Entity,panel* Inspector_Panel){
    if(!Inspector_Panel||Inspector_Panel->Count<=0) return;
    if(!Entity) return;
    inspector_element* I=(inspector_element*)Inspector_Panel->Elements[0];
    if(I->Has_Selection){
        for(size_t i=0;i<I->Count;i++){
            inspector_ui* UI =I->Inspector_uis[i];
            destroy_sprite(&UI->Text_Sprite);
            free(UI);
        }
        I->Count=0;
    }
    I->Selected=Entity;
    I->Has_Selection=1;
    char buffer[128];
    snprintf(buffer,sizeof(buffer),"ID: %zu",Entity->ID);
    add_inspector_ui(I,buffer,"",NULL,-1);
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
        snprintf(type_buffer,128,"%s",P->Name);
        add_inspector_ui(I,type_buffer,value_buffer,field,P->Type);
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
        .Z_Index=0
    };
    panel *Inspector = create_panel(Inspect);
    inspector_element *E=create_inspector_element();
    add_element_to_panel(Inspector,&E->Base);
    return Inspector;
}