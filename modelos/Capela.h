#ifndef CAPELA_H
#define CAPELA_H
#include "Objeto.h"

class Capela: public Objeto{

public:
    Capela();
    ~Capela();

    virtual void Desenhar_Objeto();
};

#endif // CAPELA_H
