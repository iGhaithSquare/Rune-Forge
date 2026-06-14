#include "entity.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
typedef struct type_registry{
    type_info** Types;
    size_t Count;
    size_t Cap;
}type_registry;
static type_registry* g_types= NULL;
type_info* TypeDB_Get(const char* name){
    if(!g_types) return NULL;   
    for (size_t i=0;i<g_types->Count;i++){
        type_info *n=g_types->Types[i];
        if(strcmp(n->Name,name)==0)
            return n; 
    }
    return NULL;
}
void TypeDB_Clear(void){
    if(!g_types||!g_types->Types) return;
    for(size_t i=0;i<g_types->Count;i++)
        g_types->Types[i]=NULL;
    g_types->Count=0;

}
void TypeDB_Register(type_info* Type){
    if(!Type||!Type->Name)
        return;
    if(g_types==NULL){
        g_types=(type_registry*)malloc(sizeof(type_registry));
        GAVEN_ASSERT(g_types,"Couldnt allocate memory to type registry");
        g_types->Types=NULL;
        g_types->Cap=0;
        g_types->Count=0;
    }
    if(g_types->Count>=g_types->Cap){
        g_types->Cap=(g_types->Cap)?g_types->Cap*2:16;
        type_info** temp =(type_info**)realloc(g_types->Types,sizeof(type_info*)*g_types->Cap);
        GAVEN_ASSERT(temp,"Couldnt allocate memory to type");
        g_types->Types=temp;
    }
    g_types->Types[g_types->Count++]=Type;
}
void Destroy_TypeDB(void){
    if(!g_types) return;
    free(g_types->Types);
    free(g_types);
    g_types=NULL;
}

void deserialize_into_entity(const char* path,entity* Entity){
    FILE *f = fopen(path,"r");
    GAVEN_ASSERT(f,"Failed to open file %s",path);
    size_t cap=0;
    size_t size=0;
    char* buffer=NULL;
    size_t n=0;
    do{
        size+=n;
        if(size>=cap){
            cap=(cap?cap*2:4096);
            char* temp = (char*)realloc(buffer,cap);
            GAVEN_ASSERT(temp,"Failed to allocate memory to read file %s",path);
            buffer=temp;
        }
    }while((n=fread(buffer+size,1,cap-size,f))>0);
    fclose(f);
    if(size>=cap){
        cap+=1;
        char* temp = (char*)realloc(buffer,cap);
        GAVEN_ASSERT(temp,"Failed to allocate memory to read file %s",path);
        buffer=temp;
    }
    buffer[size]='\0';
    cJSON *root=cJSON_Parse(buffer);
    GAVEN_ASSERT(root,"Couldnt deserialize for some reason");
    free(buffer);
    cJSON* obj=cJSON_GetObjectItem(root,"Root");
    deserialize_entity(obj,Entity);
    cJSON_Delete(root);
}
//todo guard against infinite recursion
entity* create_entity(entity* Parent,const char* Type_Name,const char* Entity_Name,char* Path){
    type_info *type=TypeDB_Get(Type_Name);
    if(!type) return NULL;
    entity *e =(entity*)calloc(1,type->Size);
    e->Children=NULL;
    e->Count=0;
    e->Cap=0;
    GAVEN_ASSERT(e,"Couldnt create entity of type %s",Type_Name);
    e->Type = type;
    e->Type_Name= strdup(Type_Name);
    e->Name = Entity_Name?strdup(Entity_Name):strdup(Type_Name);
    e->Path=Path;
    if (Path)
        deserialize_into_entity(Path,e);
    add_entity_child(Parent,e);
    return e;
}

void add_entity_child(entity* Parent,entity* Child){
    if(!Child||!Parent||Child->Parent==Parent) return;
    if(Parent->Count>=Parent->Cap){
        Parent->Cap=(Parent->Cap?Parent->Cap*2:2);
        entity** temp=(entity**)realloc(Parent->Children,sizeof(entity*)*Parent->Cap);
        GAVEN_ASSERT(temp,"Failed to allocate memory to entities registry");
        Parent->Children=temp;
    }
    if(Child->Type->Create)
        Child->Type->Create(Child);
    Child->ID=Parent->Count;
    Child->Parent=Parent;
    Parent->Children[Parent->Count++]=Child;
}

void free_child(entity *Parent,entity *Entity){
    if(!Entity)
        return;
    if(Parent){
        if(Entity->ID>=Parent->Count){
            GAVEN_WARN("Entity is not in registry");
            return;
        }
        Parent->Count--;
        Parent->Children[Entity->ID]=Parent->Children[Parent->Count];
        Parent->Children[Entity->ID]->ID=Entity->ID;
        Parent->Children[Parent->Count]=NULL;
    }
    if(Entity->Children){
        while(Entity->Children[0]){
            free_child(Entity,Entity->Children[0]);
        }
        free(Entity->Children);
    }
    if(Entity->Path)
        free(Entity->Path);
    free(Entity->Name);
    
    if(TypeDB_Get(Entity->Type_Name)&&Entity->Type->Destroy){
        free(Entity->Type_Name);
        Entity->Type->Destroy(Entity);
    }
    else{
        free(Entity->Type_Name);
        free(Entity);
    }
    Entity=NULL;
}

void update_entity(entity * Self,double deltaTime){
    size_t i;
    for(i=0;i<Self->Count;i++){
        entity* E = Self->Children[i];
        if(!TypeDB_Get(E->Type_Name)){
            continue;
        }
        type_info* Type=E->Type;
        if(!Type) continue;
        if(Type->Update)
            Type->Update(E,deltaTime);
        char* TypeName =Type->Parent;
        while(TypeName){
            type_info* Parent=TypeDB_Get(TypeName);
            if(Parent==NULL)
                break;
            if(Parent->Update)
                Parent->Update(E,deltaTime);
            TypeName=Parent->Parent;
        }
        update_entity(E,deltaTime);
    }
}

void entity_on_event(entity* Self,event *Event){
    size_t i;
    for(i=0;i<Self->Count;i++){
        entity* E = Self->Children[i];
        if(!TypeDB_Get(E->Type_Name)){
            continue;
        }
        type_info* Type=E->Type;
        if(!Type) continue;
        if(Type->OnEvent)
            Type->OnEvent(E,Event);
        char* TypeName =Type->Parent;
        while(TypeName){
            type_info* Parent=TypeDB_Get(TypeName);
            if(Parent==NULL)
                break;
            if(Parent->OnEvent)
                Parent->OnEvent(E,Event);
            TypeName=Parent->Parent;
        }
        entity_on_event(E,Event);
    }
}

uint8_t render_entity(entity* Self){
    size_t i;
    uint8_t Check=0;
    for(i=0;i<Self->Count;i++){
        entity* E = Self->Children[i];
        if(!TypeDB_Get(E->Type_Name)){
            free_child(Self,E);
            Check=1;
            E = Self->Children[i];
            if(!E) break;
        }
        type_info* Type=E->Type;
        if(!Type) continue;
        if(Type->Render)
            Type->Render(E);
        
        char* TypeName =Type->Parent;
        while(TypeName){
            type_info* Parent=TypeDB_Get(TypeName);
            if(Parent==NULL)
                break;
            if(Parent->Render)
                Parent->Render(E);
            TypeName=Parent->Parent;
        }
        Check = render_entity(E);
    }
    return Check;
}
type_info** Get_Entity_Types(size_t* Count){
    if(!g_types) return NULL;
    *Count=g_types->Count;
    return g_types->Types;
}
entity* get_child(entity* Self,size_t id){
    if(id>=Self->Count) return NULL;
    return Self->Children[id];
}
entity* deserialize_entity(cJSON* root, entity* Parent){
    cJSON* type_item =cJSON_GetObjectItem(root,"Type");
    cJSON* name_item =cJSON_GetObjectItem(root,"Name");
    cJSON* path_item =cJSON_GetObjectItem(root,"Path");
    if(!type_item||!name_item||!path_item) return NULL;
    char* Path =path_item->valuestring[0]?strdup(path_item->valuestring):NULL;
    entity* E=create_entity(Parent,type_item->valuestring,name_item->valuestring,Path);
    type_info *t =E->Type;
    for(size_t i=0;i<t->Property_Count;i++){
        property_info* p=&t->Properties[i];
        cJSON* item = cJSON_GetObjectItem(root,p->Name);
        if(!item) continue;
        void *field=(char*)E+p->Usage;
        switch(p->Type){
            case PROPERTY_TYPE_INT: *(int*)field=item->valueint; break;
            case PROPERTY_TYPE_FLOAT: *(float*)field=(float)item->valuedouble; break;
            case PROPERTY_TYPE_STRING:  char** str=(char**)field;
                *str=malloc(256);
                GAVEN_ASSERT(*str,"Couldnt allocate memory to property type %s",p->Name);
                strncpy(*str,item->valuestring,256); break;
            case PROPERTY_TYPE_DOUBLE: *(double*)field=item->valuedouble; break;
            case PROPERTY_TYPE_SIZET:   *(size_t*)field=(size_t)item->valuedouble; break;
            default: GAVEN_ASSERT(0,"Unsupported property type for deserialization");
        }
    }
    cJSON* children =cJSON_GetObjectItem(root,"Children");
    if (children&&cJSON_IsArray(children)){
        int child_count=cJSON_GetArraySize(children);
        for (int i =0;i<child_count;i++){
            cJSON* Child=cJSON_GetArrayItem(children,i);
            deserialize_entity(Child,E);
        }
    }
    return E;
}