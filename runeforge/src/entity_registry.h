#ifndef ENTITY_REGISTRY_H
#define ENTITY_REGISTRY_H
#include <runewall.h>
#include "entity.h"
typedef struct entity_registry entity_registry;
struct entity_registry{
    entity* Root;
    char* Path;
    size_t Version;
};
void entities_on_event(entity_registry* Self,event* Event);
RUNEFORGE_API entity_registry* create_entity_registry(void);
RUNEFORGE_API void add_entity_to_registry(entity_registry *Registry,entity *Entity);
RUNEFORGE_API void update_entities(entity_registry* Self,double deltaTime);
RUNEFORGE_API void render_entities(entity_registry* Self);
RUNEFORGE_API void free_entity(entity_registry *Registry,entity *Entity);
RUNEFORGE_API void destroy_entity_registry(entity_registry* Self);
RUNEFORGE_API void unload_entity_registry(entity_registry* Self);
RUNEFORGE_API void serialize_entity_registry(const char* path,entity_registry* Entity_Registry);
RUNEFORGE_API void deserialize_entity_registry(const char* path,entity_registry* Entity_Registry);

#endif