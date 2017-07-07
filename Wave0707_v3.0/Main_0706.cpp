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
static bool isTexture = true;//贴纹理
static bool drawTiles = false;
static bool paused = false;
static bool realTime = true;  // when false, animate 0.1 seconds per frame regardless of FPS
static bool solveNavierStokes = false;
static bool fullScreen = false;
static bool isLine = false;//用于在line和fill的显示模式之间做切换
static bool isDiffuse = true;//开启环境光
static bool isAmbient = true;//开启漫反射
static bool isSpecular = false;//开启聚光灯

							   //控制灯光的开启
GLboolean isLightOn = true;

static bool choppyWaves = true;
static bool fftNormals = false;
static bool displayForce = false;

//纹理缓冲区
GLuint texture[13];
//视区        
float whRatio;
int wHeight = 0;
int wWidth = 0;
//流体  
Fluid_0 *f0;
Fluid_1 *f1;
int fluidMode = 0;//选择的波浪模式，默认为0

//视点        
float center[] = { 20,10,0 };
float eye[] = { 20,-10,8 };

int mox, moy, mnx, mny, mousedown; // Mouse original X and Y and new X and Y, and whether the mouse is pressed
float mouseSpeed;
float x, y, z, rx, ry; // X, Y, and Z position, and rotation around X and Y axes

			  //环境光位置  
GLfloat light_x = 0.0f;
GLfloat light_y = 0.0f;
GLfloat light_z = 0.0f;

//聚光灯方向  
GLfloat dir_x = 0.0f;
GLfloat dir_y = 0.0f;
GLfloat dir_z = 0.0f;

GLfloat light_pos[4] = { 40.0, 0.0, 10.0, 1 };//环境光的位置
GLfloat light_pos1[4] = { 0.0f, 5.0f, 0.0f, 1.0f }; //定义聚光灯位置  								
GLfloat light_Dir[3] = { 0.0f + dir_x ,-1.0f + dir_y ,0.0f + dir_z }; ///光照角度

GLfloat color[] = { 1.0, 1.0, 1.0, 1.0 }; // 定义颜色  


GLfloat spotangle = 5.0f; //角度  


float wave_para[6][6] = {
	{ 1.6,	0.12,	0.9,	0.6,	0.0,	0.0 },
	{ 1.3,	0.1,	1.14,	0.9,	0.0,	0.0 },
	{ 0.2,	0.01,	0.8,	0.8,	0.0,	0.0 },
	{ 0.18,	0.008,	1.05,	1.0,	0.0,	0.0 },
	{ 0.23,	0.005,	1.15,	0.9,	0.0,	0.0 },
	{ 0.12,	0.003,	0.97,	1.4,	0.0,	0.0 }
};//波浪的参数


void init()
{
	srand(unsigned(time(NULL)));


	x = 0; y = 0; z = 0; rx = 20; ry = -20;
	mouseSpeed = 0.5;
	mousedown = 0;
	//预设水流模式
	switch (fluidMode)
	{
	case 0:
		//生成纹理
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



	// 启用颜色追踪      
	glEnable(GL_COLOR_MATERIAL);
	// 设置多边形正面的环境光和散射光材料属性，追踪glColor      
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);// 设置多边形正面的镜面反射属性  
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 80);// 指定镜面指数  
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse3); //设置多边形正面漫反射属性
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色和深度缓存          
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();   //初始化矩阵为单位矩阵    



	glTranslatef(x, y, z);
	glRotatef(rx, 1, 0, 0);
	glRotatef(ry, 0, 1, 0);

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 0,1);                // 场景（0，0，0)，Y轴向上        
	if(isLine)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else 
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_DEPTH_TEST);//开启深度测试     
	glFrontFace(GL_CCW);//逆时针面向外
	glEnable(GL_CULL_FACE);//隐藏面消除

	glEnable(GL_LIGHTING);

	GLfloat specularLight[] = { 0.3f, 0.68f, 0.6f, 1.0f }; //定义灯光0的颜色 
	GLfloat diffuseLight[] = { 0.2f,0.5f,0.6f,1.0f };
	GLfloat ambientLight[] = { 0.2f,0.3f,0.3f,1.0f };
	GLfloat empty[] = { 0.0f,0.0f,0.0f,0.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos); //设置第0号光源的光照位置    
	if(isSpecular)
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight); //设置镜面反射光照颜色  
	else glLightfv(GL_LIGHT0, GL_SPECULAR, empty);
	if(isDiffuse)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);                     //设置漫射光成分  
	else glLightfv(GL_LIGHT0, GL_DIFFUSE, empty);
	if(isAmbient)
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);   //设置第0号光源多次反射后的光照颜色（环境光颜色） 
	else glLightfv(GL_LIGHT0, GL_AMBIENT, empty);

	glPopMatrix();

	if (isLightOn)
		glEnable(GL_LIGHT0); 

	glShadeModel(GL_SMOOTH);
	// 启用光照计算      

	// 设置光照模型，将ambientLight所指定的RGBA强度值应用到环境光      
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	//设置模板，当画镜面时把镜面范围的模板值换成1&0xff  
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	//画镜面时必须设置深度值为只读，否则会更新深度值导致倒影画不上去  
	glDepthMask(GL_FALSE);


	drawScene();//绘制场景  

	glDepthMask(GL_TRUE);

	//继续设置模板测试，不等于镜面模板值的则不会显示  
	glStencilFunc(GL_EQUAL, 1, 0xff);
	glStencilFunc(GL_KEEP, GL_KEEP, GL_KEEP);
	glDisable(GL_STENCIL_TEST);

	inputHandler();

	glutSwapBuffers();//交换缓冲区        
}

void updateView(int height, int width)
{
	glViewport(-20, -20, width, height);
	glMatrixMode(GL_PROJECTION);//设置矩阵模式为投影         
	glLoadIdentity();   //初始化矩阵为单位矩阵            
	whRatio = (GLfloat)width / (GLfloat)height;  //设置显示比例       
												 //	gluPerspective(80.0f, whRatio, -1.5f, 20.0f); //透视投影          
												 //	glFrustum(0, 5, 0, 10, 5,100);        
	glOrtho(0, 40, -10, 80, 5, 100);
	glMatrixMode(GL_MODELVIEW);  //设置矩阵模式为模型      
}


void reshape(int width, int height)
{
	if (height == 0)      //如果高度为0            
	{
		height = 1;   //让高度为1（避免出现分母为0的现象）            
	}

	wHeight = height;
	wWidth = width;

	updateView(wHeight, wWidth); //更新视角            
}
void keyDownHandler(unsigned char c, int mx, int my) // Handles keyboard input
{
	switch (c)
	{
	case 'a'://向左移动视角
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
	case 'l': {//控制灯光的开关
		isLightOn = !isLightOn;
		break;
	}
	case 'i': {//环境光上移  
		light_y = light_y + 0.2f;
		break;
	}
	case 'k': {//环境光下移  
		light_y = light_y - 0.2f;
		break;
	}
	case 'n': {//环境光前移  
		light_z = light_z + 0.2f;
		break;
	}
	case 'm': {//环境光后移  
		light_z = light_z - 0.2f;
		break;
	}
	case 'f': {//聚光灯左移  
		dir_x = dir_x - 0.05f;
		break;
	}
	case 'h': {//聚光灯右移  
		dir_x = dir_x + 0.05f;
		break;
	}
	case 't': {//聚光灯上移  
		dir_y = dir_y - 0.05f;
		break;
	}
	case 'g': {//聚光灯下移  
		dir_y = dir_y + 0.05f;
		break;
	}
	case 'v': {//聚光灯后移  
		dir_z = dir_z - 0.05f;
		break;
	}
	case 'b': {//聚光灯前移  
		dir_z = dir_z + 0.05f;
		break;
	}
	case 'x': { //聚光灯角度变大  
		if (spotangle <= 89.0f) {
			spotangle = spotangle + 0.2f;
		}
		break;
	}
	case 'y': { //聚光灯角度变小  
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


//鼠标右键的菜单事件
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


//增加鼠标右键菜单
void createGLUTMenus() {
	int menu;
	switch (fluidMode)
	{
	case 0:
		//创建菜单
		menu = glutCreateMenu(processMenuEvents_f0);
		//为菜单增加选项
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
		//创建菜单
		menu = glutCreateMenu(processMenuEvents_f1);
		//为菜单增加选项
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
	glutInit(&argc, argv);//对glut的初始化 
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	//初始化显示模式:RGB颜色模型，深度测试，双缓冲                 
	glutInitWindowSize(500, 500);//设置窗口大小               
	glutCreateWindow("Simple GLUT Wave App");//设置窗口标题                 
	glutDisplayFunc(redraw); //注册绘制回调函数               
	glutReshapeFunc(reshape);   //注册重绘回调函数     

	glutKeyboardFunc(keyDownHandler);		// Keydown handler
	glutMouseFunc(mouseDownHandler);		// Mousedown handler
	glutMotionFunc(mouseMoveHandler);		// Mouse movement handler
	glutSpecialFunc(moveHandler);//方向前景后退控制器

	glutIdleFunc(idle);//注册全局回调函数：空闲时调用             


	createGLUTMenus();
	init();
	glutMainLoop();  // glut事件处理循环    
	return 0;
}