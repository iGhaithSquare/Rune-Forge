#ifndef ENTITY2D_TYPE_H
#define ENTITY2D_TYPE_H
#include "type_entity.h"
typedef struct type_entity2d{
    type_entity Base;
    uint8_t Is_Parent_2d;
    entity* Parent;
    double rX,rY;
    int rZ;
    int Z;
    double X,Y;
}type_entity2d;
extern type_info Type_Entity2D;
#endif