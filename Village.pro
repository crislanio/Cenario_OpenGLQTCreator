TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

CONFIG += c++11

INCLUDEPATH += bib
INCLUDEPATH += gui_glut
INCLUDEPATH += user

DEFINES += USE_SDL
LIBS += -lglut -l3ds -lGLU -lGL -lm -lSDL -lSDL_image
#LIBS += -lGL -lGLU -lglut

HEADERS += \
    bib/Camera.h \
    bib/CameraDistante.h \
    bib/CameraJogo.h \
    bib/Desenha.h \
    bib/Vetor3D.h \
    bib/model3ds.h \
    bib/stanfordbunny.h \
    modelos/Casa.h \
    bib/Objeto.h \
    gui_glut/extra.h \
    gui_glut/gui.h \
    gui_glut/OpenTextures.h \
    modelos/Placa.h \
    modelos/Capela.h \
    modelos/Cerca.h \
    modelos/Banco.h \
    modelos/Arvore.h \
    modelos/PontoDeDescanso.h \
    modelos/CadeiraPraia.h

SOURCES += \
    bib/Camera.cpp \
    bib/CameraDistante.cpp \
    bib/CameraJogo.cpp \
    bib/Desenha.cpp \
    bib/Vetor3D.cpp \
    main.cpp \
    bib/model3ds.cpp \
    modelos/Casa.cpp \
    bib/Objeto.cpp \
    gui_glut/extra.cpp \
    gui_glut/gui.cpp \
    gui_glut/OpenTextures.cpp \
    modelos/Placa.cpp \
    modelos/Capela.cpp \
    modelos/Cerca.cpp \
    modelos/Banco.cpp \
    modelos/Arvore.cpp \
    modelos/PontoDeDescanso.cpp \
    modelos/CadeiraPraia.cpp
