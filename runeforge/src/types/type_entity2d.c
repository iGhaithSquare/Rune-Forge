#include "type_entity2d.h"
static property_info Entity2D_Props[] = {
    {"X",PROPERTY_TYPE_DOUBLE,offsetof(type_entity2d,X),NULL,NULL,NULL},
    {"Y",PROPERTY_TYPE_DOUBLE,offsetof(type_entity2d,Y),NULL,NULL,NULL},
    {"Z",PROPERTY_TYPE_INT,offsetof(type_entity2d,Z),NULL,NULL,NULL},
};
type_info Type_Entity2D = {
    .Name= "Entity2D",
    .Parent= "Entity",
    .Size=sizeof(type_entity2d),
    .Create= NULL,
    .Destroy=NULL,
    .Update= NULL,
    .Render=NULL,
    .OnEvent=NULL,
    .Properties=Entity2D_Props,
    .Property_Count=3,
    .Flags=0
};