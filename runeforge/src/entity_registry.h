#include <runewall.h>
#include "entity.h"
typedef struct entity_registry entity_registry;
entity_registry* create_entity_registry(void);
void add_entity_to_registry(entity_registry *Registry,entity *Entity);
void update_entities(entity_registry* Self,double deltaTime);
void render_entities(entity_registry* Self,renderer* Renderer);
void free_entity(entity_registry *Registry,entity *Entity);
void destroy_entity_registry(entity_registry* Self);

void serialize_entity_registry(const char* path,entity_registry* Entity_Registry);
void deserialize_entity_registry(const char* path,entity_registry* Entity_Registry);