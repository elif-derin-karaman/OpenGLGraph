#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <GL/glut.h>

#define SAMPLE_QUANTITY 1000
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

struct Point {
	float x;
	float y;
	Point(float x_in, float(*function_ptr)(float)) {
        x = x_in;
		y = function_ptr(x_in); 
	}
};

float function(float x_in) {
	return exp(-1.0*pow(x_in - 10, 2.0));
}

void init(){
    glClear( GL_COLOR_BUFFER_BIT );
    glMatrixMode( GL_PROJECTION );
    glOrtho( 0, 99.0, -30, 30, -1, 1 );
    
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    glBegin(GL_LINE_STRIP);
    std::vector<Point> samplePoints; 
	for (int x = 0; x < SAMPLE_QUANTITY; x++) {
        samplePoints.emplace_back(x, &function);
	}
    for (size_t i = 0; i < samplePoints.size(); ++i) {
        glVertex2f(samplePoints[i].x, samplePoints[i].y * 20);
    }  
    glEnd();
    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowPosition(10,10);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow("graph");
    init();
    glutDisplayFunc(display);
    glutMainLoop();

    return 0;
}
