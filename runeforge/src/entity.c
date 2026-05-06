#include "entity.h"
#include <stdlib.h>
#include <string.h>

typedef struct type_node{
    type_info* Type;
    struct type_node* Next;
}type_node;
static type_node* g_types= NULL;
type_info* TypeDB_Get(const char* name){
    for (type_node* n = g_types;n;n->Next){
        if(strcmp(n->Type->Name,name)==0)
            return n->Type; 
    }
    return NULL;
}
void TypeDB_Register(type_info* Type){
    if(!Type||!Type->Name)
        return;
    type_node *n = (type_node*)malloc(sizeof(type_node));
    GAVEN_ASSERT(n,"Coudnt allocate memory to type");
    n->Type=Type;
    n->Next=g_types;
    g_types=n;
}
void Destroy_TypeDB(void){
    while(g_types){
        type_node* Next=g_types;
        free(g_types);
        g_types=Next;
    }
}
entity* create_entity(const char* Type_Name,const char* Entity_Name){
    type_info *type=TypeDB_Get(Type_Name);
    entity *e = (entity*)malloc(type->Size);
    GAVEN_ASSERT(e,"Couldnt create entity of type %s",Type_Name);
    e->Type = type;
    e->Name = Entity_Name?strdup(Entity_Name):strdup(Type_Name);
}