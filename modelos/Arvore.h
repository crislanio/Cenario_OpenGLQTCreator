#ifndef ARVORE_H
#define ARVORE_H

#include "Objeto.h"

class Arvore: public Objeto{

public:
    Arvore();
    ~Arvore();
    virtual void Desenhar_Objeto();
};

#endif // ARVORE_H
