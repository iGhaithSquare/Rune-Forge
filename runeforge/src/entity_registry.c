#include "entity_registry.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
struct entity_registry{
    entity **Entities;
    size_t Count;
    size_t Cap;
};
entity_registry* create_entity_registry(void){
    entity_registry *Registry = (entity_registry *)malloc(sizeof(entity_registry));
    GAVEN_ASSERT(Registry,"Failed to allocate memory to entities registry");
    Registry->Cap=0;
    Registry->Count=0;
    Registry->Entities=NULL;
    return Registry;
}
void add_entity_to_registry(entity_registry *Registry,entity *Entity){
    if(Registry->Count>=Registry->Cap){
        Registry->Cap=(Registry->Cap?Registry->Cap*2:128);
        entity** temp=(entity**)realloc(Registry->Entities,sizeof(entity*)*Registry->Cap);
        GAVEN_ASSERT(temp,"Failed to allocate memory to entities registry");
        Registry->Entities=temp;
    }
    if(Entity->Type->Create)
        Entity->Type->Create(Entity);
    Entity->ID=Registry->Count;
    Registry->Entities[Registry->Count++]=Entity;
}
void free_entity(entity_registry *Registry,entity *Entity){
    if(Entity->ID>=Registry->Count){
        GAVEN_WARN("Entity is not in registry");
        return;
    }
    Registry->Count--;
    Registry->Entities[Entity->ID]=Registry->Entities[Registry->Count];
    Registry->Entities[Entity->ID]->ID=Entity->ID;
    Registry->Entities[Registry->Count]=NULL;
    if(Entity->Type->Destroy)
        Entity->Type->Destroy(Entity);
    else
        free(Entity);
}
void destroy_entity_registry(entity_registry* Self){
    for(size_t i=0;i<Self->Count;i++){
        entity *Entity = Self->Entities[i];
        if(Entity->Type->Destroy)
            Entity->Type->Destroy(Entity);
        else
            free(Entity);
    }
    free(Self->Entities);
    free(Self);
}

void update_entities(entity_registry* Self,double deltaTime){
    size_t i;
    for(i=0;i<Self->Count;i++){
        entity* E = Self->Entities[i];
        if(E->Type->Update)
            E->Type->Update(E,deltaTime);
    }
}
void render_entities(entity_registry* Self){
    size_t i;
    for(i=0;i<Self->Count;i++){
        entity* E = Self->Entities[i];
        if(E->Type->Render)
            E->Type->Render(E);
    }
}

void serialize_entity(cJSON* root, entity* Entity){
    type_info *t =Entity->Type;
    cJSON_AddStringToObject(root,"Type",Entity->Type->Name);
    for(size_t i=0;i<t->Property_Count;i++){
        property_info* p =&t->Properties[i];
        void *field=(char*)Entity+p->Usage;
        switch(p->Type){
            case PROPERTY_TYPE_INT: cJSON_AddNumberToObject(root,p->Name,(double)*(int*)field); break;
            case PROPERTY_TYPE_FLOAT: cJSON_AddNumberToObject(root,p->Name,(double)*(float*)field);break;
            case PROPERTY_TYPE_STRING:  cJSON_AddStringToObject(root,p->Name,*(char**)field);break;
            case PROPERTY_TYPE_DOUBLE: cJSON_AddNumberToObject(root,p->Name,*(double *)field);break;
            case PROPERTY_TYPE_SIZET:   cJSON_AddNumberToObject(root,p->Name,(double)*(size_t*)field); break;
            default: GAVEN_ASSERT(0,"Unsupported property type for serialization");
        }
    }
}
void serialize_entity_registry(const char* path,entity_registry* Entity_Registry){
    FILE *f = fopen(path,"w");
    GAVEN_ASSERT(f,"Failed to open file %s",path);
    cJSON* root = cJSON_CreateObject();
    cJSON_AddStringToObject(root,"Scene","Untitled");
    cJSON* arr= cJSON_CreateArray();
    for (size_t i=0;i< Entity_Registry->Count;i++){
        cJSON* obj=cJSON_CreateObject();
        serialize_entity(obj,Entity_Registry->Entities[i]);
        cJSON_AddItemToArray(arr,obj);
    }
    cJSON_AddItemToObject(root,"Entities",arr); 
    char* json_string = cJSON_Print(root);
    fputs(json_string,f);
    fclose(f);
    free(json_string);
    cJSON_Delete(root);
}
void deserialize_entity(cJSON* root, entity* Entity){
    type_info *t =Entity->Type;
    for(size_t i=0;i<t->Property_Count;i++){
        property_info* p=&t->Properties[i];
        cJSON* item = cJSON_GetObjectItem(root,p->Name);
        if(!item) continue;
        void *field=(char*)Entity+p->Usage;
        switch(p->Type){
            case PROPERTY_TYPE_INT: *(int*)field=item->valueint; break;
            case PROPERTY_TYPE_FLOAT: *(float*)field=(float)item->valuedouble; break;
            case PROPERTY_TYPE_STRING:  *(char**)field=strdup(item->valuestring); break;
            case PROPERTY_TYPE_DOUBLE: *(double*)field=item->valuedouble; break;
            case PROPERTY_TYPE_SIZET:   *(size_t*)field=(size_t)item->valuedouble; break;
            default: GAVEN_ASSERT(0,"Unsupported property type for deserialization");
        }
    }
}
void deserialize_entity_registry(const char* path,entity_registry* Entity_Registry){
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
    free(buffer);
    cJSON* arr=cJSON_GetObjectItem(root,"Entities");
    int count=cJSON_GetArraySize(arr);
    for(size_t i=0;i<count;i++){
        cJSON* obj=cJSON_GetArrayItem(arr,i);
        const char* Type_Name=cJSON_GetObjectItem(obj,"Type")->valuestring;
        entity *e=create_entity(Type_Name);
        deserialize_entity(obj,e);
        add_entity_to_registry(Entity_Registry,e);
    }
}