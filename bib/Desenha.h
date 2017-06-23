#ifndef DESENHA_H
#define DESENHA_H

#include <math.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>

#include "Vetor3D.h"


class Desenha
{
   public:
      Desenha();
      ~Desenha();


      static void drawBox(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
      static void drawCube(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
      static void drawCylinder(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);


      static void drawWireBox(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
      static void drawSelectedBox(GLfloat, GLfloat, GLfloat, GLfloat, GLfloat, GLfloat);
      static void drawQuad(GLfloat, GLfloat, GLfloat, GLfloat);
      static void drawGrid(int, GLfloat, int, int);
      static void gluClosedCylinder(GLUquadric*, GLdouble, GLdouble, GLdouble, GLint, GLint);
      static void gluCapsule(GLUquadric*, GLdouble, GLdouble, GLdouble, GLint, GLint);
      static void drawArrow(Vetor3D from, Vetor3D to, GLfloat larg = 0.0);
      static void drawDoubleArrow(Vetor3D from, Vetor3D to, GLfloat larg = 0.0);
      static void drawEixos(GLfloat);

      static void setTransformODE(const dReal* pos, const dReal* R);



        private:
            GLdouble xt, yt, zt;
            GLdouble xr, yr, zr;
            GLdouble xs, ys, zs;

            bool selected;

};


#endif

