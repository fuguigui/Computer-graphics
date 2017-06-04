#include<gl/glut.h>
#include<windows.h>
//在视见空间的中心位置创建一个固态球体，让一个可移动的聚光灯照在球体上，而且可以改变法线的“平滑”
//2017年6月4日，本程序可以执行
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;


#define MODE_FLAT 1
#define MODE_SMOOTH 2
#define MODE_VERYLOW 3
#define MODE_MEDIUM 4
#define MODE_VERYHIGH 5



//灯光的值和参数
GLfloat lightPos[] = { 0.0f,0.0f,75.0f,1.0f };
GLfloat specular[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat specref[] = { 1.0f,1.0f,1.0f,1.0f };
GLfloat ambientLight[] = { 0.5f,0.5f,0.5f,1.0f };
GLfloat spotDir[] = { 0.0f,0.0f,-1.0f };

int iShade = MODE_FLAT;
int iTess = MODE_VERYLOW;

void ProcessMenu(int value) {
	switch (value)
	{
	case 1:
		iShade = MODE_FLAT; break;
	case 2:
		iShade = MODE_SMOOTH; break;
	case 3: 
		iTess = MODE_VERYLOW; break;
	case 4:
		iTess = MODE_MEDIUM; break;
	case 5:
	default:
		iTess = MODE_VERYHIGH;
		break;
	}

	glutPostRedisplay();
}

void SetupRC() {
	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);//不显示背面

	//开启灯光
	glEnable(GL_LIGHTING);
	//设置灯光0
//	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, ambientLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	//使定了位的光源形成聚光灯
	glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, 60.0f);//spot_cutoff：指定聚光灯光线发射的圆锥形辐射角度
	glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, 100.0f);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	glMateriali(GL_FRONT, GL_SHININESS, 128);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void SpecialKeys(int key, int x, int y) {
	if (key == GLUT_KEY_UP)
		xRot -= 5.0f;
	if (key == GLUT_KEY_DOWN)
		xRot += 5.0f;
	if (key == GLUT_KEY_LEFT)
		yRot -= 5.0f;
	if (key == GLUT_KEY_RIGHT)
		yRot += 5.0f;
	if (key > 356.0f)
	{
		xRot = 0.0f;
		yRot = 0.0f;
	}
	if (key < -1.0f){
		xRot = 355.0f;
		yRot = 355.0f;
	}
	glutPostRedisplay();
}

void RenderScene() {
	if (iShade == MODE_FLAT) glShadeModel(GL_FLAT);
	else glShadeModel(GL_SMOOTH);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//放置光源
	glPushMatrix();
	glRotatef(yRot, 0.0f, 1.0f, 0.0f);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spotDir);

	glColor3ub(255, 0, 0);
	glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
	glutSolidCone(4.0f, 6.0f, 15, 15);
	glPushAttrib(GL_LIGHTING_BIT);

	//关灯，画一个黄色球体
	glDisable(GL_LIGHTING);
	glColor3ub(255, 255, 0);
	glutSolidSphere(3.0f, 15, 15);

	glPopAttrib();
	glPopMatrix();

	glColor3ub(138, 138, 138);
	if (iTess == MODE_VERYLOW)
	{
		glutSolidSphere(24.0f, 100, 100);
		glutSolidCone(8.0f, 90.0f, 100, 100);
	}
	else if (iTess == MODE_MEDIUM)
	{
		glutSolidSphere(30.0f, 15, 15);
	}
	else glutSolidSphere(30.0f, 50, 50);

	glutSwapBuffers();
}

void ChangeSize(GLsizei w, GLsizei h) {
	GLfloat fAspect;

	if (h == 0) h = 1;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	fAspect = (GLfloat)w / (GLfloat)h;
	gluPerspective(35.0f, fAspect, 1.0f, 500.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -250.0f);
}

void main() {
	int nMenu;
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("spot");
	glutDisplayFunc(RenderScene);
	glutReshapeFunc(ChangeSize);

	nMenu = glutCreateMenu(ProcessMenu);
	glutAddMenuEntry("Flat Shading", 1);
	glutAddMenuEntry("Smooth Shading", 2);
	glutAddMenuEntry("VL Tess", 3);
	glutAddMenuEntry("MD Tess", 4);
	glutAddMenuEntry("VH Tess", 5);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);

	SetupRC();
	glutMainLoop();
}