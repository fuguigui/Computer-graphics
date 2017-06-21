#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>

/*set initial size of the display window*/
GLsizei winWidth = 800, winHeight = 600;

/*set size of world-coordinate clipping window.*/
GLfloat xwcMin = -10.0, xwcMax = 300.0;
GLfloat ywcMin = -100.0, ywcMax = 100.0;

class wcPt3D {
public:
	GLfloat x, y, z;
};

void init(void) {
	/*Set color of display window to white */
	glClearColor(1.0, 1.0, 1.0, 0.0);
}
void plotPoint(wcPt3D bezCurvePt) {
	glBegin(GL_POINTS);
	glVertex2f(bezCurvePt.x, bezCurvePt.y);
	glEnd();
}

void computeDecaBexPt(GLfloat u, wcPt3D * bezPt, GLint nCtrlPts,
	wcPt3D * ctrlPts) {
	GLint i,k, n = nCtrlPts - 1;
	wcPt3D *temp = new wcPt3D[n];

	/*initialize the 1-order points*/
	for (k = 0; k < n; k++) {
		temp[k].x = (1 - u)*ctrlPts[k].x + u*ctrlPts[k+1].x;
		temp[k].y = (1 - u)*ctrlPts[k].y + u*ctrlPts[k+1].y;
		temp[k].z = (1 - u)*ctrlPts[k].z + u*ctrlPts[k+1].z;
	}
	/*compute n-2 iterations, get the value of p(u)*/
	for (i = 2; i <= n; i++) {
		for (k = 0; k <= n-i; k++) {
			temp[k].x = (1 - u)*temp[k].x + u*temp[k + 1].x;
			temp[k].y = (1 - u)*temp[k].y + u*temp[k + 1].y;
			temp[k].z = (1 - u)*temp[k].z + u*temp[k + 1].z;
		}
	}
	bezPt->x = temp[0].x;
	bezPt->y = temp[0].y;
	bezPt->z = temp[0].z;

	delete[] temp;
}

void deca_bezier(wcPt3D * ctrlPts, GLint nCtrlPts, GLint nBezCurvePts) {
	wcPt3D bezCurvePt;
	GLfloat u;
	GLint  k;


	for (k = 0; k <= nBezCurvePts; k++) {
		u = GLfloat(k) / GLfloat(nBezCurvePts);
		computeDecaBexPt(u, &bezCurvePt, nCtrlPts, ctrlPts);
		plotPoint(bezCurvePt);
	}
}

void displayFcn(void) {
	/*set example number of control points and number of curve
	*positions to be poltted along the Bezier curve.
	*/
	GLint nCtrlPts = 7, nBezCurvePts = 2000;

	wcPt3D ctrlPts[7] = { { 0.0,0.0,0.0 },{ 80.0,50.0,0.0 },
	{ 200.0,-60.0,0.0 },{ 300.0,-70.0,0.0 },{ 400.0,100.0,0.0 },{100.0,200.0,0.0},{50.0,-80.0,0.0} };
	/*5 control points:
	group1:
	{ { 0.0,0.0,0.0 },{ 0.0,0.0,0.0 },
	{ 0.0,0.0,0.0 },{ 0.0,0.0,0.0 },{ 0.0,0.0,0.0 } };
	group2:
	{ { 0.0,0.0,0.0 },{ 20.0,0.0,0.0 },
	{ 40.0,0.0,0.0 },{ 60.0,0.0,0.0 },{ 80.0,0.0,0.0 } };
	group3:
	{ { 0.0,0.0,0.0 },{ 20.0,100.0,0.0 },
	{ 40.0,10.0,0.0 },{ 60.0,-80.0,0.0 },{ 80.0,-10.0,0.0 } };
	group4:
	{ { 0.0,0.0,0.0 },{ 80.0,50.0,0.0 },
	{ 40.0,-60.0,0.0 },{ 20.0,-70.0,0.0 },{ 0.0,100.0,0.0 } };
	group5:
	{ { 0.0,0.0,0.0 },{ 80.0,50.0,0.0 },
	{ 40.0,-60.0,0.0 },{ 20.0,-70.0,0.0 },{ 0.0,50.0,0.0 } };

	*/
	glClear(GL_COLOR_BUFFER_BIT);
	glPointSize(4);
	glColor3f(0.0, 1.0, 1.0);

	deca_bezier(ctrlPts, nCtrlPts, nBezCurvePts);
	glFlush();
}

void winReshapeFcn(GLint newWidth, GLint newHeight) {
	/*maintain an aspect ratio of 1.0*/
	glViewport(0, 0, newHeight, newHeight);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);

	glClear(GL_COLOR_BUFFER_BIT);
}

void main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(20, 0);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("DeCasteljau Curve");

	init();
	glutDisplayFunc(displayFcn);
	glutReshapeFunc(winReshapeFcn);

	glutMainLoop();
}