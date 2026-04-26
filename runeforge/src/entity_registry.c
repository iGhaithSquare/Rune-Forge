#include "entity_registry.h"
#include <stdlib.h>
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
    if(Entity->Vtable&&Entity->Vtable->onDestroy)
        Entity->Vtable->onDestroy(Entity);
    else
        free(Entity);
}
void destroy_entity_registry(entity_registry* Self){
    for(size_t i=0;i<Self->Count;i++){
        entity *Entity = Self->Entities[i];
        if(Entity->Vtable&&Entity->Vtable->onDestroy)
            Entity->Vtable->onDestroy(Entity);
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
        if(E->Vtable->onUpdate)
            E->Vtable->onUpdate(E,deltaTime);
    }
}
void render_entities(entity_registry* Self,renderer* Renderer){
    size_t i;
    for(i=0;i<Self->Count;i++){
        entity* E = Self->Entities[i];
        if(E->Vtable->onRender)
            E->Vtable->onRender(E,Renderer);
    }
}