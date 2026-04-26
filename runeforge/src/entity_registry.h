#include <runewall.h>
typedef struct entity entity;
typedef struct entity_vtable{
    void (*onUpdate)(entity* self,double deltaTime);
    void (*onRender)(entity* self,renderer* Renderer);
    void (*onDestroy)(entity* self);
    //other functions
}entity_vtable;
struct entity{
    entity_vtable* Vtable;
    size_t ID;
};
typedef struct entity_registry entity_registry;
entity_registry* create_entity_registry(void);
void add_entity_to_registry(entity_registry *Registry,entity *Entity);
void update_entities(entity_registry* Self,double deltaTime);
void render_entities(entity_registry* Self,renderer* Renderer);
void free_entity(entity_registry *Registry,entity *Entity);
void destroy_entity_registry(entity_registry* Self);