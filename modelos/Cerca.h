#ifndef CERCA_H
#define CERCA_H
#include "Objeto.h"


class Cerca: public Objeto
{
public:
    Cerca();
    ~Cerca();
    virtual void Desenhar_Objeto();
};

#endif // CERCA_H
