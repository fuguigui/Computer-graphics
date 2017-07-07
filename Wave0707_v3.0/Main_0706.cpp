#define _CRT_SECURE_NO_WARNINGS        

#include <stdio.h>        
#include <string.h>        
#include<time.h>      
#include <stdlib.h>   
#include"Tools.h"        
#include"fluid_0.h"  
#include "Fluid_0702.h"


#define x_start 30.0
#define y_start 30.0
#define z_start 0.0
#define x_dist 3.0
#define y_dist 3.0

static bool drawNormals = false;
static bool drawWireFrame = false;
static bool lighting = true;
static bool drawFoam = true;
static bool isTexture = true;//������
static bool drawTiles = false;
static bool paused = false;
static bool realTime = true;  // when false, animate 0.1 seconds per frame regardless of FPS
static bool solveNavierStokes = false;
static bool fullScreen = false;
static bool isLine = false;//������line��fill����ʾģʽ֮�����л�
static bool isDiffuse = true;//����������
static bool isAmbient = true;//����������
static bool isSpecular = false;//�����۹��

							   //���Ƶƹ�Ŀ���
GLboolean isLightOn = true;

static bool choppyWaves = true;
static bool fftNormals = false;
static bool displayForce = false;

//��������
GLuint texture[13];
//����        
float whRatio;
int wHeight = 0;
int wWidth = 0;
//����  
Fluid_0 *f0;
Fluid_1 *f1;
int fluidMode = 0;//ѡ��Ĳ���ģʽ��Ĭ��Ϊ0

//�ӵ�        
float center[] = { 20,10,0 };
float eye[] = { 20,-10,8 };

int mox, moy, mnx, mny, mousedown; // Mouse original X and Y and new X and Y, and whether the mouse is pressed
float mouseSpeed;
float x, y, z, rx, ry; // X, Y, and Z position, and rotation around X and Y axes

			  //������λ��  
GLfloat light_x = 0.0f;
GLfloat light_y = 0.0f;
GLfloat light_z = 0.0f;

//�۹�Ʒ���  
GLfloat dir_x = 0.0f;
GLfloat dir_y = 0.0f;
GLfloat dir_z = 0.0f;

GLfloat light_pos[4] = { 40.0, 0.0, 10.0, 1 };//�������λ��
GLfloat light_pos1[4] = { 0.0f, 5.0f, 0.0f, 1.0f }; //����۹��λ��  								
GLfloat light_Dir[3] = { 0.0f + dir_x ,-1.0f + dir_y ,0.0f + dir_z }; ///���սǶ�

GLfloat color[] = { 1.0, 1.0, 1.0, 1.0 }; // ������ɫ  


GLfloat spotangle = 5.0f; //�Ƕ�  


float wave_para[6][6] = {
	{ 1.6,	0.12,	0.9,	0.6,	0.0,	0.0 },
	{ 1.3,	0.1,	1.14,	0.9,	0.0,	0.0 },
	{ 0.2,	0.01,	0.8,	0.8,	0.0,	0.0 },
	{ 0.18,	0.008,	1.05,	1.0,	0.0,	0.0 },
	{ 0.23,	0.005,	1.15,	0.9,	0.0,	0.0 },
	{ 0.12,	0.003,	0.97,	1.4,	0.0,	0.0 }
};//���˵Ĳ���


void init()
{
	srand(unsigned(time(NULL)));


	x = 0; y = 0; z = 0; rx = 20; ry = -20;
	mouseSpeed = 0.5;
	mousedown = 0;
	//Ԥ��ˮ��ģʽ
	switch (fluidMode)
	{
	case 0:
		//��������
		if(isTexture){

			glGenTextures(1, texture);
			loadTex(0, "water.bmp", texture);
			f0 = new Fluid_0(100, 100, 2, 1, 0.8, 0.2,texture[0]);
			break;
		}
		else
		f0 = new Fluid_0(200, 100, 2, 0.5, 0.5, 1); break;
	case 1:
			f1 = new Fluid_1(100, 80, z_start);
			f1->initWave(wave_para, x_start, y_start, x_dist, y_dist);
			break;
	default:
		break;
	}
}
void drawScene()
{
	GLfloat mat_specular[] = { 0.3f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_diffuse0[] = { 0.65f, 0.45f, 0.7f, 1.0f };
	GLfloat mat_diffuse3[] = { 0.5f, 1.0f, 0.9f };

	glPushMatrix();



	// ������ɫ׷��      
	glEnable(GL_COLOR_MATERIAL);
	// ���ö��������Ļ������ɢ���������ԣ�׷��glColor      
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);// ���ö��������ľ��淴������  
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 80);// ָ������ָ��  
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse3); //���ö������������������
	static int count = 0;											 
	switch (fluidMode)
	{
	case 0:
		count++;
		if (count > 40) {
			count = 0;
			f0->Evaluate();
		}
		f0->draw();
		break;
	case 1:
		count++;
		if (count > 100) {
			count = 0;
			f1->Evaluate();
		}
		f1->draw();
		break;

	default:
		break;
	}
	glPopMatrix();

}

void inputHandler() {
	if (mousedown == 1)
	{
		ry += (mnx - mox) * mouseSpeed;
		rx += (mny - moy) * mouseSpeed;
		mox = mnx;
		moy = mny;
	}
	if (rx > 90) rx = 90; if (rx < -90) rx = -90;
}


void redraw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//�����ɫ����Ȼ���          
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   //��ʼ������Ϊ��λ����    



	glTranslatef(x, y, z);
	glRotatef(rx, 1, 0, 0);
	glRotatef(ry, 0, 1, 0);

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 0,1);                // ������0��0��0)��Y������        
	if(isLine)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);//������Ȳ���     
	glFrontFace(GL_CCW);//��ʱ��������
	glEnable(GL_CULL_FACE);//����������

	glEnable(GL_LIGHTING);

	GLfloat specularLight[] = { 0.3f, 0.68f, 0.6f, 1.0f }; //����ƹ�0����ɫ 
	GLfloat diffuseLight[] = { 0.2f,0.5f,0.6f,1.0f };
	GLfloat ambientLight[] = { 0.2f,0.3f,0.3f,1.0f };
	GLfloat empty[] = { 0.0f,0.0f,0.0f,0.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos); //���õ�0�Ź�Դ�Ĺ���λ��    
	if(isSpecular)
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight); //���þ��淴�������ɫ  
	else glLightfv(GL_LIGHT0, GL_SPECULAR, empty);
	if(isDiffuse)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);                     //���������ɷ�  
	else glLightfv(GL_LIGHT0, GL_DIFFUSE, empty);
	if(isAmbient)
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);   //���õ�0�Ź�Դ��η����Ĺ�����ɫ����������ɫ�� 
	else glLightfv(GL_LIGHT0, GL_AMBIENT, empty);

	glPopMatrix();

	if (isLightOn)
		glEnable(GL_LIGHT0); 

	glShadeModel(GL_SMOOTH);
	// ���ù��ռ���      

	// ���ù���ģ�ͣ���ambientLight��ָ����RGBAǿ��ֵӦ�õ�������      
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	//����ģ�壬��������ʱ�Ѿ��淶Χ��ģ��ֵ����1&0xff  
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//������ʱ�����������ֵΪֻ���������������ֵ���µ�Ӱ������ȥ  
	glDepthMask(GL_FALSE);


	drawScene();//���Ƴ���  

	glDepthMask(GL_TRUE);

	//��������ģ����ԣ������ھ���ģ��ֵ���򲻻���ʾ  
	glStencilFunc(GL_EQUAL, 1, 0xff);
	glStencilFunc(GL_KEEP, GL_KEEP, GL_KEEP);
	glDisable(GL_STENCIL_TEST);

	inputHandler();

	glutSwapBuffers();//����������        
}

void updateView(int height, int width)
{
	glViewport(-20, -20, width, height);
	glMatrixMode(GL_PROJECTION);//���þ���ģʽΪͶӰ         
	glLoadIdentity();   //��ʼ������Ϊ��λ����            
	whRatio = (GLfloat)width / (GLfloat)height;  //������ʾ����       
												 //	gluPerspective(80.0f, whRatio, -1.5f, 20.0f); //͸��ͶӰ          
												 //	glFrustum(0, 5, 0, 10, 5,100);        
	glOrtho(0, 40, -10, 80, 5, 100);
	glMatrixMode(GL_MODELVIEW);  //���þ���ģʽΪģ��      
}


void reshape(int width, int height)
{
	if (height == 0)      //����߶�Ϊ0            
	{
		height = 1;   //�ø߶�Ϊ1��������ַ�ĸΪ0������            
	}

	wHeight = height;
	wWidth = width;

	updateView(wHeight, wWidth); //�����ӽ�            
}
void keyDownHandler(unsigned char c, int mx, int my) // Handles keyboard input
{
	switch (c)
	{
	case 'a'://�����ƶ��ӽ�
		mnx += 10;
		mousedown = 1;
		break;
	case 'd':
		mnx -= 10;
		mousedown = 1;
		break;
	case 'w':
		mny += 10;
		mousedown = 1;
		break;
	case 's':
		mny -= 10;
		mousedown = 1;
		break;
	case 'l': {//���Ƶƹ�Ŀ���
		isLightOn = !isLightOn;
		break;
	}
	case 'i': {//����������  
		light_y = light_y + 0.2f;
		break;
	}
	case 'k': {//����������  
		light_y = light_y - 0.2f;
		break;
	}
	case 'n': {//������ǰ��  
		light_z = light_z + 0.2f;
		break;
	}
	case 'm': {//���������  
		light_z = light_z - 0.2f;
		break;
	}
	case 'f': {//�۹������  
		dir_x = dir_x - 0.05f;
		break;
	}
	case 'h': {//�۹������  
		dir_x = dir_x + 0.05f;
		break;
	}
	case 't': {//�۹������  
		dir_y = dir_y - 0.05f;
		break;
	}
	case 'g': {//�۹������  
		dir_y = dir_y + 0.05f;
		break;
	}
	case 'v': {//�۹�ƺ���  
		dir_z = dir_z - 0.05f;
		break;
	}
	case 'b': {//�۹��ǰ��  
		dir_z = dir_z + 0.05f;
		break;
	}
	case 'x': { //�۹�ƽǶȱ��  
		if (spotangle <= 89.0f) {
			spotangle = spotangle + 0.2f;
		}
		break;
	}
	case 'y': { //�۹�ƽǶȱ�С  
		if (spotangle >= 1.0f) {
			spotangle = spotangle - 0.2f;
		}
		break;
	}
	default:
		break;
	}

	glutPostRedisplay();
}

void mouseMoveHandler(int x, int y)
{
	mnx = x;
	mny = y;
	glutPostRedisplay();
}
void moveHandler(GLint key, GLint x, GLint y) {
	switch (key)
	{
	case GLUT_KEY_UP:
		eye[1] += 10;
		center[1] += 10;
		break;
	case GLUT_KEY_LEFT:
		eye[0] -= 10;
		center[0] -= 10;
		break;
	case  GLUT_KEY_DOWN:
		eye[1] -= 10;
		center[1] -= 10;
		break;
	case GLUT_KEY_RIGHT:
		eye[0] += 10;
		center[0] += 10;
		break;
	case GLUT_KEY_F1:

	default:
		break;
	}
	//glutPostRedisplay();
	redraw();
}
void mouseDownHandler(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{
			mnx = mox = x;
			mny = moy = y;
			mousedown = 1;
		}
		else if (state == GLUT_UP)
		{
			mousedown = 0;
		}
	}
	glutPostRedisplay();
}
void idle()
{
	if (fluidMode == 1) {
		f1->Wave_Value.time += 1.0f;
		f1->Wave_Value.time += 1.0f;
	}
	inputHandler();
	glutPostRedisplay();
}



void togglePaused() {
	paused = !paused;
	if (paused)
		glutIdleFunc(0);
	else
		glutIdleFunc(idle);
}
void toggleAddSpeed() {
	f0->ChangeSpeed(f0->v + 0.05f);
}
void toggleReduceSpeed() {

	f0->ChangeSpeed(f0->v - 0.05f);
}
void toggleAddResist() {
	f0->ChangeResist(f0->r + 0.05f);
}
void toggleReduceResist() {
	f0->ChangeResist(f0->r - 0.05f);
}
void toggleChangeDisplayMode() {
	isLine = !isLine;
}
void toggleAmbient() {
	isAmbient = !isAmbient;
}
void toggleDiffuse() {
	isDiffuse = !isDiffuse;
}
void toggleSpecular() {
	isSpecular = !isSpecular;
}
void createGLUTMenus();
void toggleNextModel() {
	if (fluidMode < 1)
		fluidMode++;
	else fluidMode = 0;
	init();
	createGLUTMenus();
}

void toggleAddHeight() {
	f1->ChangeHeight(f1->HEIGHT_SCALE + 0.1f);
}

void toggleReduceHeight() {
	f1->ChangeHeight(f1->HEIGHT_SCALE - 0.1f);
}
void toggleTexture() {
	isTexture = !isTexture;
	init();
}


//����Ҽ��Ĳ˵��¼�
void processMenuEvents_f0(int option) {
	switch (option) {
	case 0:        togglePaused();       break;
	case 1:       toggleAddSpeed();      break;
	case 2:         toggleReduceSpeed();        break;
	case 3:      toggleAddResist();     break;
	case 4:         toggleReduceResist();        break;
	case 5:	toggleChangeDisplayMode(); break;
	case 6: toggleNextModel(); break;
	case 7:toggleAmbient(); break;
	case 8:toggleDiffuse(); break;
	case 9:toggleSpecular(); break;
	case 10:toggleTexture(); break;
	}
}
void processMenuEvents_f1(int option) {
	switch (option) {
	case 0:        togglePaused();       break;

	case 1:	toggleChangeDisplayMode(); break;
	case 2:toggleNextModel(); break;
	case 3:toggleAddHeight(); break;
	case 4:toggleReduceHeight(); break;

	case 5:toggleAmbient(); break;
	case 6:toggleDiffuse(); break;
	case 7:toggleSpecular(); break;
	}
}


//��������Ҽ��˵�
void createGLUTMenus() {
	int menu;
	switch (fluidMode)
	{
	case 0:
		//�����˵�
		menu = glutCreateMenu(processMenuEvents_f0);
		//Ϊ�˵�����ѡ��
		glutAddMenuEntry("Pause/Unpause", 0);
		glutAddMenuEntry("Add Speed",1);
		glutAddMenuEntry("Reduce Speed", 2);
		glutAddMenuEntry("Add Resistence", 3);
		glutAddMenuEntry("Reduce Resistence", 4);
		glutAddMenuEntry("DisplayMode: Line/Fill", 5);
		glutAddMenuEntry("Next Wave Mode", 6);
		glutAddMenuEntry("Close/Open Ambient light", 7);
		glutAddMenuEntry("Close/Open Diffuse light", 8);
		glutAddMenuEntry("Close/Open Specular light", 9);
		glutAddMenuEntry("Add/Remove Texture", 10);
		break;
	case 1:
		//�����˵�
		menu = glutCreateMenu(processMenuEvents_f1);
		//Ϊ�˵�����ѡ��
		glutAddMenuEntry("Pause/Unpause (P)", 0);

		glutAddMenuEntry("DisplayMode: Line/Fill", 1);
		glutAddMenuEntry("Next Wave Mode", 2);
		glutAddMenuEntry("Add Height", 3);
		glutAddMenuEntry("Reduce Height", 4);
		glutAddMenuEntry("Close/Open Ambient light", 5);
		glutAddMenuEntry("Close/Open Diffuse light", 6);
		glutAddMenuEntry("Close/Open Specular light", 7);
		break;
	default:
		break;
	}
	// attach the menu to the right button
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}


int main(int argc, char *argv[])
{
	glutInit(&argc, argv);//��glut�ĳ�ʼ�� 
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	//��ʼ����ʾģʽ:RGB��ɫģ�ͣ���Ȳ��ԣ�˫����                 
	glutInitWindowSize(500, 500);//���ô��ڴ�С               
	glutCreateWindow("Simple GLUT Wave App");//���ô��ڱ���                 
	glutDisplayFunc(redraw); //ע����ƻص�����               
	glutReshapeFunc(reshape);   //ע���ػ�ص�����     

	glutKeyboardFunc(keyDownHandler);		// Keydown handler
	glutMouseFunc(mouseDownHandler);		// Mousedown handler
	glutMotionFunc(mouseMoveHandler);		// Mouse movement handler
	glutSpecialFunc(moveHandler);//����ǰ�����˿�����

	glutIdleFunc(idle);//ע��ȫ�ֻص�����������ʱ����             


	createGLUTMenus();
	init();
	glutMainLoop();  // glut�¼�����ѭ��    
	return 0;
}