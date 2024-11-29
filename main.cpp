#include <GLFW/glfw3.h>
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

struct Pontos {
    float x, y, z;
};

struct Transformar {
    int type;
    float x, y, z;
};

struct Triangulo {
    Pontos v1, v2, v3;
};

bool aux = false, pintar = false, lightOn = false, aux2 = true, mostrarPontos = true;
int cont = 0;
vector<Pontos> inicial;

void setupLight() {
    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat light_ambient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat light_diffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

// Função para carregar o arquivo .obj com os vértices, faces e transformações
bool loadObj(const string& filename, vector<Pontos>& vertices, vector<Pontos>& faces, vector<Transformar>& transformar) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo.\n";
        return false;
    }
    string line;
    while (getline(file, line)) {
        if (line.substr(0, 2) == "v ") {  // Lê vértices
            Pontos pontos;
            sscanf_s(line.c_str(), "v %f %f %f", &pontos.x, &pontos.y, &pontos.z);
            vertices.push_back(pontos);
        }
        else if (line.substr(0, 2) == "f ") {  // Lê faces
            Pontos face;
            sscanf_s(line.c_str(), "f %f %f %f", &face.x, &face.y, &face.z);
            faces.push_back(face);
        }
        else if (line.substr(0, 2) == "s ") {  // Lê escala
            Transformar transformacao;
            sscanf_s(line.c_str(), "s %f %f %f", &transformacao.x, &transformacao.y, &transformacao.z);
            transformacao.type = 1;
            transformar.push_back(transformacao);
        }
        else if (line.substr(0, 2) == "t ") {  // Lê translação
            Transformar transformacao;
            sscanf_s(line.c_str(), "t %f %f %f", &transformacao.x, &transformacao.y, &transformacao.z);
            transformacao.type = 2;
            transformar.push_back(transformacao);
        }
        else if (line.substr(0, 2) == "x ") {  // Lê rotação em X
            Transformar transformacao;
            sscanf_s(line.c_str(), "x %f", &transformacao.x);
            transformacao.y = 0;
            transformacao.z = 0;
            transformacao.type = 3;
            transformar.push_back(transformacao);
        }
        else if (line.substr(0, 2) == "y ") {  // Lê rotação em Y
            Transformar transformacao;
            sscanf_s(line.c_str(), "y %f", &transformacao.x);
            transformacao.y = 0;
            transformacao.z = 0;
            transformacao.type = 4;
            transformar.push_back(transformacao);
        }
        else if (line.substr(0, 2) == "z ") {  // Lê rotação em Z
            Transformar transformacao;
            sscanf_s(line.c_str(), "z %f", &transformacao.x);
            transformacao.y = 0;
            transformacao.z = 0;
            transformacao.type = 5;
            transformar.push_back(transformacao);
        }
        else if (line.substr(0, 2) == "c ") {  // Lê cisalhamento
            Transformar transformacao;
            sscanf_s(line.c_str(), "c %f %f %f", &transformacao.x, &transformacao.y, &transformacao.z);
            transformacao.type = 6;
            transformar.push_back(transformacao);
        }
        else if (line.substr(0, 2) == "e ") {  // Lê reflexão
            Transformar transformacao;
            sscanf_s(line.c_str(), "e %f %f %f", &transformacao.x, &transformacao.y, &transformacao.z);
            transformacao.type = 7;
            transformar.push_back(transformacao);
        }
    }
    file.close();
    return true;
}

static void teclado(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if ((key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        aux = true;
        mostrarPontos = !mostrarPontos;
    }
    //if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        //pintar = false; 
    //}
    //if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        //aux2 = !aux2;
    //}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void desenho(const vector<Triangulo>& triangulos) {
    glColor3f(1.0f, 1.0f, 1.0f); // Cor branca para modo wireframe
    glBegin(GL_LINE_LOOP);
    for (const auto& tri : triangulos) {
        glVertex3f(tri.v1.x, tri.v1.y, tri.v1.z);
        glVertex3f(tri.v2.x, tri.v2.y, tri.v2.z);
        glVertex3f(tri.v3.x, tri.v3.y, tri.v3.z);
    }
    glEnd();
}

void desenho2(vector<Triangulo> triangulos2) {
    glColor3f(1.0f, 0.0f, 0.0f); // Cor vermelha para modo wireframe
    for (auto& tri : triangulos2) {
        glBegin(GL_LINE_LOOP);
        glVertex3f(tri.v1.x, tri.v1.y, tri.v1.z);
        glVertex3f(tri.v2.x, tri.v2.y, tri.v2.z);
        glVertex3f(tri.v3.x, tri.v3.y, tri.v3.z);
        glEnd();
    }
    glFlush();
}

// Função para desenhar pontos nos vértices dos quadriláteros
void desenharPontos(const vector<Triangulo>& triangulos) {
    if (!mostrarPontos) return; // Não desenha os pontos se mostrarPontos for false
    glPointSize(5.0f); // Ajusta o tamanho dos pontos
    glColor3f(1.0f, 1.0f, 0.0f); // Cor amarela para os pontos

    glBegin(GL_POINTS);
    for (const auto& tri : triangulos) {
        glVertex3f(tri.v1.x, tri.v1.y, tri.v1.z);
        glVertex3f(tri.v2.x, tri.v2.y, tri.v2.z);
        glVertex3f(tri.v3.x, tri.v3.y, tri.v3.z);
    }
    glEnd();
}

// Função para aplicar transformações geométricas
void applyTransformation(const Transformar& transform, const Pontos& fixo) {
    glPushMatrix();
    glTranslatef(fixo.x, fixo.y, fixo.z);

    switch (transform.type) {
    case 1: // Escala
        glScalef(transform.x, transform.y, transform.z);
        break;

    case 2: // Translação
        glTranslatef(transform.x, transform.y, transform.z);
        break;

    case 3: // Rotação em X
        glRotatef(transform.x, 1.0f, 0.0f, 0.0f);
        break;

    case 4: // Rotação em Y
        glRotatef(transform.x, 0.0f, 1.0f, 0.0f);
        break;

    case 5: // Rotação em Z
        glRotatef(transform.x, 0.0f, 0.0f, 1.0f);
        break;

    case 6: // Cisalhamento
    {
        // Cria matriz de cisalhamento
        GLfloat shearMatrix[16] = {
            1.0f, transform.y, transform.z, 0.0f,
            transform.x, 1.0f, transform.z, 0.0f,
            transform.x, transform.y, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(shearMatrix);
    }
    break;

    case 7: // Reflexão
    {
        GLfloat mirrorMatrix[16] = {
            transform.x == 0 ? -1.0f : 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, transform.y == 0 ? -1.0f : 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, transform.z == 0 ? -1.0f : 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(mirrorMatrix);
    }
    break;
    }

    // Retorna do ponto fixo
    glTranslatef(-fixo.x, -fixo.y, -fixo.z);
}

// Função principal
int main() {
    string caminho = "sbezier.obj";
    const char* filename = caminho.c_str();
    vector<Pontos> vertices;
    vector<Pontos> faces;
    vector<Transformar> transformar;
    loadObj(filename, vertices, faces, transformar);

    if (!glfwInit()) {
        return -1;
    }

    vector<Pontos> inicial(vertices);
    GLFWwindow* window = glfwCreateWindow(700, 500, "Curvas de Bezier de Grau 3", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Inicialização do contexto OpenGL
    glfwMakeContextCurrent(window);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glfwSetKeyCallback(window, teclado);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    vector<Triangulo> tri;
    vector<Triangulo> tri2;
    int ponto1, ponto2, ponto3;

    for (int i = 0; i < faces.size(); i++) {
        ponto1 = (int)faces[i].x - 1;
        ponto2 = (int)faces[i].y - 1;
        ponto3 = (int)faces[i].z - 1;
        Triangulo ponto;
        ponto.v1 = vertices[ponto1];
        ponto.v2 = vertices[ponto2];
        ponto.v3 = vertices[ponto3];
        tri.push_back(ponto);
    }

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();

        if (!lightOn) {
            glDisable(GL_LIGHTING);
        }
        else {
            setupLight();
        }

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)width / (double)height, 1.0, 1000.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glFrustum(-320, 320, -240, 240.0, 25, 250);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // Posicionamento da câmera
        glTranslatef(0.0f, 0.0f, -50.0f);
        glRotatef(35.264f, 1.0f, 0.0f, 0.0f);
        glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);

        // Desenho dos eixos de coordenadas
        // Eixo X em verde
        glBegin(GL_LINES);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(0, 0.0f, 0.0f);
        glVertex3f(320, 0.0f, 0.0f);
        glEnd();

        // Eixo Y em azul
        glColor3f(0.0f, 0.0f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0, 0.0f);
        glVertex3f(0.0f, 240, 0.0f);
        glEnd();

        // Eixo Z em vermelho
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 320.0f);
        glEnd();

        // Construção dos triângulos a partir das faces
        for (int i = 0; i < faces.size(); i++) {
            ponto1 = (int)faces[i].x - 1;
            ponto2 = (int)faces[i].y - 1;
            ponto3 = (int)faces[i].z - 1;
            Triangulo ponto;
            ponto.v1 = vertices[ponto1];
            ponto.v2 = vertices[ponto2];
            ponto.v3 = vertices[ponto3];
            tri.push_back(ponto);
        }

        if (aux2 && cont != 0) {
            desenho2(tri2);
        }

        // Renderização do objeto original
        desenho(tri);

        // Desenho dos pontos dos vértices dos quadriláteros
        desenharPontos(tri);

        // Aplicação de transformações quando a tecla espaço é pressionada
        if (aux) {
            tri2.clear();
            for (const auto& triangle : tri) {
                tri2.push_back(triangle);
            }
            tri.clear();
            if (cont != transformar.size()) {
                applyTransformation(transformar[cont], vertices[0]);
                cont++;
            }
            else {
                cont = 0;
                glLoadIdentity();
            }
            aux = false;
        }

        // Renderização final dos objetos
        if (aux2 && cont != 0) {
            desenho2(tri2);
        }

        desenho(tri);

        // Atualização da tela e processamento de eventos
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpeza e encerramento
    glfwTerminate();

    return 0;
}