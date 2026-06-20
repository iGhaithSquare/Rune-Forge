#ifndef ENTITY2D_TYPE_H
#define ENTITY2D_TYPE_H
#include "type_entity.h"
typedef struct type_entity2d{
    type_entity Base;
    double X,Y;
    int Z;
}type_entity2d;
extern type_info Type_Entity2D;
#endif