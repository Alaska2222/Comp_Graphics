#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <iostream>

// БАГ З МЕНЮ
// БАГ З ВИДАЛЕННЯМ СТАНУ

int currentStateIndex = 0;
int draggedVertexIndex = -1;
bool menu_active = false;
GLfloat currentColor[3] = { 1.0f, 1.0f, 1.0f };

struct Point {
    float x, y;
    Point(float _x, float _y) : x(_x), y(_y) {}
    Point() : x(0.0), y(0.0) {}
};

struct Triangle {
    Point p1, p2, p3;
    GLfloat color[3];
    Triangle(Point _p1, Point _p2, Point _p3) : p1(_p1), p2(_p2), p3(_p3) {}
    Triangle(Point _p1, Point _p2, Point _p3, GLfloat _r, GLfloat _g, GLfloat _b)
            : p1(_p1), p2(_p2), p3(_p3) {
        color[0] = _r;
        color[1] = _g;
        color[2] = _b;
    }

};

std::vector<Triangle> curr;
std::vector<std::vector<Triangle>> states;

void init() {
    glClearColor(0.976f, 0.651f, 0.012f, 1.0f);
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    std::cout << "state_size: " << states.size() << ' ';
    std::cout << std::endl;
    std::cout << "curr_index: " << currentStateIndex << " ";

    if (states.empty() || currentStateIndex >= states.size()) {
        glColor3fv(currentColor);
        for (const auto& triangle : curr) {
            glBegin(GL_TRIANGLES);
            glVertex2f(triangle.p1.x, triangle.p1.y);
            glVertex2f(triangle.p2.x, triangle.p2.y);
            glVertex2f(triangle.p3.x, triangle.p3.y);
            glEnd();
        }
    }
    else {
        std::vector<Triangle> state = states[currentStateIndex];
        for (const auto& triangle : state) {
            glBegin(GL_TRIANGLES);
            glColor3fv(triangle.color);
            glVertex2f(triangle.p1.x, triangle.p1.y);
            glVertex2f(triangle.p2.x, triangle.p2.y);
            glVertex2f(triangle.p3.x, triangle.p3.y);
            glEnd();
        }
    }
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        float xPos = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
        float yPos = -(float)y / glutGet(GLUT_WINDOW_HEIGHT) * 2 + 1;

        static int clickCount = 0;
        static Point clickPoints[3];

        if (clickCount < 3) {
            clickPoints[clickCount] = Point(xPos, yPos);
            clickCount++;
        }

        if (clickCount == 3) {
            curr.emplace_back(clickPoints[0], clickPoints[1], clickPoints[2], currentColor[0],
                              currentColor[1], currentColor[2]);
            clickCount = 0;
            glutPostRedisplay();
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        float xPos = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
        float yPos = -(float)y / glutGet(GLUT_WINDOW_HEIGHT) * 2 + 1;

        std::vector<Point> vertices;
        for (const auto& tri : curr) {
            vertices.push_back(tri.p1);
            vertices.push_back(tri.p2);
            vertices.push_back(tri.p3);
        }
        for (size_t i = 0; i < vertices.size(); i++) {
            float dx = xPos - vertices[i].x;
            float dy = yPos - vertices[i].y;
            if (sqrt(dx * dx + dy * dy) < 0.1) {
                draggedVertexIndex = i;
                break;
            }
        }
    }
    else if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
        draggedVertexIndex = -1;
    }
}

void motion(int x, int y) {
    if (draggedVertexIndex != -1) {
        float xPos = (float)x / glutGet(GLUT_WINDOW_WIDTH) * 2 - 1;
        float yPos = -(float)y / glutGet(GLUT_WINDOW_HEIGHT) * 2 + 1;

        size_t triangleIndex = draggedVertexIndex / 3;
        size_t vertexIndex = draggedVertexIndex % 3;
        Triangle& triangle = curr[triangleIndex];
        Point* vertexToUpdate = nullptr;
        if (vertexIndex == 0) {
            vertexToUpdate = &triangle.p1;
        } else if (vertexIndex == 1) {
            vertexToUpdate = &triangle.p2;
        } else {
            vertexToUpdate = &triangle.p3;
        }
        vertexToUpdate->x = xPos;
        vertexToUpdate->y = yPos;
        glutPostRedisplay();
    }
}

void menu(int value) {
    std::vector<Triangle> copyOfCurr;
    switch (value) {
        case 1:
            curr.clear();
            currentColor[0] = 1.0f;
            currentColor[1] = 1.0f;
            currentColor[2] = 1.0f;
            glutPostRedisplay();
            break;
        case 2:
            std::cout << "Colors: ";
            for (const auto& triangle : curr) {
                std::cout << triangle.color[0] << ", " << triangle.color[1] << ", " << triangle.color[2] << "; ";
            }
            std::cout << "curr_c: "<< currentColor[0];
            for (const auto& triangle : curr) {
                copyOfCurr.emplace_back(triangle.p1, triangle.p2,
                                        triangle.p3, currentColor[0], currentColor[1], currentColor[2]);
            }
            states.push_back(copyOfCurr);
            currentStateIndex++;
            glutPostRedisplay();
            break;
        case 3:
            if (currentStateIndex > 0){
                currentStateIndex--;
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glutPostRedisplay();
            }
            break;
        case 4:
            if (currentStateIndex < states.size() - 1)
            {
                currentStateIndex++;
                glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
                glutPostRedisplay();
            }
            else if(currentStateIndex + 1 == states.size()){
                currentStateIndex++;
                glClearColor(0.976f, 0.651f, 0.012f, 1.0f);
                glutPostRedisplay();
            }
            break;
        case 5:
            if (!curr.empty()) {
                curr.pop_back();
                glutPostRedisplay();
            }
            break;
        case 6:
            if (states.size() > 0) {
                if (currentStateIndex >= states.size() - 1) {
                    currentStateIndex = states.size() - 1;
                }
                states.pop_back();
                glutPostRedisplay();
            }
            break;
        case 7:
            currentColor[0] = static_cast<float>(rand()) / RAND_MAX;
            currentColor[1] = static_cast<float>(rand()) / RAND_MAX;
            currentColor[2] = static_cast<float>(rand()) / RAND_MAX;
            glutPostRedisplay();
            break;
        case 8:
            exit(0);
    }
}

void initMenu() {
    glutCreateMenu(menu);
    glutAddMenuEntry("Clear All", 1);
    glutAddMenuEntry("Save", 2);
    glutAddMenuEntry("Previous set", 3);
    glutAddMenuEntry("Next set", 4);
    glutAddMenuEntry("Ctrl + Z", 5);
    glutAddMenuEntry("Delete last set", 6);
    glutAddMenuEntry("Change color", 7);
    glutAddMenuEntry("Quit", 8);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void keyboard(unsigned char key, int x, int y) {
    if (key == 'c' || key == 'C') {
        curr.clear();
        currentColor[0] = 1.0f;
        currentColor[1] = 1.0f;
        currentColor[2] = 1.0f;
        glutPostRedisplay();
    }
    else if (key == 's' || key == 'S') {
        std::vector<Triangle> copyOfCurr;
        std::cout << "Colors: ";
        for (const auto& triangle : curr) {
            std::cout << triangle.color[0] << ", " << triangle.color[1] << ", " << triangle.color[2] << "; ";
        }
        std::cout << "curr_c: "<< currentColor[0];
        for (const auto& triangle : curr) {
            copyOfCurr.emplace_back(triangle.p1, triangle.p2,
                                    triangle.p3, currentColor[0], currentColor[1], currentColor[2]);
        }
        states.push_back(copyOfCurr);
        currentStateIndex++;
        glutPostRedisplay();
    }
    else if (key == 'a' || key == 'A') {
        if (currentStateIndex > 0){
            currentStateIndex--;
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glutPostRedisplay();
        }
    }
    else if (key == 'd' || key == 'D') {
        if (currentStateIndex < states.size() - 1)
        {
            currentStateIndex++;
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glutPostRedisplay();
        }
        else if(currentStateIndex+1 == states.size()){
            currentStateIndex++;
            glClearColor(0.976f, 0.651f, 0.012f, 1.0f);
            glutPostRedisplay();
        }

    }
    else if (key == 'q' || key == 'Q') {
        if (!curr.empty()) {
            curr.pop_back();
            glutPostRedisplay();
        }
    }
    else if (key == 'e' || key == 'E') {
        if (states.size() > 0) {
            if (currentStateIndex >= states.size() - 1) {
                currentStateIndex = states.size() - 1;
            }
            states.pop_back();
            glutPostRedisplay();
        }
    }
    else if (key == ' ') {
        currentColor[0] = static_cast<float>(rand()) / RAND_MAX;
        currentColor[1] = static_cast<float>(rand()) / RAND_MAX;
        currentColor[2] = static_cast<float>(rand()) / RAND_MAX;
        glutPostRedisplay();
    }
    if (!menu_active && (key == 'u' || key == 'U')) {
        initMenu();
        menu_active = true;
    } else if (menu_active && (key == 'u' || key == 'U')) {
        glutDetachMenu(GLUT_RIGHT_BUTTON);
        menu_active = false;
    }
}

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1, 1, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("V5 TRIANGLES");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

