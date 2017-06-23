#include "PontoDeDescanso.h"
#include "GL/glut.h"

PontoDeDescanso::PontoDeDescanso(){
    this->tipo = 5;
}
PontoDeDescanso::~PontoDeDescanso(){}

void PontoDeDescanso::Desenhar_Objeto(){



    float qr,qv,qb;
    float qr1,qv1,qb1;
    // Alternar a cor
    if(isSelecionado()){
        qr = 0, qv = 0, qb = 0;
        qr1 = 0,qv1 = 0,qb1 = 0;
    }else{
        qr = 0.87, qv = 0.72, qb = 0.53; // colunas
        qr1 = 0.43,qv1 = 0.21, qb1 = 0.1; // teto
    }

    glPushMatrix();

    Objeto::Desenhar_Objeto();

    //cor das colunas
    this->setCor(qr,qv,qb);
    glColor3f(this->getQtd_R(), this->getQtd_G(), this->getQtd_B());
    // Linhas laterais
    Desenha::drawBox(0,2.4,0,0.2,2.5,-3.5);
    Desenha::drawBox(3.8,2.4,0,4,2.5,-3.5);
    // Colunas centrais
    Desenha::drawBox(0,0,-1.6,0.2,3.0,-1.9);
    Desenha::drawBox(3.8,0,-1.6,4,3.0,-1.9);

    // cor do teto
    this->setCor(qr1,qv1,qb1);
    glColor3f(this->getQtd_R(), this->getQtd_G(), this->getQtd_B());
    // Parte de cima do teto
    glBegin(GL_POLYGON);
        glVertex3d( 0, 2.5, 0.23);
        glVertex3d( 0, 3.0, -1.8);
        glVertex3d( 4, 3.0, -1.8);
        glVertex3d( 4, 2.5, 0.23);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex3d( 0, 2.5, -3.72);
        glVertex3d( 0, 3.0, -1.8);
        glVertex3d( 4, 3.0, -1.8);
        glVertex3d( 4, 2.5, -3.72);
    glEnd();

    // Desenha os eixos na posicao atual do objeto
    if(isDesenhaEixos()){
       Desenha::drawEixos(0.5);
    }

    glPopMatrix();

}

