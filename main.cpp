#include <GLFW/glfw3.h>
#include <iostream>
#include <GL/glut.h>
#include <vector>
#include <fstream>
#include <string>
using namespace std;

struct Pontos {
    float x, y, z;
};

struct Transformar {
    int type;   // Tipo de transformação
    float x, y, z;
};

// Vetores para armazenar vértices e transformações
vector<Pontos> controlPoints;
vector<Transformar> transformar;

// Controle da exibição
bool mostrarPontos = false;

// Função para carregar o arquivo .obj
bool loadObj(const string& filename, vector<Pontos>& controlPoints, vector<Transformar>& transformar) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo .obj.\n";
        return false;
    }

    string line;
    while (getline(file, line)) {
        if (line.substr(0, 2) == "v ") {  // Lê vértices
            Pontos ponto;
            sscanf_s(line.c_str(), "v %f %f %f", &ponto.x, &ponto.y, &ponto.z);
            controlPoints.push_back(ponto);
        }
        else if (line.substr(0, 2) == "s ") {  // Lê escala
            Transformar t;
            sscanf_s(line.c_str(), "s %f %f %f", &t.x, &t.y, &t.z);
            t.type = 1;
            transformar.push_back(t);
        }
        else if (line.substr(0, 2) == "t ") {  // Lê translação
            Transformar t;
            sscanf_s(line.c_str(), "t %f %f %f", &t.x, &t.y, &t.z);
            t.type = 2;
            transformar.push_back(t);
        }
        else if (line.substr(0, 2) == "x ") {  // Lê rotação em X
            Transformar t;
            sscanf_s(line.c_str(), "x %f", &t.x);
            t.y = 0;
            t.z = 0;
            t.type = 3;
            transformar.push_back(t);
        }
        else if (line.substr(0, 2) == "y ") {  // Lê rotação em Y
            Transformar t;
            sscanf_s(line.c_str(), "y %f", &t.x);
            t.y = 0;
            t.z = 0;
            t.type = 4;
            transformar.push_back(t);
        }
        else if (line.substr(0, 2) == "z ") {  // Lê rotação em Z
            Transformar t;
            sscanf_s(line.c_str(), "z %f", &t.x);
            t.y = 0;
            t.z = 0;
            t.type = 5;
            transformar.push_back(t);
        }
        else if (line.substr(0, 2) == "c ") {  // Lê cisalhamento
            Transformar t;
            sscanf_s(line.c_str(), "c %f %f %f", &t.x, &t.y, &t.z);
            t.type = 6;
            transformar.push_back(t);
        }
        else if (line.substr(0, 2) == "e ") {  // Lê reflexão
            Transformar t;
            sscanf_s(line.c_str(), "e %f %f %f", &t.x, &t.y, &t.z);
            t.type = 7;
            transformar.push_back(t);
        }
    }

    file.close();
    return true;
}

// Função para calcular os coeficientes de Bézier
Pontos calculaSuperficieBezier(const vector<vector<Pontos>>& controlPoints, float u, float v) {
    int n = controlPoints.size() - 1;
    int m = controlPoints[0].size() - 1;
    Pontos point = { 0, 0, 0 };

    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= m; ++j) {
            float binomialCoeffI = tgamma(n + 1) / (tgamma(i + 1) * tgamma(n - i + 1));
            float binomialCoeffJ = tgamma(m + 1) / (tgamma(j + 1) * tgamma(m - j + 1));
            float bernsteinPolyI = binomialCoeffI * pow(u, i) * pow(1 - u, n - i);
            float bernsteinPolyJ = binomialCoeffJ * pow(v, j) * pow(1 - v, m - j);
            point.x += bernsteinPolyI * bernsteinPolyJ * controlPoints[i][j].x;
            point.y += bernsteinPolyI * bernsteinPolyJ * controlPoints[i][j].y;
            point.z += bernsteinPolyI * bernsteinPolyJ * controlPoints[i][j].z;
        }
    }
    return point;
}

// Função para desenhar a superfície de Bézier
void desenharSuperficieBezier(const vector<vector<Pontos>>& controlPoints) {
    int resolution = 100;
    glColor3f(0.0f, 1.0f, 0.5f);
    glBegin(GL_TRIANGLES);

    for (int i = 0; i < resolution; ++i) {
        float u = (float)i / resolution;
        float uNext = (float)(i + 1) / resolution;

        for (int j = 0; j < resolution; ++j) {
            float v = (float)j / resolution;
            float vNext = (float)(j + 1) / resolution;

            Pontos p1 = calculaSuperficieBezier(controlPoints, u, v);
            Pontos p2 = calculaSuperficieBezier(controlPoints, uNext, v);
            Pontos p3 = calculaSuperficieBezier(controlPoints, u, vNext);
            Pontos p4 = calculaSuperficieBezier(controlPoints, uNext, vNext);

            glVertex3f(p1.x, p1.y, p1.z);
            glVertex3f(p2.x, p2.y, p2.z);
            glVertex3f(p3.x, p3.y, p3.z);

            glVertex3f(p2.x, p2.y, p2.z);
            glVertex3f(p3.x, p3.y, p3.z);
            glVertex3f(p4.x, p4.y, p4.z);
        }
    }

    glEnd();
}

// Função para desenhar pontos de controle e as linhas conectando-os
void desenharPontosEConexoes(const vector<vector<Pontos>>& controlPoints) {
    glColor3f(1.0f, 1.0f, 0.0f);  // Amarelo para os pontos
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    for (const auto& linha : controlPoints) {
        for (const auto& ponto : linha) {
            glVertex3f(ponto.x, ponto.y, ponto.z);
        }
    }
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);  // Branco para as linhas
    glBegin(GL_LINES);
    for (int i = 0; i < controlPoints.size(); ++i) {
        for (int j = 0; j < controlPoints[i].size() - 1; ++j) {
            glVertex3f(controlPoints[i][j].x, controlPoints[i][j].y, controlPoints[i][j].z);
            glVertex3f(controlPoints[i][j + 1].x, controlPoints[i][j + 1].y, controlPoints[i][j + 1].z);
        }
    }

    for (int j = 0; j < controlPoints[0].size(); ++j) {
        for (int i = 0; i < controlPoints.size() - 1; ++i) {
            glVertex3f(controlPoints[i][j].x, controlPoints[i][j].y, controlPoints[i][j].z);
            glVertex3f(controlPoints[i + 1][j].x, controlPoints[i + 1][j].y, controlPoints[i + 1][j].z);
        }
    }
    glEnd();
}



// Função para aplicar transformações geométricas
void applyTransformation(const Transformar& transform) {
    glPushMatrix();

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

    glPopMatrix();
}

// Callback para ajustar o viewport e a perspectiva
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspectRatio = (float)width / (float)height;
    gluPerspective(60.0f, aspectRatio, 1.0f, 100.0f);
}

// Callback de teclado
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        mostrarPontos = !mostrarPontos;
    }
    if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    // Carregar arquivo .obj
    string caminho = "C:\\Users\\Sal\\source\\repos\\Projeto2\\Projeto2\\flor.obj"; // Substituir pelo caminho real
    if (!loadObj(caminho, controlPoints, transformar)) {
        return -1;
    }

    // Preparar os pontos de controle em uma estrutura bidimensional
    // Supondo que seja necessário definir manualmente a estrutura (exemplo: uma grade 4x4)
    vector<vector<Pontos>> controlPoints2D;
    int rows = 4; // Número de linhas
    int cols = 4; // Número de colunas
    if (controlPoints.size() < rows * cols) {
        cerr << "Pontos insuficientes para formar uma grade " << rows << "x" << cols << ".\n";
        return -1;
    }
    for (int i = 0; i < rows; ++i) {
        vector<Pontos> row;
        for (int j = 0; j < cols; ++j) {
            row.push_back(controlPoints[i * cols + j]);
        }
        controlPoints2D.push_back(row);
    }

    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(700, 500, "Transformações com .obj", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -50.0f);
        glRotatef(35.264f, 1.0f, 0.0f, 0.0f);
        glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);

        // Aplicar transformações carregadas
        for (const auto& transform : transformar) {
            applyTransformation(transform);
        }

        // Desenhar os pontos e conexões, e a superfície de Bézier
        if (mostrarPontos) {
            desenharPontosEConexoes(controlPoints2D);
        }
        desenharSuperficieBezier(controlPoints2D);

        // Desenho dos eixos de coordenadas
        glBegin(GL_LINES);
        glColor3f(0.0f, 1.0f, 0.0f);  // Eixo X em verde
        glVertex3f(0, 0.0f, 0.0f);
        glVertex3f(320, 0.0f, 0.0f);
        glEnd();

        glColor3f(0.0f, 0.0f, 1.0f);  // Eixo Y em azul
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0, 0.0f);
        glVertex3f(0.0f, 240, 0.0f);
        glEnd();

        glColor3f(1.0f, 0.0f, 0.0f);  // Eixo Z em vermelho
        glBegin(GL_LINES);
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 320.0f);
        glEnd();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}