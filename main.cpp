#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-flp30-c"
#define USE_MATH_DEFINES

#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <climits>
#include <iomanip>
#include <chrono>
#include <vector>
#include <bitset>
#include <limits>
#include <string>
#include <random>
#include <glut.h>
#include <queue>
#include <stack>
#include <deque>
#include <cmath>
#include <list>
#include <set>
#include <map>

#define watch(x) cerr << "\n" << (#x) << " is " << (x) << endl
#define print(x) cerr << endl; for (auto a: x) cerr << (a) << " "; cerr << endl
#define alle(x) (x).begin(), (x).end()
#define ralle(x) (x).rbegin(), (x).rend()
#define lb(x, t) lower_bound(alle((x)), t)
#define ub(x, t) upper_bound(alle((x)), t)
#define pb push_back
#define el '\n'

using ull = unsigned long long;
using ll = long long;

using namespace std;

float xMin, yMin, xMax, yMax;
int windowWidth = 800, windowHeight = 600;

void setRange(float (*func)(float), float xStart, float xEnd) {
    xMin = xStart;
    yMin = func(xStart);

    xMax = xEnd;
    yMax = yMin;

    for (float x = xStart; x <= xEnd; x += 0.01) {
        float y = func(x);
        if (y < yMin) yMin = y;
        if (y > yMax) yMax = y;
    }
}

void reshape(int w, int h) {
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(xMin, xMax, yMin, yMax);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glutPostRedisplay();
}

void setupGL(float (*func)(float)) {
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2.0);
    setRange(func, -10.0, 10.0);
}

static const vector<float (*)(float)> funcs = {
        [](float x) { return sin(x); },
        [](float x) { return x * x; },
};

float (*cur_func)(float) = funcs[0];
static int window;
void menu(int num){
    if (num == -1) {
        glutDestroyWindow(window);
        exit(0);
    }
    cur_func = funcs[num];

    glutPostRedisplay();
}

void drawGraph(float (*func)(float)) {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.570, 0.570, 0.570);
    glLineWidth(1.0);

    glBegin(GL_LINES);

    for (float x = xMin; x <= xMax; x += 1) {
        glVertex2f(x, -0.01);
        glVertex2f(x, 0.01);
    }

    for (float y = yMin; y <= yMax; y += 1) {
        glVertex2f(-0.01, y);
        glVertex2f(0.01, y);
    }

    glVertex2f(xMin, 0); glVertex2f(xMax, 0);
    glVertex2f(0, yMin); glVertex2f(0, yMax);

    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(2.0);
    glBegin(GL_LINE_STRIP);
    for (float x = xMin; x <= xMax; x += 0.01) {
        glVertex2f(x, func(x));
    }
    glEnd();
    glutSwapBuffers();
}

void display() {
    drawGraph(cur_func);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

    glutInitWindowSize(windowWidth, windowHeight);
    glutCreateWindow("Graph Scaling Example");

    glutCreateMenu(menu);
    glutAddMenuEntry("sinx", 0);
    glutAddMenuEntry("x^2", 1);
    glutAddMenuEntry("Exit", -1);
    glutAttachMenu(GLUT_RIGHT_BUTTON);

    setupGL(cur_func);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}

#pragma clang diagnostic pop