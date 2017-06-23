# Cenario_OpenGLQTCreator


Cenário OpenGL C++  crislanio.wordpress.com

- Projeto para IDE **QtCreator** utilizando *C++, OpenGL, GLUT/Qt*;

## Dependências
- C++
- QtCreator
- OpenGL
- GLUT/Qt

## Instalação
- G++:
- sudo apt update
 - sudo apt install g++
 
- GLUT:
- sudo apt install freeglut3-dev

- MALHAS 3D:
- sudo apt install lib3ds-dev

- IMAGENS/TEXTURAS:
- sudo apt install libdevil-dev

- SDL:
- sudo apt install libsdl1.2-dev
- sudo apt install libsdl-image1.2-dev

- QT: siga o tutorial abaixo:
- https://wiki.qt.io/Install_Qt_5_on_Ubuntu

- OpenGL: siga o tutorial abaixo:
- https://pt.wikibooks.org/wiki/Programa%C3%A7%C3%A3o_com_OpenGL/Instala%C3%A7%C3%A3o/Linux




| Aplicação                     | Versão        |
| :---------------------------- |:-------------:|
| Village OpenGL C++            | 1.0.0           |

## Objetivos do trabalho

# Implementar um modelador 
  - já implementar com orientação a objeto
    - definir e usar classe Objeto
    - definir um método desenhar (virtual)
      - a ser definido por classes mais específicas (herdando da classe Objeto)
        - por exemplo: Arvore, ...
    - incluir atributos referentes às possíveis transformações
      - translação/rotação/escala
    - incluir um atributo na classe Objeto para alternar entre desenhar ou não os
     eixos do sistema de coordenadas local do objeto
      - usá-lo como teste no método desenhar()
  - definir um vetor de Objetos
    - permitir incluir/apagar objetos desse vetor (usando o teclado)
      - incluir objeto com sist local inicialmente alinhado com o sist global
        - usar o proprio modelador implementado para posicionar o objeto no cenario
      - permitir apagar o objeto selecionado ou o ultimo objeto do vetor
    - permitir alternar/selecionar os objetos desse vetor (de um a um, seguindo a ordem do vetor)
      - definir teclas para avançar e para voltar no vetor
    - marcar o objeto selecionado (pintar com uma cor de destaque, todo o objeto com a mesma cor)
      - incluir um atributo na classe Objeto para dizer se o objeto está ou não selecionado,
       se deve ou não ser pintado com a cor de destaque (branco, por exemplo)
        - usá-lo como teste no método desenhar()
  - permitir modificar os atributos do objeto selecionado
    - translação/rotação/escala (usando o mouse)
    - desenhar ou não eixos (usando alguma tecla)
  - permitir salvar/carregar todos os objetos (todos os seus atributos) do vetor em/de arquivo

- Modelar um cenário com um certo nível de detalhes 
  
# Câmeras 
  - (class Camera [CameraDistante e/ou CameraJogo])
    - que permite movimentar a câmera pelo cenário
  - pré-estabelecer posições/orientações de câmera específicas interessantes, de acordo com o cenário modelado
    - permitir alternar entre essas posições/orientações pré-estabelecidas usando o teclado


# Parte 02) 

# Projeções
  - alternar entre proj. perspectiva e proj. ortográfica
    - usando alguma tecla
  - definir posições/orientações de câmera específicas para simular todas as possíveis projeções
   clássicas, de acordo com a classificação mostrada em sala
    - não precisa simular as projeções oblíquas
    - pode focar em um objeto específico
  - permitir alternar entre essas posições/orientações pré-estabelecidas usando o teclado
- Sombras 
  - mostrar as sombras de todos os objetos
  - no chão (plano horizontal) e em uma parede qualquer (plano vertical)
    - mostrar resultado da sombra em um canto de parede
  - alternar entre luz pontual (proj. perspectiva) e luz distante (proj. oblíqua)

### Screenshots
![alt tag](https://raw.githubusercontent.com/crislanio/Cenario_OpenGLQTCreator/master/Imagens/cenario.png)
![alt tag](https://raw.githubusercontent.com/crislanio/Cenario_OpenGLQTCreator/master/Imagens/descricao.png)
![alt tag](https://raw.githubusercontent.com/crislanio/Cenario_OpenGLQTCreator/master/Imagens/multiplosViewPointsComMuitosFrames.png)
![alt tag](https://raw.githubusercontent.com/crislanio/Cenario_OpenGLQTCreator/master/Imagens/cruzSelecionada.png)
![alt tag](https://raw.githubusercontent.com/crislanio/Cenario_OpenGLQTCreator/master/Imagens/0-tecla-depois1-ilunimação.png)
![alt tag](https://raw.githubusercontent.com/crislanio/Cenario_OpenGLQTCreator/master/Imagens/0-tecla-Iluminação.png)

