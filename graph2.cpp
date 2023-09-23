#include <vector>
#include <array>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#define PLOT_Y_MARGIN 10.0
#define SAMPLE_QUANTITY 1000
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define GRID_X_INTERVAL 1
#define GRID_Y_INTERVAL 1
#define X_LENGTH 30
#define X_SCALE 2.0
#define Y_SCALE 2.0

struct Point {
    float x;
    float y;
    Point() : x(0.0f), y(0.0f) {}
    Point(float x_in, float(*function_in)(const float)) {
        x = x_in;
        y = function_in(((x_in*X_LENGTH/SAMPLE_QUANTITY) - X_LENGTH/2)/X_SCALE)*Y_SCALE; 
    }
};

class Plotter {
public:
    Plotter(int argc, char **argv, float(*function_in)(const float));
private:
    static std::vector<Point> samplePoints;
    static float getMinY();
    static float getMaxY();
    static void drawLine(float x1, float y1,
                         float x2, float y2,
                         float r,  float g,
                         float b,  float thickness);
    static void drawPlot();
    static void drawGrid();
    static void drawNumerals();
    static void drawNumber(float x, float y, float number, float r, float g, float b);
    static void updatePlot();
    static void drawLoopOpenGL();
    static void initializeOpenGL();
};

std::vector<Point> Plotter::samplePoints;

Plotter::Plotter(int argc, char **argv, float(*function_in)(const float)) {
    /* Initialize the sampled points. */
    samplePoints.resize(SAMPLE_QUANTITY);
    for (int x = 0; x < SAMPLE_QUANTITY; x++) {
        samplePoints[x] = Point(x, function_in);
    }
    /* Initialize new window. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(10, 10);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Graph");

    /* Setup OpenGL parameters*/
    glClear( GL_COLOR_BUFFER_BIT );
    glMatrixMode( GL_PROJECTION );
    glOrtho(0, SAMPLE_QUANTITY, getMinY() - PLOT_Y_MARGIN, getMaxY() + PLOT_Y_MARGIN, -1, 1);
    //glOrtho(0, SAMPLE_QUANTITY, -30, 30, -1, 1);

    /* Input the OpenGL render function and run the render loop.*/
    glutDisplayFunc(Plotter::drawLoopOpenGL);
    glutMainLoop();
}

float Plotter::getMaxY() {
    float maximum = samplePoints[0].y;
    for (std::vector<Point>::const_iterator it = Plotter::samplePoints.begin(); it < Plotter::samplePoints.end(); it++) {
        maximum = it->y > maximum ? it->y : maximum;
    }
    return maximum;
}

float Plotter::getMinY() {
    float minimum = samplePoints[0].y;
    for (std::vector<Point>::const_iterator it = Plotter::samplePoints.begin(); it < Plotter::samplePoints.end(); it++) {
        minimum = it->y > minimum ? it->y : minimum;
    }
    return minimum;
}

void Plotter::drawLine(float x1, float y1,
                       float x2, float y2,
                       float r,  float g,
                       float b,  float thickness) {
    glLineWidth(thickness);
    glColor3f(r, g, b);
    glBegin(GL_LINE_STRIP);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glEnd();
}

void Plotter::drawPlot() {
    glLineWidth(3);
    glColor3f(0.9, 0.1, 0.1);
    glBegin(GL_LINE_STRIP);
    for (std::vector<Point>::iterator it = samplePoints.begin(); it < samplePoints.end(); it++) {
        glVertex2f(it->x, it->y);
    }
    glEnd();
}

void Plotter::drawGrid() {
    float left = 0.0;
    float right = SAMPLE_QUANTITY;
    float top = getMaxY() + PLOT_Y_MARGIN;
    float bot = getMinY() - PLOT_Y_MARGIN;

    /* Draw sub-axes for X*/
    float x_pen = 0.0,
          y_pen = 0.0;

    while (x_pen < right) {
        drawLine(0 + x_pen, bot, 0 + x_pen, top, 0.2, 0.2, 0.2, 1);
        drawLine(0 - x_pen, bot, 0 - x_pen, top, 0.2, 0.2, 0.2, 1);
        x_pen += GRID_X_INTERVAL*SAMPLE_QUANTITY/X_LENGTH;
        
    }
    while (y_pen < top) {
        drawLine(left, 0 + y_pen, right, 0 + y_pen, 0.2, 0.2, 0.2, 1);
        drawLine(left, 0 - y_pen, right, 0 - y_pen, 0.2, 0.2, 0.2, 1);
        y_pen += GRID_Y_INTERVAL;
    }

    /* Draw the X and Y axes*/
    drawLine(SAMPLE_QUANTITY/2, top,
             SAMPLE_QUANTITY/2, bot, 0.3, 0.3, 0.3, 2);
    drawLine(0, 0, right, 0, 0.3, 0.3, 0.3, 2);

}

void Plotter::drawNumerals() {
    float left = 0.0;
    float right = SAMPLE_QUANTITY;
    float top = getMaxY() + PLOT_Y_MARGIN;
    float bot = getMinY() - PLOT_Y_MARGIN;

    /* Draw the numeral indicators. */
    float x_pen = 0.0,
          y_pen = 0.0;
    int x_num = 0;

    while (x_pen < right) {
        drawNumber(SAMPLE_QUANTITY/2 + x_pen, 0, GRID_X_INTERVAL*x_num, 1.0, 0.8, 0.5);
        drawNumber(SAMPLE_QUANTITY/2 - x_pen, 0, GRID_X_INTERVAL*x_num, 1.0, 0.8, 0.5);
        x_pen += GRID_X_INTERVAL*SAMPLE_QUANTITY/X_LENGTH;
        x_num++;
        
    }
    while (y_pen < top) {
        drawNumber(0, 0 + y_pen, y_pen, 1.0, 0.8, 0.5);
        drawNumber(0, 0 - y_pen, y_pen, 1.0, 0.8, 0.5);
        y_pen += GRID_Y_INTERVAL;
    }

}

void Plotter::drawNumber(float x, float y, float number, float r, float g, float b) {
    std::string numString = std::to_string(number);
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    if (number < 0.0) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, '-');
    }
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, numString[0]);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, numString[1]);
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, numString[2]);
}

void Plotter::drawLoopOpenGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();
    drawPlot();
    drawNumerals();
    glFlush();
}

float arbitraryFunction(const float x_in) {
    if (x_in > 1.8 | x_in < -1.8) {
        return 0.0;
    }

    float y_out = pow(pow(x_in, 2), 1.0/3.0) + 0.9*pow(3.3 - pow(x_in, 2), 1.0/2.0)*sin(10*M_PI*x_in);
    return y_out;
}

int main(int argc, char **argv) {
    Plotter functionDisplayer(argc, argv, &arbitraryFunction);
    return 0;
}