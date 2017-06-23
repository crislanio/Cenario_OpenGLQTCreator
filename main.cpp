// 1° trazer para a origem,- escala depois, rotação, translação

#include <GL/glut.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include <string>
#include <cstring>
#include <list>

#include "modelos/Casa.h"
#include "modelos/Placa.h"
#include "modelos/Capela.h"
#include "modelos/Cerca.h"
#include "modelos/PontoDeDescanso.h"
#include "modelos/Banco.h"
#include "modelos/Arvore.h"
#include "modelos/CadeiraPraia.h"

using namespace std;

#include "CameraDistante.h"
#include "Desenha.h"

#include "gui.h"

#include "stanfordbunny.h"
#include "model3ds.h"


// ----------------
// picking project
static int slices = 200; //400;//16;
static int stacks = 200; //400;//16;


static bool iluminacao = true;
static bool light0 = true;
static bool light1 = false;

static bool trans_luz = false;
static bool luz_pontual = false;
static bool spot_light = false;
static float spot_angle = 25.0;


// transparência
// aqui
float trans_obj = false;
static bool obj_transp = false;
static float transparencia = 0.5;

static bool multiple_viewports = false;

static bool draw_eixos = true;

static float lx = 0.0;
static float ly = 0.0;
static float lz = 0.0;



GLuint gBunnyWireList = NULL;
GLuint gBunnySolidList = NULL;
string str;
//variaveis globais
int width = 800;
int height = 600;


float tx = 0.0;
float ty = 0.0;
float tz = 0.0;

float ax = 0.0;
float ay = 0.0;
float az = 0.0;

float delta = 5.0;

float sx = 1.0;
float sy = 1.0;
float sz = 1.0;

//ponto em coords locais, a ser calculado em coords de mundo
float pl[4] = { 0.0, 0.0, 0.0, 1.0 };
//ponto em coords globais, resultado da conversao a partir de pl
float pg[4] = { 0.0, 0.0, 0.0, 1.0 };

bool lbpressed = false;
bool mbpressed = false;
bool rbpressed = false;

float last_x = 0.0;
float last_y = 0.0;


// Criando objetos do cenario
Casa *casa = new Casa();
Placa *placa = new Placa();
Capela *capela = new Capela();
Cerca *cerca1 = new Cerca();
PontoDeDescanso *pontodedescanso = new PontoDeDescanso();
Banco *banco1 = new Banco();
Arvore *arvoreLago1 = new Arvore();
CadeiraPraia *cadeirapraia = new CadeiraPraia();


bool mover = false;
// Vetor de objetos base
vector<Objeto*> objs;
vector<Objeto*> objFixed;
int posVetor = -1;
int posSelecionado = -1;
const int CASA = 1, PLACA = 2, CAPELA = 3, CERCA = 4, PONTODEDESCANSO = 5, BANCO = 6, ARVORE = 7, DOG = 8, CADEIRAPRAIA = 9;

// PARA MOVER OBJETO
bool moverObjeto = false;
bool objetoSelecionado = false;

Camera* cam = new CameraDistante();
float savedCamera[9];

// Arquivos de saída e entrada
ofstream arquivo_saida;
ifstream arquivo_entrada;

// CRIAMOS VÁRIAS CÂMERAS PARA SEREM POSICIONADAS AO LONGO DO CENÁRIO
Camera* cam2 = new CameraDistante(-3,2,-5, 0,0,0, 0,1,0);
Camera* cam3 = new CameraDistante(-13.4901, 1.16263, 5.89514, 1.82564, 1.24932, 1.81578, -0.00528537, 0.999985, 0.00140776);

bool manual_cam = false;
bool change_manual_cam = false;


//Picking project
// COMENTADO PRA SOMBRA ADD LINHA ABAIXO
//GLfloat lightPosition[4];


GLfloat light_position[] = { 1.5f, 1.5f, 1.5f, 1.0f };

#define BUFSIZE 512
GLuint selectBuf[BUFSIZE];

void mult_matriz_vetor(float res[4], float matriz[16], float entr[4]) {
    for (int i = 0; i < 4; i++) {
        res[i] = 0.0;
        for (int j = 0; j < 4; j++) {
            //res[i] += matriz[4*i+j] * entr[j];
            res[i] += matriz[4*j+i] * entr[j]; //matriz^T.entr
        }
    }
}

void mostra_matriz_transform(float matriz[16], bool transposta = true) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (!transposta) {
                cout << setiosflags (ios::fixed) << setprecision(2) << matriz[4*i+j] << "  ";
            } else {
                cout << setiosflags (ios::fixed) << setprecision(2) << matriz[4*j+i] << "  "; //matriz^T
            }
        }
        cout << "\n";
    }
    //cout << "\n";
}

void imprime_coords_locais_globais()
{
    //imprimindo coords locais e coords globais
      //locais
        cout << "Coords locais: " << pl[0] << ", " << pl[1] << ", " << pl[2] << "\n";
      //globais
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
            glLoadIdentity();
              //composicao de transformacoes
              glTranslated(tx,ty,tz);
              glRotated(az,0,0,1);
              glRotated(ay,0,1,0);
              glRotated(ax,1,0,0);
            float mudanca_sist_coords[16];
            glGetFloatv(GL_MODELVIEW_MATRIX,mudanca_sist_coords);
            cout << "Matriz mudanca sist coords local=>global (T.Rz.Ry.Rx):\n";
            mostra_matriz_transform(mudanca_sist_coords);
            mult_matriz_vetor(pg,mudanca_sist_coords,pl);
            cout << "Coords globais: " << pg[0] << ", " << pg[1] << ", " << pg[2] << "\n\n";
        glPopMatrix();
}

void desenha_camera(float tam) {
    GLUquadricObj *quad = gluNewQuadric();

    glPushMatrix();
    Desenha::drawBox( -tam,-tam,0.0, tam,tam,2*tam );
      glTranslated(0,0,-tam);
      Desenha::gluClosedCylinder(quad, tam, tam/2, tam, slices,stacks);
    glPopMatrix();
    gluDeleteQuadric( quad );
}

// SOMBRAS
//void desenhaObjetosComSombra() {
//    glPushMatrix();
//        //composicao de transformacoes
//        glTranslated(tx,ty,tz);
//        glRotated(az,0,0,1);
//        glRotated(ay,0,1,0);
//        glRotated(ax,1,0,0);

//        glScaled(sx,sx,sx);
//        Desenha::drawBox( 0.0,0.0,0.0, 1.0,1.0,1.0 );
//    glPopMatrix();
//}

void transformacao_camera_2_global(Vetor3D e, Vetor3D c, Vetor3D u, bool mostra_matriz = false)
{
    //z'
    Vetor3D z_ = e - c;
    z_.normaliza();
    //x'
    Vetor3D x_ = u ^ z_;
    x_.normaliza();
    //y'
    Vetor3D y_ = z_ ^ x_;
    //y_.normaliza();

    //matriz de transformacao
        float transform[16] = {
                                x_.x,   y_.x,   z_.x,   e.x,
                                x_.y,   y_.y,   z_.y,   e.y,
                                x_.z,   y_.z,   z_.z,   e.z,
                                0.0,    0.0,    0.0,    1.0
                             };
        glMultTransposeMatrixf( transform );

    if (mostra_matriz) {
        cout << "Matriz mudanca sist coords camera2=>global (R t = x' y' z' e):\n";
        mostra_matriz_transform(transform,false);
        cout << "\n";
    }
}

void transformacao_global_2_camera(Vetor3D e, Vetor3D c, Vetor3D u, bool mostra_matriz = false)
{
    //z'
    Vetor3D z_ = e - c;
    z_.normaliza();
    //x'
    Vetor3D x_ = u ^ z_;
    x_.normaliza();
    //y'
    Vetor3D y_ = z_ ^ x_;
    //y_.normaliza();

    //t'=-R^T.t=R^T.(-e)
    Vetor3D t_;
        t_.x = x_ * ( e * (-1) );
        t_.y = y_ * ( e * (-1) );
        t_.z = z_ * ( e * (-1) );

    //matriz de transformacao
        float transform[16] = {
                                x_.x,   x_.y,   x_.z,   t_.x,
                                y_.x,   y_.y,   y_.z,   t_.y,
                                z_.x,   z_.y,   z_.z,   t_.z,
                                0.0,    0.0,    0.0,    1.0
                             };
        glMultTransposeMatrixf( transform );
    if (mostra_matriz) {
        cout << "Matriz mudanca sist coords global=>camera2 (R^T -R^T.t):\n";
        mostra_matriz_transform(transform,false);
        cout << "\n";
    }
}

//converte global para camera
void matrixGlobal2Cam( float ex, float ey, float ez,
                       float cx, float cy, float cz,
                       float ux, float uy, float uz ) {
    Vetor3D e(ex,ey,ez);
    Vetor3D c(cx,cy,cz);
    Vetor3D u(ux,uy,uz);

    Vetor3D k_ = !(e - c); //!k_; // ou !(e - c)
    // para virar unitário
    Vetor3D i_ = !(u ^ k_); //!i_;
    Vetor3D j_ = !(k_ ^ i_); //!j_; / temos o O que é o próprio e (O_ = e)


    Vetor3D Rt_O; // R transpsto O * para descobrir qual o 4°  vetor . que é a mult da matriz * O
        Rt_O.x = -(i_ * e); // i_ produto escalar com o O (que é  proprio olho)
        Rt_O.y = -(j_ * e);
        Rt_O.z = -(k_ * e);
        // prova- composiçao de transfromaçoes, mudança de sistemas de coord. mudança de sist. cood mas dando inf de olho, centro
        //gerar dado inf de alto nivel. implementação no lookAAt.


    float matrix[16] = { i_.x, i_.y, i_.z, Rt_O.x,
                         j_.x, j_.y, j_.z, Rt_O.y,
                         k_.x, k_.y, k_.z, Rt_O.z,
                            0,    0,    0, 1 };

    glMultTransposeMatrixf(matrix);
}


/* GLUT callback Handlers */
void resize(int w, int h)
{
    width = w;
    height = h;
}

void sistema_localObjs(){
    // Objetos criados no Cenário
    glPushMatrix();
        for(int i = 0; i < objs.size(); i++){
              if(objs[i]->isVisivel()){
                glPushMatrix();
                    objs[i]->Desenhar_Objeto();
                glPopMatrix();
              }
          }
     glPopMatrix();
}

void sistema_local() {

    if (!iluminacao) glDisable(GL_LIGHTING); //iluminacao: enabled - cor do objeto definido pelo material (desconsidera glColor); disabled - considera glColor como a cor do objeto inteiro;
    //glEnable(GL_COLOR_MATERIAL); //enabled - mesmo com iluminacao habilitada, considera glColor como a cor do objeto
    if (!obj_transp) glDisable(GL_BLEND); //transparencia

    glColor4f(0.1,0.1,0.1,transparencia);

    glPushMatrix(); // fixo
            // LAGOA
                glPushMatrix();
                    glColor3d(0, 1, 1);
                    glTranslated(5,0,30);  // ESQ-DIR, ALTURA, PRONF.
                    Desenha::drawCylinder(-1, 0, 0, 15, 0.3, 17);  // ESQ-DIR, ALTURA, PRONF. , ESPE. , TAM.
                    if (!draw_eixos) Desenha::drawEixos( 0.5 );
                glPopMatrix();

                // CHÃO DA PRAÇA
                glPushMatrix();
                    glColor3d(3.2, 3.2, 3.2);
                    glTranslated(40,0,-22.5);
                    Desenha::drawCylinder(-1, 0, 0, 15, 0.9, 27);  // ESQ-DIR, ALTURA, PRONF. , ESPE. , TAM.
                    if (!draw_eixos) Desenha::drawEixos( 0.5 );
                 glPopMatrix();


            // ESTRADAS
                glPushMatrix();
                     glColor3d(0.24, 0.17, 0.12);
                    glTranslated(13,0,-65);  // ESQ-DIR, ALTURA, PRONF.
                    Desenha::drawBox(9, 0, 125, 5, 0.1, 7);
                    if (!draw_eixos) Desenha::drawEixos( 0.5 );
                glPopMatrix();

                 glPushMatrix();
                     glColor3d(0.24, 0.17, 0.12);
                    glTranslated(-40,0,35);  // ESQ-DIR, ALTURA, PRONF.
                    Desenha::drawBox(100, 0, 12, 5, 0.1, 7);
                    if (!draw_eixos) Desenha::drawEixos( 0.5 );
                 glPopMatrix();
    glPopMatrix();  // fixo

//        // Objetos criados no Cenário
//        glPushMatrix();
//            for(int i = 0; i < objs.size(); i++){
//                  if(objs[i]->isVisivel()){
//                    glPushMatrix();
//                        objs[i]->Desenhar_Objeto();
//                    glPopMatrix();
//                  }
//              }
//         glPopMatrix();

        //sistema local da camera
        if (!manual_cam) {
            glPushMatrix();
                //definindo sistema de coords atraves do glulookat (eye,center,up)
                transformacao_camera_2_global(cam2->e,cam2->c,cam2->u);
                transformacao_camera_2_global(cam3->e,cam3->c,cam3->u);
                glColor3d(0.5,0.5,0.0);
                desenha_camera(0.2);
            glPopMatrix();
        } else {
            glPushMatrix();
                //definindo sistema de coords atraves do glulookat (eye,center,up)
                transformacao_camera_2_global(cam->e,cam->c,cam->u);
                //desenhando eixos do sistema de coordenadas local da camera
                  Desenha::drawEixos( 0.5 );
                //desenhando camera
                glColor3d(0.5,0.5,0.5);
                desenha_camera(0.2);
            glPopMatrix();
        }

}

void displayInit()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void displayEnd()
{
    glutSwapBuffers();
}


void define_iluminacao()
{
//light0
    //habilita/desabilita light0
    if (iluminacao && light0) glEnable(GL_LIGHT0);
    else glDisable(GL_LIGHT0);
    //definindo intensidades de cor da light0
    const GLfloat light0_ambient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
    const GLfloat light0_diffuse[]  = { 0.7f, 0.7f, 0.7f, 1.0f };
    const GLfloat light0_specular[] = { 0.7f, 0.7f, 0.7f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  light0_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light0_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
    //posicionando light0
    GLfloat light0_position[] = { 1.0f + lx, 1.0f + ly, 1.0f + lz, 1.0f }; //4o parametro: 0.0 - luz no infinito, 1.0 - luz pontual
      if (!luz_pontual) light0_position[3] = 0.0f;
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    //desenha uma esfera representando a light0
    if (iluminacao && light0) {
        glDisable(GL_LIGHTING);
        glColor4f(1.0,1.0,1.0,1.0);
        glPushMatrix();
            glTranslatef(light0_position[0],light0_position[1],light0_position[2]);
//            glutSolidSphere(0.5,slices,stacks);
            glColor3d(1,0,0);
            glTranslated(10,10,10);  // ESQ-DIR, ALTURA, PRONF.
//            glutSolidSphere(2, 100 ,20);  // 1° arg tam. esfera, 2° arg. bordas esfera, 3° arg

            glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    //desenha uma linha do (0,0,0) at� a posi��o da light0
    if (iluminacao && light0 && trans_luz) {

        glDisable(GL_LIGHTING); // comentado para teste//MUDANDO A COR DA LUZ
        glColor4f(1.0,1.0,1.0,1.0);

//        glBegin(GL_LINES);
//           glVertex3f(10,10,10);
////            glVertex4fv(light0_position);
////            glColor3d(1,1,1);
////             glutSolidSphere(2, 100 ,20);  // 1° arg tam. esfera, 2° arg. bordas esfera, 3° arg
//           glEnd();

////        glutGUI::trans_luz = trans_luz;
////        GUI::setLight(0,light_position[0],light_position[1],light_position[2],false,false);
////        glLightfv(GL_LIGHT0, GL_POSITION, light_position);
////        Desenha::drawEixos( 0.5 );
//        glEnable(GL_LIGHTING);


        //sistema global
        glPushMatrix();
            //posicao da luz
                glutGUI::trans_luz = trans_luz;
                GUI::setLight(0,light_position[0],light_position[1],light_position[2],false,false);
                glLightfv(GL_LIGHT0, GL_POSITION, light_position);
            //desenhando eixos do sistema de coordenadas global
                Desenha::drawEixos( 0.5 );
            //chao
                glColor3d(0.6,0.4,0.0);
                glTranslated(0.0,-0.001,0.0);
                //Desenha::drawGrid( 10, 0, 10, 1 );
                GUI::drawFloor(15.0,15.0,0.5,0.5);
        glPopMatrix();

    }
    //spot_light
    if (spot_light) {
        const GLfloat light0_direction[] = { 0.0f, 0.0f, -1.0f, 1.0f }; //{ -(2.0f + lx), -(5.0f + ly), -(5.0f + lz), 1.0f };
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light0_direction);
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot_angle);
    } else {
        glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 180.0);
    }

//light1
    //habilita/desabilita light1
    if (iluminacao && light1) glEnable(GL_LIGHT1);
    else glDisable(GL_LIGHT1);
    //definindo intensidades de cor da light1
    const GLfloat light1_ambient[]  = { 0.02f, 0.02f, 0.02f, 1.0f };
    const GLfloat light1_diffuse[]  = { 0.4f, 0.4f, 0.4f, 1.0f };
    const GLfloat light1_specular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    glLightfv(GL_LIGHT1, GL_AMBIENT,  light1_ambient);
    glLightfv(GL_LIGHT1, GL_DIFFUSE,  light1_diffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
    //posicionando light1
    const GLfloat light1_position[] = { 5.0f, 2.0f, 2.0f, 1.0f };
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
    //desenha uma esfera representando a light1
    if (iluminacao && light1) {
        glDisable(GL_LIGHTING);
        glColor4f(1.0,1.0,1.0,1.0);
        glPushMatrix();
            glTranslatef(light1_position[0],light1_position[1],light1_position[2]);
            glutSolidSphere(0.05,slices,stacks);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }
}
void displayInner(bool manual_cam)
{
    //iluminacao
      define_iluminacao();
    //tempo
    const double t = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    //transformacao de camera
    if (!manual_cam) {
        //definindo posicao e orientacao da camera
        gluLookAt(cam->e.x,cam->e.y,cam->e.z, cam->c.x,cam->c.y,cam->c.z, cam->u.x,cam->u.y,cam->u.z);
        // DEFININDO NOSSA PRÓPRIA gluLookAt
       //   matrixGlobal2Cam(cam->e.x,cam->e.y,cam->e.z, cam->c.x,cam->c.y,cam->c.z, cam->u.x,cam->u.y,cam->u.z);
    } else {
        //definindo posicao e orientacao da camera 2 "manualmente"
        transformacao_global_2_camera(cam2->e,cam2->c,cam2->u);
        transformacao_global_2_camera(cam3->e,cam3->c,cam3->u);

    }



    // sistema global
    glPushMatrix();

// CHÃO
            glPushMatrix();
            float teta = (glutGet(GLUT_ELAPSED_TIME)/100);
                glColor3d(0.13, 0.55, 0.13);
                Desenha::drawBox(-60, 0, -60, 60, 0.02, 60);
                glColor3d(1,0,0);
                Desenha::drawEixos( 0.5 );
// ROTAÇÃO DA ESFERA
                //desenhando eixos do sistema de coordenadas global
                glTranslated(cos(teta*PI/100)*10,10, sin(teta*PI/100)*10);
                glutSolidSphere(2, 100 ,20);  // 1° arg tam. esfera, 2° arg. bordas esfera, 3° arg
            glPopMatrix();

    glPopMatrix();


//    //sistema global
//    glPushMatrix();
//        //posicao da luz
//            glutGUI::trans_luz = trans_luz;
//            GUI::setLight(0,light_position[0],light_position[1],light_position[2],false,false);
//            glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//        //desenhando eixos do sistema de coordenadas global
//            Desenha::drawEixos( 0.5 );
//        //chao
//            glColor3d(0.6,0.4,0.0);
//            glTranslated(0.0,-0.001,0.0);
//            //Desenha::drawGrid( 10, 0, 10, 1 );
//            GUI::drawFloor(15.0,15.0,0.5,0.5);
//    glPopMatrix();

// SISTEMA LOCAL 1 COM SOMBRA
    //sistema local 1 com sombra
    glPushMatrix();
        glColor3d(0.3,0.3,0.3);
//        desenhaObjetosComSombra();

        // descomentar
//        sistema_local(); // vai ter sombras
    sistema_localObjs();

    glPopMatrix();
//    glPushMatrix();
//        //matriz p multiplicar tudo por -1
//            //float neg[16] = {
//            //                   -1.0, 0.0, 0.0, 0.0,
//            //                    0.0,-1.0, 0.0, 0.0,
//            //                    0.0, 0.0,-1.0, 0.0,
//            //                    0.0, 0.0, 0.0,-1.0
//            //                };
//            //glMultTransposeMatrixf( neg );
//        //matriz de projecao para gerar sombra no plano y=0
//            float sombra[16] = {
//                                 light_position[1], -light_position[0],                0.0,                0.0,
//                                               0.0,                0.0,                0.0,                0.0,
//                                               0.0, -light_position[2],  light_position[1],                0.0,
//                                               0.0, -light_position[3],                0.0,  light_position[1]
//                               };
//            glMultTransposeMatrixf( sombra );
//        glDisable(GL_LIGHTING);
//        glColor3d(0.0,0.0,0.0);
////        desenhaObjetosComSombra();
////        sistema_local(); // retirar vai gerar as sombras
////        sistema_localObjs();

//        glEnable(GL_LIGHTING);
//    glPopMatrix();
    //sistema local 1 com distorção de projeção
    //glPushMatrix();
    //    glColor3d(0.3,0.3,0.3);
    //    glTranslatef(-0.5,-0.5,-2.5);
    //    desenhaObjetosComSombra();
    //glPopMatrix();
    //glPushMatrix();
    //    glColor3d(0.3,0.3,0.3);
    //    //glTranslatef(2.,0,0);
    //    //matriz p multiplicar z por -1
    //        float zNeg[16] = {
    //                            1.0, 0.0, 0.0, 0.0,
    //                            0.0, 1.0, 0.0, 0.0,
    //                            0.0, 0.0,-1.0, 0.0,
    //                            0.0, 0.0, 0.0, 1.0
    //                         };
    //        glMultTransposeMatrixf( zNeg );
    //    //matriz de projecao perspectiva
    //        gluPerspective(30.,1.,1.5,2.5);
    //        //glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 2.5);
    //    //matriz de projecao ortografica
    //        //glOrtho(-1.0, 1.0, -1.0, 1.0, 1.0, 3.0);
    //    glTranslatef(-0.5,-0.5,-2.5);
    //    desenhaObjetosComSombra();
    //glPopMatrix();

    //matriz de projecao perspectiva (gluPerspective)
    //    float far = 2.5;
    //    float near = 1.5;
    //    float esq = (far+near)/(far-near);
    //    float dir = (2*far*near)/(far-near);
    //    float persp[16] = {
    //                        1.0, 0.0, 0.0, 0.0,
    //                        0.0, 1.0, 0.0, 0.0,
    //                        0.0, 0.0, esq, dir,
    //                        0.0, 0.0,-1.0, 0.0
    //                      };
    //glMultTransposeMatrixf( persp );
    //    float fovy = 30.;
    //    float aspect = 1.;
    //    float f = 1./tan((fovy/2.)*(PI/180.));
    //    float pri = f/aspect;
    //    float seg = f;
    //    float Escala[16] = {
    //                          pri, 0.0, 0.0, 0.0,
    //                          0.0, seg, 0.0, 0.0,
    //                          0.0, 0.0, 1.0, 0.0,
    //                          0.0, 0.0, 0.0, 1.0
    //                       };
    //glMultTransposeMatrixf( Escala );





     sistema_local(); // chamei logo acima
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const float ar = height>0 ? (float) width / (float) height : 1.0;
    displayInit();
    displayInner(manual_cam);
    //displayEnd();

//    glMatrixMode(GL_PROJECTION);
//    glLoadIdentity();
//    gluPerspective(30.,ar,0.1,1000.);


    if (!multiple_viewports) {
        //viewport unica
        glViewport(0, -40, 1370, 800);

            glLoadIdentity();
            gluLookAt(cam->e.x,cam->e.y,cam->e.z, cam->c.x,cam->c.y,cam->c.z, cam->u.x,cam->u.y,cam->u.z);
                displayInner(manual_cam);

    } else {
        //viewport 01 - top left
        glViewport(0, height/2.0, width/2.0, height/2.0);
            glLoadIdentity();
            gluLookAt(cam2->e.x,cam2->e.y,cam2->e.z, cam2->c.x,cam2->c.y,cam2->c.z, cam2->u.x,cam2->u.y,cam2->u.z);
                displayInner(manual_cam);
        //viewport 02 - top right
        glViewport(width/2.0, height/2.0, width/2.0, height/2.0);
              glLoadIdentity();
            gluLookAt(0,1,5, 0,1,0, 0,1,0);
              displayInner(manual_cam);


        //viewport 03 - bottom left
        glViewport(0, 0, width/2.0, height/2.0);
            glLoadIdentity();
            gluLookAt(5,1,0, 0,1,0, 0,1,0);
                displayInner(manual_cam);
        glViewport(width/2.0, 0, width/2.0, height/2.0);
            glLoadIdentity();
            gluLookAt(0,6,0, 0,1,0, 0,0,-1);
                displayInner(manual_cam);

// *** DEIXEI EM CADA IF ELSE POIS SE FICAR FORA AFETARÁ O CENÁRIO
//    glutSwapBuffers();
    }
    glutSwapBuffers();

}

void mouseButton(int button, int state, int x, int y) {
// if the left button is pressed
    if (button == GLUT_LEFT_BUTTON) {
// when the button is pressed
        if (state == GLUT_DOWN) {
            lbpressed = true; // se picking
//            lbpressed = true;
// picking tem q corrigir
            //picking
//            int pick = picking( x, y, 685, 400 );
//            int pick = picking( x, y, 342.5, 200 );
//            std::cout << pick << std::endl;
//            if (pick == 0)
//              lbpressed = true;
//            else
//              objeto_selecionado = pick;
        } else {// state = GLUT_UP
            lbpressed = false;
        }
    }
    // if the middle button is pressed
    if (button == GLUT_MIDDLE_BUTTON) {
        // when the button is pressed
        if (state == GLUT_DOWN) {
            mbpressed = true;
        } else {// state = GLUT_UP
            mbpressed = false;
        }
    }
    // if the left button is pressed
    if (button == GLUT_RIGHT_BUTTON) {
        // when the button is pressed
        if (state == GLUT_DOWN) {
            rbpressed = true;
              //picking
//            objetoSelecionado = 0;
        } else {// state = GLUT_UP
            rbpressed = false;
        }
    }
    last_x = x;
    last_y = y;
}
void mouseMove(int x, int y) {
    float fator = 10.0;
    if (lbpressed && !rbpressed && !mbpressed) {
        if (!trans_obj && (!trans_luz || !obj_transp)) {

            if (!manual_cam) {
                if (!change_manual_cam) {
                    cam->rotatex(y,last_y);
                    cam->rotatey(x,last_x);
                } else {
                    cam2->rotatex(last_y,y);
                    cam2->rotatey(last_x,x);
                    cam3->rotatex(last_y,y);
                    cam3->rotatey(last_x,x);
                }
            } else {
                if (!change_manual_cam) {
                    cam->rotatex(last_y,y);
                    cam->rotatey(last_x,x);
                } else {
                    cam2->rotatex(y,last_y);
                    cam2->rotatey(x,last_x);
                    cam3->rotatex(y,last_y);
                    cam3->rotatey(x,last_x);

                }
            }

        }
                else if (trans_luz && obj_transp) {
                    fator = 100.0;
                    transparencia -= (y - last_y)/fator;
                    if (transparencia < 0.0) transparencia = 0.0;
                    if (transparencia > 1.0) transparencia = 1.0;
                }

        else {
             // objs selecionados
             objs[posSelecionado]->ax  += (y - last_y)/fator;
             objs[posSelecionado]->ay  += (x - last_x)/fator;
        }


    }
    fator = 100.0;
    if (!lbpressed && rbpressed && !mbpressed) {
        if (!trans_obj) {
            if (!manual_cam) {
                if (!change_manual_cam) {
                    cam->translatex(x,last_x);
                    cam->translatey(y,last_y);
                } else {
                    cam2->translatex(last_x,x);
                    cam2->translatey(last_y,y);
                    cam3->translatex(last_x,x);
                    cam3->translatey(last_y,y);

                }
            } else {
                if (!change_manual_cam) {
                    cam->translatex(last_x,x);
                    cam->translatey(last_y,y);
                } else {
                    cam2->translatex(x,last_x);
                    cam2->translatey(y,last_y);
                    cam3->translatex(x,last_x);
                    cam3->translatey(y,last_y);
                }
            }
        } else {
            // ACRESCENTADO PRA SOMBRA
             if (trans_obj) { //ISSO
                // para movimentação dos objs selecionados
                objs[posSelecionado]->tx += (x - last_x)/fator;
                objs[posSelecionado]->ty += -(y - last_y)/fator;
             }// ISSO
             // ESSE IF TAMBÉM
             if (trans_luz) {
                 light_position[0] += (x - last_x)/fator;
                 light_position[1] += -(y - last_y)/fator;
             }
       }
    }
    if (lbpressed && rbpressed && !mbpressed) {
        if (!trans_obj) {
            if (!manual_cam) {
                if (!change_manual_cam) {
                    cam->zoom(y,last_y);
                } else {
                    cam2->zoom(last_y,y);
                    cam3->zoom(last_y,y);

                }
            } else {
                if (!change_manual_cam) {
                    cam->zoom(last_y,y);
                } else {
                    cam2->zoom(last_y,y);
                    cam3->zoom(last_y,y);

                }
            }
        } else {
            // ACRESCENTADO SOMBRA
            if (trans_obj) { // ISSO
                // para movimentação dos objs selecionados
                objs[posSelecionado]->tz += (y - last_y)/fator;
                fator = 10.0;
                objs[posSelecionado]->az += -(x - last_x)/fator;
            } // ISSO
            // ESSE IF TAMBÉM
            if (trans_luz) {
                light_position[2] += (y - last_y)/fator;
            }


        }
    }
    fator = 100.0;
    if (!lbpressed && !rbpressed && mbpressed) {
        if (!trans_obj) {
        } else {
            //sx *= 1+(x - last_x)/fator;
            // para movimentação dos objs selecionados
            objs[posSelecionado]->sx += (x - last_x)/fator;
            objs[posSelecionado]->sy += -(y - last_y)/fator;
        }
    }
    if (lbpressed && !rbpressed && mbpressed) {
        if (!trans_obj) {
        } else {
            objs[posSelecionado]->sz += (y - last_y)/fator;
        }
    }
    if (!lbpressed && rbpressed && mbpressed) {
        if (!trans_obj) {
        } else {
            pl[0] += (x - last_x)/fator;
            pl[1] += -(y - last_y)/fator;
            imprime_coords_locais_globais();
        }
    }

    last_x = x;
    last_y = y;
}
void key(unsigned char key, int x, int y)
{
    Objeto *objeto;
    bool eixo;
    static int pos_manual_cam = 0;
    const float ar = height>0 ? (float) width / (float) height : 1.0;

    float s;
    float d; //1.0/2.0;
    float alfa; //30
    //matriz de cisalhamento (projecao obliqua)
    float transform[16] = {
                                    1.0,    0.0,    -d*cos(alfa),    0.0,
                                    0.0,    1.0,    0.0,    0.0,
                                    0.0,    0.0,    1.0,    0.0,
                                    0.0,    0.0,    0.0,    1.0
                                 };

    switch (key)
    {

    // -----------------------------------------------------------
        // MUDANDO PERSPECTIVAS DE CÂMERA - OPÇÕES A,B,D,E
    // -----------------------------------------------------------
    case 'E' :
        // PROJEÇÃO PERSPECTIVA
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(30.,ar,0.1,1000.);
        break;
    case 'B' :
        // PROJEÇÃO ORTOGRÁFICA
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-10*ar, 10*ar, -10.0, 10.0, -100.0, 1000.0);
        break;
    case 'D' :
        // PROJEÇÃO PERSPECTIVA
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(100.,ar,0.1,1000.); //ângulo de abertura. 1° parâmetro ideia de distênciamento, distorção em relação ao plano. 30 mantem o cenário perfeito.
        break;
    // PROJEÇÃO OBLÍQUA
    case 'R' :
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        s = 3.0;
        glOrtho(-s, s, -s, s, -10.0, 100.0);
        d = 1.0; //1.0/2.0;
        alfa = alfa*(PI/180);
        glMultTransposeMatrixf( transform );
    break;
    case 'A' :
        // PROJEÇÃO PERSPECTIVA
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glFrustum(-ar, ar, -1.0, 1.0, 2.0, 200.0);
        break;


    case 'q':
            exit(0);
            break;
        case 'F':
            glutFullScreen();
            break;
        case 'f':
            glutReshapeWindow(1000,800);
            break;
//        case '_':
//            slices-=20;
//            stacks-=20;
//            if (slices<3 || stacks<3)
//            {
//                slices = 3;
//                stacks = 3;
//            }
//            break;

//        case 'i':
//            ax=ay=az=0.0;
//            tx=ty=tz=0.0;
//            sx=sy=sz=1.0;
//            lx=ly=lz=0.0;
//            spot_angle=25.0;
//            transparencia=0.5;
//            break;

        case 't':
            trans_obj = !trans_obj;
            break;

//        // TRANSPARÊNCIA NÃO TÁ  FUNCIONANDO
        case 'T':
             obj_transp = !obj_transp;
            break;
//luz
        case '2':  // DEU CERTO
             if(mover){
                trans_obj = !trans_obj;
                break;
            }
        case 'I':
            iluminacao = !iluminacao;
            if (!iluminacao) trans_luz = false;
            break;
        case '0':
            light0 = !light0;
            if (!light0) trans_luz = false;
            break;
        case '1':
            light1 = !light1;
            break;
        case 'l':
            trans_luz = !trans_luz;
            break;
        // incluir objetos no cenário
        case '3':
            posVetor++; // posVetor = -1 (0,  1,2,3,4,5,6,  quando for igual a 7) posVetor=0
            if(posVetor < objFixed.size()){
                Objeto *obj = objFixed[posVetor];
                objs.push_back(obj);
            }else{
                posVetor = 0;
            }
            break;
        // MOVER FRENTE
        case '4':
            if(!(posSelecionado < 0)){
                objs[posSelecionado]->setSelecionado(false);
                mover = false;
            }

            if(posSelecionado < (objs.size()-1) ){
                posSelecionado++;
                objs[posSelecionado]->setSelecionado(true); //Setando selecionado true para os objetos q estao visiveis
                mover = true;
            }else{
                posSelecionado = 0;
                objs[posSelecionado]->setSelecionado(true); //Setando selecionado true para o objetos na posicao 0
                mover = true;
            }
        break;
        // MOVER TRÁS
        case '5':

            if(!(posSelecionado < 0)){
                objs[posSelecionado]->setSelecionado(false);
                mover = false;
            }
            if(posSelecionado > 0 ){
                posSelecionado--;
                objs[posSelecionado]->setSelecionado(true); //Setando selecionado true para os objetos q estao visiveis
                mover = true;
            }else{
                posSelecionado = objs.size()-1;
                objs[posSelecionado]->setSelecionado(true); //Setando selecionado true para o objetos na ultima posicao
                mover = true;
            }
        break;
            case '+' :
                cout << " CRIAR CASA:\n";
                objeto = new Casa();
                objeto->setTranslacao(40,0,40);
                objs.push_back(objeto);
                break;
            case '-' :
                cout << " CRIAR IGREJA:\n";
                objeto = new Capela();
                objs.push_back(objeto);
                break;
//            case ',' :
//                cout << " CRIAR DOG:\n";
//                objeto = new Dog();
//                objs.push_back(objeto);
//                break;
            case '.' :
                cout << " CRIAR CERCA:\n";
                objeto = new Cerca();
                objeto->setTranslacao(40,0,20);
                objs.push_back(objeto);
                break;

            case '*' :
                cout << " CRIAR BANCO:\n";
                objeto = new Banco();
                objeto->setTranslacao(40,0,-5);
                objeto->setVisivel(true);
                objs.push_back(objeto);

                break;
            case '/' :
                cout << " CRIAR PontoDeDescanso:\n";
                objeto = new PontoDeDescanso();
                objeto->setTranslacao(-40,0,40);

                objs.push_back(objeto);
                break;
            case '=' :

                cout << " CRIAR ARVORE:\n";
                objeto = new Arvore();
                objeto->setTranslacao(-50,0,10);
                objs.push_back(objeto);
                break;
            case '>' :
                cout << " CRIAR PLACA:\n";
                objeto = new Placa();
                objeto->setTranslacao(-30,0,-40);

                objs.push_back(objeto);
                break;
//            case '<' :
//                cout << " CRIAR AGUIA:\n";
//                objeto = new Aguia();
//                objs.push_back(objeto);
//                break;
            case 'H' :

                cout << " CRIAR CadeiraPraia:\n";
                objeto = new CadeiraPraia();
                objeto->setTranslacao(10,0,20);

                objs.push_back(objeto);
                break;

            // CRIAR OBJ QUANDO SELECIONADO
            case '6':
                cout << " caso 6:\n";
                if(objs[posSelecionado]->isSelecionado()){
                    cout << " SELECIONADO:\n";
                    switch (objs[posSelecionado]->getTipo()) {
                    case CASA:
                        objeto = new Casa();
                        break;
                        case PLACA:
                            objeto = new Placa();
                            break;
                        case CAPELA:
                            objeto = new Capela();
                            break;
                        case CERCA:
                            objeto = new Cerca();
                            break;
                        case PONTODEDESCANSO:
                            objeto = new PontoDeDescanso();
                            break;
                        case BANCO:
                            objeto = new Banco();
                            break;
                        case ARVORE:
                            objeto = new Arvore();
                            break;
//                        case DOG:
//                            objeto = new Dog();
//                            break;
                        case CADEIRAPRAIA:
                            objeto = new CadeiraPraia();
                            break;

                        }

                        objs.push_back(objeto);
                        }
        break;
        // DELETAR OBJ QUANDO SELECIONADO
        case '7':
            if(objs[posSelecionado]->isSelecionado()){
                // delecao de um objeto em um intervalo
                // Assim  sendo o unico elemento entre ele e o proximo, e ele mesmo
                objs.erase(objs.begin()+posSelecionado, objs.begin()+(posSelecionado+1));
            }
            break;
        // TÁ DESENHANDO OS EIXOS
        case '8':
            eixo = objs[posSelecionado]->isDesenhaEixos();
            objs[posSelecionado]->setDesenharEixos(!eixo);
            break;
        // SOMBRA
        case 'U':
            light_position[3] = 1 - light_position[3];
            break;
        case 'p':
            luz_pontual = !luz_pontual;
            break;
        case 'P': // VER DEPOIS
            light_position[3] = 1 - light_position[3];
            break;
        case 'S':
            spot_light = !spot_light;
            break;
        case 'v':
            multiple_viewports = !multiple_viewports;
            break;
        case 'e':
            draw_eixos = !draw_eixos;
            break;
        case 'X':
            ax+=delta;
            if(trans_obj) ax+=delta;
            else { pl[0] += 0.01; imprime_coords_locais_globais(); };
            break;

        case 'Y':
            ay+=delta;
            if(trans_obj) ay+=delta;
            else { pl[1] += 0.01; imprime_coords_locais_globais(); };
            break;

        case 'Z':
            az+=delta;
            if(trans_obj) az+=delta;
            else { pl[2] += 0.01; imprime_coords_locais_globais(); };
            break;

        case 'x':
            ax-=delta;
            if(trans_obj) ax-=delta;
            else { pl[0] -= 0.01; imprime_coords_locais_globais(); };
            break;

        case 'y':
            ay-=delta;
            if(trans_obj) ay-=delta;
            else { pl[1] -= 0.01; imprime_coords_locais_globais(); };
            break;

        case 'z':
            az-=delta;
            if(trans_obj) az-=delta;
            else { pl[2] -= 0.01; imprime_coords_locais_globais(); };
            break;
//        case 'c':
//            static int posCam = 0;
//            posCam++;
//            if (cam->estilo == 1) {
//                delete cam;
//                if (posCam%5==0) cam = new CameraDistante(); //CameraDistante(0,1,5, 0,1,0, 0,1,0);
//                if (posCam%5==1) cam = new CameraDistante(5,1,0, 0,1,0, 0,1,0); // olho onde ela tá, centro pra onde ela tá olhando
//                // é um vetor perpendicular.

//                if (posCam%5==2) cam = new CameraDistante(-64.9617, 46.4857, 71.3579, 5.88033, -3.14202, 6.02176, 0.336545, 0.889042, -0.310389);
//                if (posCam%5==3) cam = new CameraDistante(-5,1,0, 0,1,0, 0,1,0);
//                if (posCam%5==4) cam = new CameraDistante(0,5,0, 0,1,0, 0,0,-1); // NESSE CASO up é -1 pq ta olhando z negativo.
//                // como funciona o gluLookat prova

//                // GUARDA AS POSIÇÕES DA CÂMERA
//               //if (posCam%5==4) cam = new CameraDistante(savedCamera[0],savedCamera[1],savedCamera[2],savedCamera[3],savedCamera[4],savedCamera[5],savedCamera[6],savedCamera[7],savedCamera[8]);
//            } else if (cam->estilo == 2) {
//                delete cam;
//                cam = new CameraDistante();
//            }
//            break;
            // PARA A PROJEÇÃO ORTOGRÁFICA - Front-elevation, Side-elevation, Plan-elevation
                   case 'c':
                       static int posCam = 0;
                       posCam++;
                       if (cam->estilo == 1) {
                           delete cam;
                           if (posCam%5==0) cam = new CameraDistante(); //CameraDistante(0,1,5, 0,1,0, 0,1,0);
                           if (posCam%5==1) cam = new CameraDistante(5,1,0, 0,1,0, 0,1,0);
                           // olho onde ela tá, centro pra onde ela tá olhando
                                          // é um vetor perpendicular.
                           if (posCam%5==2) cam = new CameraDistante(0,1,-5, 0,1,0, 0,1,0);
                           if (posCam%5==3) cam = new CameraDistante(0,5,0, 0,1,0, 0,0,-1); // NESSE CASO up é -1 pq ta olhando z negativo. // CameraDistante(1,1,0, 1,0,0, 0,0,-1); // ISOMETRIC
                           // como funciona o gluLookat prova

           // GUARDA AS POSIÇÕES DA CÂMERA
                           if (posCam%5==4) cam = new CameraDistante(savedCamera[0],savedCamera[1],savedCamera[2],savedCamera[3],savedCamera[4],savedCamera[5],savedCamera[6],savedCamera[7],savedCamera[8]);
                       } else if (cam->estilo == 2) {
                           delete cam;
                           cam = new CameraDistante();
                       }
                       break;
           // PARA A PROJEÇÃO ORTOGRÁFICA - Isometric, Dimetric, Trimetric
                   case 'i':
                       static int posCam3 = 0;
                       posCam3++;
                       if (cam->estilo == 1) {
                           delete cam;
                           if (posCam3%5==0) cam = new CameraDistante();
                           if (posCam3%5==1) cam = new CameraDistante(5,5,5, 0,0,1, 0,1,0); // ISOMETRIC 3 eixos iguais (x,y 30°)
                           if (posCam3%5==2) cam = new CameraDistante(5,0,0, 0,0,5, 0,1,0); // DIMETRIC 2 eixos iguais
                           if (posCam3%5==3) cam = new CameraDistante(-64.9617, 46.4857, 71.3579, 5.88033, -3.14202, 6.02176, 0, 1, 0); // TRIMETRIC
                           if (posCam3%5==4) cam = new CameraDistante(savedCamera[0],savedCamera[1],savedCamera[2],savedCamera[3],savedCamera[4],savedCamera[5],savedCamera[6],savedCamera[7],savedCamera[8]);
                       } else if (cam->estilo == 2) {
                           delete cam;
                           cam = new CameraDistante();
                       }
                       break;

                   // PARA A PROJEÇÃO ORTOGRÁFICA - Front-elevation, Side-elevation, Plan-elevation
                   case 'd':
                       static int posCam2 = 0;
                       posCam2++;
                       if (cam->estilo == 1) {
                           delete cam;
                           if (posCam2%5==0) cam = new CameraDistante(); //CameraDistante(0,1,5, 0,1,0, 0,1,0);
                           if (posCam2%5==1) cam = new CameraDistante(5,1,0, 0,1,0, 0,1,0); // 1° PONTO DDE FUGA
                           // olho onde ela tá, centro pra onde ela tá olhando
                                          // é um vetor perpendicular.
                           if (posCam2%5==2) cam = new CameraDistante(-5,0,-5, 0,0,5, 0,1,0); // 2 PONTOS DE FUGA
               //                if (posCam%5==3) cam = new CameraDistante(-5,1,0, 0,1,0, 0,0,-1);

                           if (posCam2%5==3) cam = new CameraDistante(5,5,5, 0,0,1, 0,1,0); // NESSE CASO up é -1 pq ta olhando z negativo.
                           // como funciona o gluLookat prova

               // GUARDA AS POSIÇÕES DA CÂMERA
                               if (posCam2%5==4) cam = new CameraDistante(savedCamera[0],savedCamera[1],savedCamera[2],savedCamera[3],savedCamera[4],savedCamera[5],savedCamera[6],savedCamera[7],savedCamera[8]);
               //                if (posCam%5==3) cam = new CameraDistante(-5,1,0, 0,1,0, 0,1,0);



                       } else if (cam->estilo == 2) {
                           delete cam;
                           cam = new CameraDistante();
                       }
                       break;

        case 'C':
            pos_manual_cam++;
            if (pos_manual_cam%4==0) {
                cout << "Mostrando camera 1/Alterando camera 1:\n";
                change_manual_cam = false;
                manual_cam = false;
            }
            if (pos_manual_cam%4==1) {
                cout << "Mostrando camera 1/Alterando camera 2:\n";
                change_manual_cam = true;
                manual_cam = false;
            }
            if (pos_manual_cam%4==2) {
                cout << "Mostrando camera 2/Alterando camera 2:\n";
                change_manual_cam = true;
                manual_cam = true;
            }
            if (pos_manual_cam%4==3) {
                cout << "Mostrando camera 2/Alterando camera 1:\n";
                change_manual_cam = false;
                manual_cam = true;
            }
            //manual_cam = !manual_cam;
            if (manual_cam) {
                glPushMatrix();
                    transformacao_global_2_camera(cam2->e,cam2->c,cam2->u,true);
                glPopMatrix();
            } else {
                glPushMatrix();
                    transformacao_camera_2_global(cam2->e,cam2->c,cam2->u,true);
                glPopMatrix();
            }
            break;

            case 'K':
                pos_manual_cam++;
                if (pos_manual_cam%4==0) {
                    cout << "Mostrando camera 1/Alterando camera 1:\n";
                    change_manual_cam = false;
                    manual_cam = false;
                }
                if (pos_manual_cam%4==1) {
                    cout << "Mostrando camera 1/Alterando camera 2:\n";
                    change_manual_cam = true;
                    manual_cam = false;
                }
                if (pos_manual_cam%4==2) {
                    cout << "Mostrando camera 2/Alterando camera 2:\n";
                    change_manual_cam = true;
                    manual_cam = true;
                }
                if (pos_manual_cam%4==3) {
                    cout << "Mostrando camera 2/Alterando camera 1:\n";
                    change_manual_cam = false;
                    manual_cam = true;
                }
                //manual_cam = !manual_cam;
                if (manual_cam) {
                    glPushMatrix();
                        transformacao_global_2_camera(cam3->e,cam3->c,cam3->u,true);
                    glPopMatrix();
                } else {
                    glPushMatrix();
                        transformacao_camera_2_global(cam3->e,cam3->c,cam3->u,true);
                    glPopMatrix();
                }
                break;

        case 's':
            //save current camera location
            savedCamera[0] = cam->e.x;
            savedCamera[1] = cam->e.y;
            savedCamera[2] = cam->e.z;
            savedCamera[3] = cam->c.x;
            savedCamera[4] = cam->c.y;
            savedCamera[5] = cam->c.z;
            savedCamera[6] = cam->u.x;
            savedCamera[7] = cam->u.y;
            savedCamera[8] = cam->u.z;
            break;
        case 'm':
            if (!manual_cam) {
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                    glLoadIdentity();
                      glTranslated(tx,ty,tz);
                      glRotated(az,0,0,1);
                      glRotated(ay,0,1,0);
                      glRotated(ax,1,0,0);
                      glScaled(sx,sy,sz);
                    float transform[16];
                    glGetFloatv(GL_MODELVIEW_MATRIX,transform);
                    cout << "Matriz composicao de transformacoes (T.Rz.Ry.Rx.S):\n";
                    mostra_matriz_transform(transform);
                    cout << "\n";
                glPopMatrix();
            } else {
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                    glLoadIdentity();
                      gluLookAt(cam2->e.x,cam2->e.y,cam2->e.z, cam2->c.x,cam2->c.y,cam2->c.z, cam2->u.x,cam2->u.y,cam2->u.z);
                    float transform[16];
                    glGetFloatv(GL_MODELVIEW_MATRIX,transform);
                    cout << "Matriz gluLookAt:\n";
                    mostra_matriz_transform(transform);
                    cout << "\n";
                glPopMatrix();
            }
            break;

        case 'w':
            arquivo_saida.open("cenarioTeste");
            for(int i = 0; i < objs.size(); i++){
                if(objs[i]->isVisivel()){
                    switch (objs[i]->getTipo()) {
                    case CASA:
                        arquivo_saida << "C" << objs[i]->getTX() << "," << objs[i]->getTY() << "," << objs[i]->getTZ() << ",";
                        arquivo_saida << objs[i]->getAX() << "," << objs[i]->getAY() << "," << objs[i]->getAZ() << ",";
                        arquivo_saida << objs[i]->getSX() << "," << objs[i]->getSY() << "," << objs[i]->getSZ() << "," << endl;
                        break;
                    case PLACA:
                        arquivo_saida << "P" << objs[i]->getTX() << "," << objs[i]->getTY() << "," << objs[i]->getTZ() << ",";
                        arquivo_saida << objs[i]->getAX() << "," << objs[i]->getAY() << "," << objs[i]->getAZ() << ",";
                        arquivo_saida << objs[i]->getSX() << "," << objs[i]->getSY() << "," << objs[i]->getSZ() << "," << endl;
                        break;
                    case CAPELA:
                        arquivo_saida << "I" << objs[i]->getTX() << "," << objs[i]->getTY() << "," << objs[i]->getTZ() << ",";
                        arquivo_saida << objs[i]->getAX() << "," << objs[i]->getAY() << "," << objs[i]->getAZ() << ",";
                        arquivo_saida << objs[i]->getSX() << "," << objs[i]->getSY() << "," << objs[i]->getSZ() << "," << endl;
                        break;
                    case CERCA:
                        arquivo_saida << "c" << objs[i]->getTX() << "," << objs[i]->getTY() << "," << objs[i]->getTZ() << ",";
                        arquivo_saida << objs[i]->getAX() << "," << objs[i]->getAY() << "," << objs[i]->getAZ() << ",";
                        arquivo_saida << objs[i]->getSX() << "," << objs[i]->getSY() << "," << objs[i]->getSZ() << "," << endl;
                        break;

                    case PONTODEDESCANSO:
                        arquivo_saida << "K" << objs[i]->getTX() << "," << objs[i]->getTY() << "," << objs[i]->getTZ() << ",";
                        arquivo_saida << objs[i]->getAX() << "," << objs[i]->getAY() << "," << objs[i]->getAZ() << ",";
                        arquivo_saida << objs[i]->getSX() << "," << objs[i]->getSY() << "," << objs[i]->getSZ() << "," << endl;
                        break;

                    case BANCO:
                        arquivo_saida << "B" << objs[i]->getTX() << "," << objs[i]->getTY() << "," << objs[i]->getTZ() << ",";
                        arquivo_saida << objs[i]->getAX() << "," << objs[i]->getAY() << "," << objs[i]->getAZ() << ",";
                        arquivo_saida << objs[i]->getSX() << "," << objs[i]->getSY() << "," << objs[i]->getSZ() << "," << endl;
                        break;

                    case ARVORE:
                        arquivo_saida << "A" << objs[i]->getTX() << "," << objs[i]->getTY() << "," << objs[i]->getTZ() << ",";
                        arquivo_saida << objs[i]->getAX() << "," << objs[i]->getAY() << "," << objs[i]->getAZ() << ",";
                        arquivo_saida << objs[i]->getSX() << "," << objs[i]->getSY() << "," << objs[i]->getSZ() << "," << endl;
                        break;
                    case DOG:
                        arquivo_saida << "D" << objs[i]->getTX() << "," << objs[i]->getTY() << "," << objs[i]->getTZ() << ",";
                        arquivo_saida << objs[i]->getAX() << "," << objs[i]->getAY() << "," << objs[i]->getAZ() << ",";
                        arquivo_saida << objs[i]->getSX() << "," << objs[i]->getSY() << "," << objs[i]->getSZ() << "," << endl;
                        break;
                    case CADEIRAPRAIA:
                        arquivo_saida << "p" << objs[i]->getTX() << "," << objs[i]->getTY() << "," << objs[i]->getTZ() << ",";
                        arquivo_saida << objs[i]->getAX() << "," << objs[i]->getAY() << "," << objs[i]->getAZ() << ",";
                        arquivo_saida << objs[i]->getSX() << "," << objs[i]->getSY() << "," << objs[i]->getSZ() << "," << endl;
                        break;


                    }
                }
            }
            arquivo_saida.close();
            cout << "Escrito com sucesso!! "  << endl;
            break;
        case 'r':
            arquivo_entrada.open("cenarioTeste");
            string linha, objetos_arq;
            string temp = "";
            int sinal = 1;
            double valor;
            Objeto *obj;
            vector<double> posicoes;
            if(arquivo_entrada.is_open()){
                while (true) {
                    getline(arquivo_entrada,linha);
                    if(!linha.empty()){
                        objetos_arq += linha[0];
                        temp = "";
                        for(int i = 1; i < linha.length(); i++){
                            if(linha[i] != ','){
                                if(linha[i] == '-'){
                                    sinal = -1;
                                }else{
                                    temp += linha[i];
                                }
                            }else{
                                valor = stod(temp) * sinal;
                                sinal = 1;
                                temp = "";
                                posicoes.push_back(valor);
                            }
                        }
                        // Tratando as informacoes do arquivo
                        int i = 0;
                        for(int j = 0; j < objetos_arq.length(); j++){
                            switch (linha[j]) {
                            case 'C':
                                obj = new Casa();
                                break;
                            case 'P':
                                obj = new Placa();
                                break;
                            case 'I':
                                obj = new Capela();
                                break;
                            case 'c':
                                obj = new Cerca();
                                break;
                            case 'K':
                                obj = new PontoDeDescanso();
                                break;
                            case 'B':
                                obj = new Banco();
                                break;
                            case 'A':
                                obj = new Arvore();
                                break;
                            case 'p':
                                obj = new CadeiraPraia();
                                break;
//                            case 'D':
//                                obj = new Dog();
//                                break;
                            }
                            // Setando as posicoes dos objetos
                            obj->setTranslacao(posicoes[i], posicoes[i+1], posicoes[i+2]);
                            obj->setRotacao(posicoes[i+3], posicoes[i+4], posicoes[i+5]);
                            obj->setEscala(posicoes[i+6], posicoes[i+7], posicoes[i+8]);
                            obj->setVisivel(true);
                            objs.push_back(obj);
                            i += 9;
                        }
                       }else{
                        break;
                    }
                }
                arquivo_entrada.close();
            }
            break;



    } // FIM CASOS

    glutPostRedisplay();
}
void idle(void)
{
    glutPostRedisplay();
}

void ajuda(){
    cout << "                                     "<< endl;
    cout << "-------------------------------------"<< endl;
    cout << "                                     "<< endl;
    cout << "PROJEÇÕES  PARALELAS"<< endl;
    cout << "caso  R: PROJEÇÃO OBLÍQUA"<< endl;
    cout << "caso  B: PROJEÇÃO ORTOGRÁFICA"<< endl;

    cout << "PROJEÇÕES  EM PERSPECTIVA"<< endl;
    cout << "caso  A: PROJEÇÃO PERSPECTIVA"<< endl;
    cout << "caso  D: PROJEÇÃO PERSPECTIVA"<< endl;
    cout << "caso  E: PROJEÇÃO PERSPECTIVA"<< endl;

    cout << "                                     "<< endl;
    cout << "-------------------------------------"<< endl;
    cout << "                                             PROJEÇÃO ORTOGRÁFICA"<< endl;
    cout << "+++++++++++++++++++++++++++++++++++++"<< endl;
    cout << "help()"<< endl;
    cout << "primeiro aperte F para deixar a tela cheia, depois aperte B para escolher a PROJEÇÃO ORTOGRÁFICA"<< endl;
    cout << "+++++++++++++++++++++++++++++++++++++"<< endl;
    cout << "                                     "<< endl;
    cout << "-------------------------------------"<< endl;
    cout << "Mude as vistas ORTOGRÁFICAS pressionando a tecla c"<< endl;
    cout << "• aperte c  a 1° vez - Front-elevation"<< endl;
    cout << "• aperte c  a 2° vez - Side-elevation"<< endl;
    cout << "• aperte c  a 3° vez - Plan-elevation"<< endl;
    cout << "• DEPOIS DISSO APERTE c mais 2 vezes para alternar entre os testes"<< endl;
    cout << "                                     "<< endl;
    cout << "-------------------------------------"<< endl;
    cout << "Mude as vistas ORTOGRÁFICAS pressionando a tecla i (Projeções axonométricas - Um tipo especial de projeção ORTOGRÁFICA)"<< endl;
    cout << "• aperte i  a 1° vez - Isometric (Ângulos iguais com dois eixos)"<< endl;
    cout << "• aperte i  a 2° vez -  Dimetric Ângulos diferentes com os eixos x, y e z)   "<< endl;
    cout << "• aperte i  a 3° vez - Trimetric(Ângulos iguais com os eixos x, y e z:)"<< endl;
    cout << "• DEPOIS DISSO APERTE i mais 2 vezes para alternar entre os testes"<< endl;


    cout << "                                     "<< endl;
    cout << "-------------------------------------"<< endl;
    cout << "                                             PROJEÇÃO PERSPECTIVA"<< endl;
    cout << "+++++++++++++++++++++++++++++++++++++"<< endl;
    cout << "help()"<< endl;
    cout << "primeiro aperte F para deixar a tela cheia, depois aperte D, A para escolher a PROJEÇÃO PERSPECTIVA"<< endl;
    cout << "+++++++++++++++++++++++++++++++++++++"<< endl;
    cout << "                                     "<< endl;
    cout << "-------------------------------------"<< endl;
    cout << "Mude as vistas PERSPECTIVAS pressionando a tecla d"<< endl;
    cout << "• aperte d  a 1° vez - 1-point perspective"<< endl;
    cout << "• aperte d  a 2° vez - 2-point perspective"<< endl;
    cout << "• aperte d  a 3° vez - 3-point perspective"<< endl;


    cout << "caso  q: SAIR DO PROGRAMA"<< endl;
    cout << "caso  F: TELA CHEIA"<< endl;
    cout << "caso  f: TELA PEQUENA"<< endl;
    cout << "caso  2: TRANSPARÊNCIA"<< endl;
    cout << "caso  I: ILUMINAÇAO"<< endl;
    cout << "caso  0: ESCURECER CENÁRIO"<< endl;
    cout << "caso  1: LUZ TIPO 1"<< endl;
    cout << "caso  l: LUZ NUM PONTO (PARA DÁ EFEITO AOS OBJETOS COM SOMBRA)"<< endl;
    cout << "caso  3: INCLUIR OBJETOS NO CENÁRIO"<< endl;
    cout << "caso  4: MOVER PARA FRENTE"<< endl;
    cout << "caso  4: MOVER PARA TRÁS"<< endl;
    cout << "                                     "<< endl;
    cout << "                                     "<< endl;
    cout << "CRIAR OBJETOS COM UMA TECLA ESPECÍFICA"<< endl;
    cout << "caso  +: CRIAR CASA"<< endl;
    cout << "caso  -: CRIAR IGREJA"<< endl;
    cout << "caso  ,: CRIAR DOG"<< endl;
    cout << "caso  .: CRIAR CERCA"<< endl;
    cout << "caso  *: CRIAR BANCO"<< endl;
    cout << "caso  /: CRIAR PONTO DE DESCANSO"<< endl;
    cout << "caso  =: CRIAR ÁRVORE"<< endl;
    cout << "caso  >: CRIAR PLACA"<< endl;
    cout << "caso  <: CRIAR ÁGUIA"<< endl;
    cout << "caso  H: CRIAR CADEIRA DE PRAIA"<< endl;
    cout << "caso  6: CRIAR UM NOVO OBJETO QUANDO SELECIONADO"<< endl;
    cout << "caso  7: DELETAR OBJETO QUANDO SELECIONADO"<< endl;
    cout << "caso  8: ALTERNAR ENTRE DESENHAR EIXOS DO OBJETO"<< endl;
    cout << "caso  U: DESENHAR SOMBRA"<< endl;
    cout << "caso  p: LUZ PONTUAL"<< endl;
    cout << "caso  S: LUZ spot"<< endl;
    cout << "caso  v: MÚLTIPLOS VIEWPORTS"<< endl;
    cout << "caso e: DESENHAR EIXO DO OBJETO"<< endl;
//    cout << "caso X: ROTACIONAR CENÁRIO NO EIXO X PARA A DIREITA "<< endl;
//    cout << "caso Y: ROTACIONAR CENÁRIO NO EIXO Y PARA A DIREITA "<< endl;
//    cout << "caso Z: ROTACIONAR CENÁRIO NO EIXO Z PARA A DIREITA "<< endl;
//    cout << "caso x: ROTACIONAR CENÁRIO NO EIXO x PARA A ESQUERDA "<< endl;
//    cout << "caso y: ROTACIONAR CENÁRIO NO EIXO y PARA A ESQUERDA"<< endl;
//    cout << "caso z: ROTACIONAR CENÁRIO NO EIXO z PARA A ESQUERDA"<< endl;
    cout << "caso c: ALTERNAR ENTRE DIFERENTES ÂNGULOS NO CENÁRIO"<< endl;
    cout << "caso C: CÂMERA 1 NO CENÁRIO"<< endl;
    cout << "caso K: CÂMERA 2 NO CENÁRIO"<< endl;
    cout << "caso w: SALVAR CENÁRIO ATUAL"<< endl;
    cout << "caso r: CARREGAR CENÁRIO"<< endl;
}

/* Program entry point */
int main(int argc, char *argv[])
{
        ajuda();
        objFixed.push_back(casa);
        objFixed.push_back(placa);
        objFixed.push_back(capela);
        objFixed.push_back(cerca1);
        objFixed.push_back(pontodedescanso);
        objFixed.push_back(banco1);
        objFixed.push_back(arvoreLago1);
        objFixed.push_back(cadeirapraia);
//        objFixed.push_back(dog);

        //chamadas de inicializacao da GLUT
        glutInit(&argc, argv);
        glutInitWindowSize(width,height);
        glutInitWindowPosition(10,10);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

        glutCreateWindow("Vilarejo");

        glutReshapeFunc(resize);
        glutDisplayFunc(display);
        glutKeyboardFunc(key);
        glutIdleFunc(idle);

        glutMouseFunc(mouseButton);
        glutMotionFunc(mouseMove);

//chamadas de inicializacao da OpenGL
        glClearColor(1,1,1,1);
        //glClearColor(0.3,0.3,0.3,1.0);

        glEnable(GL_LIGHTING);
        glEnable(GL_COLOR_MATERIAL);
        //glEnable(GL_CULL_FACE);
        //glCullFace(GL_BACK);
        glEnable(GL_NORMALIZE); //mantem a qualidade da iluminacao mesmo quando glScalef eh usada

        glShadeModel(GL_SMOOTH);
        //glShadeModel(GL_FLAT);

        glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LESS);

// PORÉM MESMO ASSIM A TRANSPARÊNCIA NÃO ESTÁ FUNCIONANDO
        glEnable(GL_BLEND); //habilita a transparencia
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //definindo uma luz
            glEnable(GL_LIGHT0);

            const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
            const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
            const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
// COMENTADO PRA SOMBRA
        //   const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

            const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
            const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
            const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
            const GLfloat high_shininess[] = { 100.0f };

            glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
            glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
            glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
// COMENTADO PRA SOMBRA
//            glLightfv(GL_LIGHT0, GL_POSITION, light_position);

            glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
            glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
            glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
            glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

            //glFrontFace(GL_CCW);
            gBunnySolidList = GenStanfordBunnySolidList ();
            gBunnyWireList = GenStanfordBunnyWireList ();

                const float ar = height>0 ? (float) width / (float) height : 1.0;

                glViewport(0, -40, 1370, 800);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(30.,ar,0.1,1000.);
            //iniciando o loop principal da glut
        glutMainLoop();
    return EXIT_SUCCESS;}


