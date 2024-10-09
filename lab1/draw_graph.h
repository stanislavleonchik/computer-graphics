/*
#ifndef COMPUTER_GRAPHICS_DRAW_GRAPH_H
#define COMPUTER_GRAPHICS_DRAW_GRAPH_H

#include "includes.h"

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

    for (float y = yMin; y <= yMax; y += 0.1) {
        glVertex2f(-0.05, y);
        glVertex2f(0.05, y);
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

void execute_graph(int argc, char** argv) {
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
}

#endif //COMPUTER_GRAPHICS_DRAW_GRAPH_H
*/
