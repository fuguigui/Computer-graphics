#define _CRT_SECURE_NO_WARNINGS        

#include <stdio.h>        
#include <string.h>        
#include<time.h>      
#include <stdlib.h>   
#include<glut.h>
#include"test.h"        
#include"fluid.h"  
//#include "Fluid_0702.h"
//纹理缓冲区
GLuint texture[13];
//视区        
float whRatio;
int wHeight = 0;
int wWidth = 0;
//流体  
Fluid *f;

//视点        
//float center[] = { 27, 10, 0 };
//float eye[] = { 27,-50, 50 };
float center[] = { 30,40,0 };
float eye[] = { 30,0,8 };

int mox, moy, mnx, mny, mousedown; // Mouse original X and Y and new X and Y, and whether the mouse is pressed
float mouseSpeed;
float x, y, z, rx, ry; // X, Y, and Z position, and rotation around X and Y axes
int fluidMode;//选择水流的模式，预定义了几种模式

					 //环境光位置  
GLfloat light_x = 0.0f;
GLfloat light_y = 0.0f;
GLfloat light_z = 0.0f;

//聚光灯方向  
GLfloat dir_x = 0.0f;
GLfloat dir_y = 0.0f;
GLfloat dir_z = 0.0f;

GLfloat light_pos[4] = { 50.0, 80.0, 10.0, 1 };//环境光的位置
GLfloat light_pos1[4] = { 0.0f, 5.0f, 0.0f, 1.0f }; //定义聚光灯位置  								
GLfloat light_Dir[3] = { 0.0f + dir_x ,-1.0f + dir_y ,0.0f + dir_z }; ///光照角度

GLfloat color[] = { 1.0, 1.0, 1.0, 1.0 }; // 定义颜色  


GLfloat spotangle = 5.0f; //角度  

//控制灯光的开启
GLboolean isLightOn = true;

void init()
{
	srand(unsigned(time(NULL)));


	x = 0; y = 0; z = 0; rx = 20; ry = -20;
	mouseSpeed = 0.5;
	mousedown = 0;
	//生成纹理
	glGenTextures(1, texture);
	loadTex(0, "water.bmp", texture);
	//预设水流模式
	fluidMode = 1;
	f = new Fluid(200, 100, 2, 0.5, 0.5, 1); 
	//f = new Fluid(100, 100, 2, 1, 0.3, 0,texture[0]);
}
void drawScene()
{/*
	switch (fluidMode)
	{
	case 0:
		f = new Fluid(100, 100, 2, 1, 0.3, 0);
		break;
	case 1:f = new Fluid(200, 100, 4, 0.5, 2.0, 1); break;
	case 2:f = new Fluid(200, 200, 2, 2, 1.0, 0); break;
	default:
		break;
	}*/
	GLfloat mat_specular[] = { 0.3f, 0.8f, 0.8f, 1.0f };
	GLfloat mat_diffuse0[] = { 0.85f, 0.65f, 0.2f, 1.0f };
//	GLfloat mat_diffuse1[] = { 1.0f, 0.0f, 0.0f };
//	GLfloat mat_diffuse2[] = { 0.0f, 1.0f, 0.0f };
	GLfloat mat_diffuse3[] = { 0.5f, 1.0f, 0.0f };
//	GLfloat mat_diffuse4[] = { 0.0f, 1.0f, 1.0f };
//	GLfloat mat_diffuse5[] = { 0.0f, 0.0f, 1.0f };

	glPushMatrix();



	// 启用颜色追踪      
	glEnable(GL_COLOR_MATERIAL);
	// 设置多边形正面的环境光和散射光材料属性，追踪glColor      
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);// 设置多边形正面的镜面反射属性  
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 100);// 指定镜面指数  
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse3); //设置多边形正面漫反射属性  
//	glTranslatef(0, 0, 4 + 1);
//	glRotatef(90, 1, 0, 0);
//	glutSolidTeapot(1);

	static int count = 0;
	count++;
	if (count > 100) {
		count = 0;
		f->Evaluate();
	}
	f->draw();


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

	gluLookAt(eye[0], eye[1], eye[2], center[0], center[1], center[2], 0, 1, 0);                // 场景（0，0，0)，Y轴向上        
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//glPushMatrix(); 

	glEnable(GL_DEPTH_TEST);//开启深度测试     
	glFrontFace(GL_CCW);//逆时针面向外
	glEnable(GL_CULL_FACE);//隐藏面消除
//	glEnable(GL_LIGHTING);  //开启光照模式    

//	glDisable(GL_LIGHTING);
	glPushMatrix();
	glTranslatef(light_pos[0], light_pos[1], light_pos[2]);//移动绘图原点到光源处
	glColor3f(1.0f, 1.0f, 0.0f);           //设置球的颜色为黄色
	glutSolidSphere(0.5f, 50.0f, 50.0f);    //绘制球

	glEnable(GL_LIGHTING);

	GLfloat gold[] = { 1.0f, 0.6f, 0.0f, 1.0f }; //定义金色 
	GLfloat diffuseLight[] = { 0.7f,0.5f,0.0f,1.0f };
	GLfloat ambientLight[] = { 0.3f,0.3f,0.3f,1.0f };

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos); //设置第0号光源的光照位置    
	glLightfv(GL_LIGHT0, GL_SPECULAR, gold); //设置镜面反射光照颜色  
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);                     //设置漫射光成分  
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);   //设置第0号光源多次反射后的光照颜色（环境光颜色）  

	glPopMatrix();

	if(isLightOn)
	glEnable(GL_LIGHT0); //开启第0号光源   

/*	glLightfv(GL_LIGHT1, GL_AMBIENT, color);             //设置环境光成分  
	glLightfv(GL_LIGHT1, GL_SPECULAR, white);                    //设置镜面光成分  
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white);                     //设置漫射光成分  

	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
	glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, spotangle);             //裁减角度  
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, light_Dir);          //光源方向  
	glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.);                    //聚集度  

	glEnable(GL_LIGHT1);*/



	glShadeModel(GL_SMOOTH);
	// 启用光照计算      

	// 设置光照模型，将ambientLight所指定的RGBA强度值应用到环境光      
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	drawScene();//绘制场景      
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
/*	keys[c] = 1;
	if (selectingNode)
	{
		int current;
		if (selectingNode == 1) current = ux1;
		else if (selectingNode == 2) current = uy1;
		else if (selectingNode == 3) current = ux2;
		else if (selectingNode == 4) current = uy2;
		lastSelectingNode = selectingNode;
		selectionUpdate = 1;
		if (c == 27 || c == 'q')
		{
			ux1 = 0; ux2 = 0; uy1 = 0; uy2 = 0;
			selectingNode = 0;
		}
		else
		{
			if (c == '\b')
			{
				if (current == 0) selectingNode -= 1;
				else current /= 10;
				if (selectingNode < 3) rangeSelect = 0;
			}
			else if (c > 47 && c < 58)
			{
				current = current * 10 + c - 48;
			}
			else if (c == '-' && selectingNode < 3)
			{
				ux2 = 0; uy2 = 0;
				rangeSelect = 1;
				selectingNode = 3;
			}
			else if (c == ',' && selectingNode == 1) selectingNode = 2;
			else if (c == ',' && selectingNode == 3) selectingNode = 4;
			else if (c == 13)
			{
				if (rangeSelect)
				{
					x1 = imax(imin(ux1, ux2), 0);
					x2 = imin(imax(ux1, ux2), w - 1);
					y1_ = imax(imin(uy1, uy2), 0);
					y2 = imin(imax(uy1, uy2), h - 1);
				}
				else
				{
					x2 = x1 = imax(imin(ux1, w - 1), 0);
					y2 = y1_ = imax(imin(uy1, h - 1), 0);
				}
				selectingNode = 0;
			}
			if (lastSelectingNode == 1) ux1 = current;
			else if (lastSelectingNode == 2) uy1 = current;
			else if (lastSelectingNode == 3) ux2 = current;
			else if (lastSelectingNode == 4) uy2 = current;
		}
	}
	else if (selectingHeight || adjustingHeight)
	{
		if (c == '\b')
		{
			if (selectedHeight == 0) heightModifier = 1;
			else selectedHeight /= 10;
			if (selectedHeight < 0.0099) selectedHeight = 0; // Stupid
		}
		else if (c > 47 && c < 58) selectedHeight = selectedHeight * 10 + 0.01 * (c - 48);
		else if (c == '-') heightModifier *= -1;
		else if (c == 13) adjustingHeight = selectingHeight = 0;
		else if (c == 27 || c == 'q' || (c == 'h' && selectingHeight) || (c == 'j' && adjustingHeight))
		{
			selectingHeight = 0;
			adjustingHeight = 0;
			for (int i = x1; i <= x2; i++) for (int j = y1_; j <= y2; j++) depth[i][j] = save[i][j];
		}
		selectionUpdate = 1;
	}
	else if (selectingPreset)
	{
		if (c > 47 && c < 58)
		{
			modelNum = c - 48;
			initView();
			run = 0;
		}
		selectingPreset = 0;
	}
	else
	{
		if (c == 'q')
		{
			cleanup();
			exit(0);
		}
		else if (c == 13)
		{
			selectingNode = 1;
			selectionUpdate = 1;
			ux1 = 0; ux2 = 0; uy1 = 0; uy2 = 0;
			rangeSelect = 0;
		}
		else if (c == 'h')
		{
			selectingHeight = 1;
			selectionUpdate = 1;
			selectedHeight = 0;
			heightModifier = 1;
			for (int i = x1; i <= x2; i++) for (int j = y1_; j <= y2; j++) save[i][j] = depth[i][j];
		}
		else if (c == 'j')
		{
			adjustingHeight = 1;
			selectionUpdate = 1;
			selectedHeight = 0;
			heightModifier = 1;
			for (int i = x1; i <= x2; i++) for (int j = y1_; j <= y2; j++) save[i][j] = depth[i][j];
		}
		else if (c == 'o')
		{
			for (int i = x1; i <= x2; i++) for (int j = y1_; j <= y2; j++)
			{
				if (connect[i][j] > 0) save[i][j] = depth[i][j];
				else depth[i][j] = save[i][j];
				connect[i][j] = (connect[i][j] + 1) % 2;
			}

		}
		else if (c == 'f')
		{
			cleanup();
			init();
		}
		else if (c == 'r') initView();
		else if (c == ' ') run = (run + 1) % 2;
		else if (c == '=' || c == '+') viscosity /= 1.5;
		else if (c == '-' || c == '_') viscosity *= 1.5;
		else if (c == '[' && heightCoef >= 0.1) heightCoef -= 0.1;
		else if (c == ']') { heightCoef += 0.1; }
		else if (c == '.') { step(); run = 0; }
		else if (c == 'x') { vwireframe = (vwireframe + 1) % 2; vsolid = (vwireframe + 1) % 2; }
		else if (c == 'v') vnormals = (vnormals + 1) % 2;
		else if (c == 'c') colorMode = (colorMode + 1) % 4;
		else if (c == 'b') vheights = (vheights + 1) % 2;
		else if (c == 'n' && atten > 0) atten -= 0.00001;
		else if (c == 'm') atten += 0.00001;
		else if (c == 'i') selectingPreset = 1;
	}
	glutPostRedisplay();*/
}

void keyUpHandler(unsigned char c, int mx, int my)
{
	//keys[c] = 0.9;
	//glutPostRedisplay();
}
void mouseMoveHandler(int x, int y)
{
	mnx = x;
	mny = y;
	glutPostRedisplay();
}
void moveHandler(GLint key,GLint x,GLint y) {
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
	// Zoom
	//else if (button == 3) z += mouseSpeed;
//	else if (button == 4) z -= mouseSpeed;
	glutPostRedisplay();
}
void idle()
{
	inputHandler();
	glutPostRedisplay();
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
	glutKeyboardUpFunc(keyUpHandler);		// Keyup handler
	glutMouseFunc(mouseDownHandler);		// Mousedown handler
	glutMotionFunc(mouseMoveHandler);		// Mouse movement handler
	glutSpecialFunc(moveHandler);//方向前景后退控制器

	glutIdleFunc(idle);//注册全局回调函数：空闲时调用             

	init();
	glutMainLoop();  // glut事件处理循环    
	return 0;
}
