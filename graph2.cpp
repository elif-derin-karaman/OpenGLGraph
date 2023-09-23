#include <vector>
#include <array>
#include <cmath>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>

#define SAMPLE_QUANTITY 1000
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define X_LENGTH 10.0
#define X_SCALE 2
#define Y_SCALE 1.0

struct Point {
    float x;
    float y;
    Point() : x(0.0f), y(0.0f) {}
    Point(float x_in, float(*function_in)(const float)) {
        x = x_in;
        y = function_in(((x_in*X_LENGTH/SAMPLE_QUANTITY) - X_LENGTH/2)*X_SCALE)*Y_SCALE; 
    }
};

class Plotter {
public:
    Plotter(int argc, char **argv, float(*function_in)(const float));
private:
    static std::vector<Point> samplePoints;
    std::array<float, 2> getYLimits(const std::vector<Point> &points_in);
    static void initializeOpenGL();
    static void displayOpenGL();
};

std::vector<Point> Plotter::samplePoints;

std::array<float, 2> Plotter::getYLimits(const std::vector<Point> &points_in) {
    float maximum = 0.0,
          minimum = 0.0;
    for (std::vector<Point>::const_iterator it = points_in.begin(); it < points_in.end(); it++) {
        maximum = it->y > maximum ? it->y : maximum;
        minimum = it->y < minimum ? it->y : minimum;
    }
    return std::array<float, 2>({maximum, minimum});
}

Plotter::Plotter(int argc, char **argv, float(*function_in)(const float)) {
    /* Initialize the sampled points. */
    samplePoints.resize(SAMPLE_QUANTITY);
    for (int x = 0; x < SAMPLE_QUANTITY; x++) {
        samplePoints[x] = Point(x, function_in);
	}
    std::array<float, 2> y_lims = getYLimits(samplePoints);

    /* Initialize new window. */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(10, 10);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("Graph");

    /* Setup OpenGL parameters*/
    glClear( GL_COLOR_BUFFER_BIT );
    glMatrixMode( GL_PROJECTION );
    glOrtho(0, SAMPLE_QUANTITY, y_lims[1] - 10, y_lims[0] + 10, -1, 1);
    //glOrtho(0, SAMPLE_QUANTITY, -30, 30, -1, 1);

    /* Input the OpenGL render function and run the render loop.*/
    glutDisplayFunc(Plotter::displayOpenGL);
    glutMainLoop();
}

void Plotter::displayOpenGL() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINE_STRIP);
        
    for (std::vector<Point>::iterator it = samplePoints.begin(); it < samplePoints.end(); it++) {
        glVertex2f(it->x, it->y);
    }

    glEnd();
    glFlush();
}

float arbitraryFunction(const float x_in) {
    return exp(-1.0*pow(x_in, 2.0));
}

int main(int argc, char **argv) {
    Plotter functionDisplayer(argc, argv, &arbitraryFunction);
    return 0;
}