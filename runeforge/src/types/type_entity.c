#include "type_entity.h"
static property_info Entity_Props[] = {
    
};
type_info Type_Entity = {
    .Name= "Entity",
    .Parent= NULL,
    .Size=sizeof(type_entity),
    .Create= NULL,
    .Destroy=NULL,
    .Update= NULL,
    .Render=NULL,
    .OnEvent=NULL,
    .Properties=Entity_Props,
    .Property_Count=0,
    .Flags=0
};