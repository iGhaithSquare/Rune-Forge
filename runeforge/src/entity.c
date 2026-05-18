#include "entity.h"
#include <stdlib.h>
#include <string.h>

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
entity* create_entity(const char* Type_Name,const char* Entity_Name){
    type_info *type=TypeDB_Get(Type_Name);
    entity *e = (entity*)calloc(1,type->Size);
    GAVEN_ASSERT(e,"Couldnt create entity of type %s",Type_Name);
    e->Type = type;
    e->Name = Entity_Name?strdup(Entity_Name):strdup(Type_Name);
    return e;
}
type_info** Get_Entity_Types(size_t* Count){
    if(!g_types) return NULL;
    *Count=g_types->Count;
    return g_types->Types;
}