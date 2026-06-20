#include "type_entity2d.h"

void type_entity2d_poll(entity* self){    
    type_entity2d* E=(type_entity2d*)self;
    entity* parent_entity=self->Parent;
    if(!parent_entity){
        E->Parent=NULL;
        E->Is_Parent_2d=0;
    }
    else if(E->Parent!=parent_entity){
        type_info* Info=parent_entity->Type;
        E->Is_Parent_2d=0;
        E->Parent=parent_entity;
        while(Info){
            if(Info==&Type_Entity2D){
                E->Is_Parent_2d=1;
                break;
            }
            if(!Info->Parent)
                break;
            Info=Info->Parent;
        }
    }
    if(E->Is_Parent_2d){
        type_entity2d* P=(type_entity2d*)parent_entity;
        E->X=E->rX+P->X;
        E->Y=E->rY+P->Y;
        E->Z=E->rZ+P->Z;
    }
    else{
        E->X=E->rX;
        E->Y=E->rY;
        E->Z=E->rZ;
    }
}
static property_info Entity2D_Props[] = {
    {"X",PROPERTY_TYPE_DOUBLE,offsetof(type_entity2d,rX),NULL,NULL,NULL},
    {"Y",PROPERTY_TYPE_DOUBLE,offsetof(type_entity2d,rY),NULL,NULL,NULL},
    {"Z",PROPERTY_TYPE_INT,offsetof(type_entity2d,rZ),NULL,NULL,NULL},
};
type_info Type_Entity2D = {
    .Name= "Entity2D",
    .Parent= &Type_Entity,
    .Size=sizeof(type_entity2d),
    .Create= NULL,
    .Destroy=NULL,
    .Poll=type_entity2d_poll,
    .Update= NULL,
    .Render=NULL,
    .OnEvent=NULL,
    .Properties=Entity2D_Props,
    .Property_Count=3,
    .Flags=0
};