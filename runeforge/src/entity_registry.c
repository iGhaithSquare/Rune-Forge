#include "entity_registry.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

entity_registry* create_entity_registry(void){
    entity_registry *Registry = (entity_registry *)malloc(sizeof(entity_registry));
    GAVEN_ASSERT(Registry,"Failed to allocate memory to entities registry");
    Registry->Root=create_entity(NULL,"Entity","Root",NULL);
    Registry->Version=0;
    return Registry;
}
void add_entity_to_registry(entity_registry *Registry,entity *Entity){
    add_entity_child(Registry->Root,Entity);
    Registry->Version++;
}
void free_entity(entity_registry *Registry,entity *Entity){
    free_child(Registry->Root,Entity);
    Registry->Version++;
}
void destroy_entity_registry(entity_registry* Self){
    free_child(NULL,Self->Root);
    free(Self);
    Self=NULL;
}
void unload_entity_registry(entity_registry* Self){
    free_child(NULL,Self->Root);
    Self->Root=create_entity(NULL,"Entity","Root",NULL);
    Self->Version++;
}
void update_entities(entity_registry* Self,double deltaTime){
    update_entity(Self->Root,deltaTime);
}
void entities_on_event(entity_registry* Self,event *Event){
    entity_on_event(Self->Root,Event);
}
void render_entities(entity_registry* Self){
    if(render_entity(Self->Root)){
        Self->Version++;
        serialize_entity_registry(Self->Path,Self);

    }
}
void serialize_entity(cJSON* root, entity* Entity){
    type_info *t =Entity->Type;
    cJSON_AddStringToObject(root,"Type",Entity->Type->Name);
    cJSON_AddStringToObject(root,"Name",Entity->Name);
    cJSON_AddStringToObject(root,"Path",Entity->Path?Entity->Path:"");
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
    
    cJSON* Children=cJSON_CreateArray();
    
    if(!Entity->Path)
        for(size_t i=0;i<Entity->Count;i++){
            cJSON* Child=cJSON_CreateObject();
            serialize_entity(Child,Entity->Children[i]);
            cJSON_AddItemToArray(Children,Child);
        }
    cJSON_AddItemToObject(root,"Children",Children);
}
void serialize_entity_registry(const char* path,entity_registry* Entity_Registry){
    FILE *f = fopen(path,"w");
    GAVEN_ASSERT(f,"Failed to open file %s",path);
    cJSON* root = cJSON_CreateObject();
    cJSON* obj=cJSON_CreateObject();
    serialize_entity(obj,Entity_Registry->Root);
    cJSON_AddItemToObject(root,"Root",obj); 
    char* json_string = cJSON_Print(root);
    fputs(json_string,f);
    fclose(f);
    free(json_string);
    cJSON_Delete(root);
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
    cJSON* obj=cJSON_GetObjectItem(root,"Root");
    Entity_Registry->Root=deserialize_entity(obj,NULL);
    cJSON_Delete(root);
}