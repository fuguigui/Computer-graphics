#define _CRT_SECURE_NO_WARNINGS        

#include <stdio.h>        
#include <string.h>        
#include<time.h>      
#include <stdlib.h>   
#include<glut.h>
#include"test.h"        
#include"fluid.h"  

//纹理缓冲区
GLuint texture[13];
//视区        
float whRatio;
int wHeight = 0;
int wWidth = 0;
//流体  
Fluid *f;

//视点        
float center[] = { 27, 10, 0 };
float eye[] = { 27,-50, 50 };

int mox, moy, mnx, mny, mousedown; // Mouse original X and Y and new X and Y, and whether the mouse is pressed
float mouseSpeed;
float x, y, z, rx, ry; // X, Y, and Z position, and rotation around X and Y axes

void init()
{
	srand(unsigned(time(NULL)));
	glEnable(GL_DEPTH_TEST);//开启深度测试         

	glEnable(GL_LIGHTING);  //开启光照模式         
	x = 0; y = 0; z = 0; rx = 20; ry = -20;
	mouseSpeed = 0.5;
	mousedown = 0;
	//生成纹理
	glGenTextures(1, texture);
	loadTex(0, "water.bmp", texture);
	//生成水流
	f = new Fluid(30, 30, 2, 1, 0.6, 0);
}
void drawScene()
{
	static int count = 0;
	count++;
	if (count > 100) {
		count = 0;
		f->Evaluate();
	}
	f->draw();
}


void key(unsigned char key, int x, int y) {
	switch (key)
	{
//	case 'a': lRotate(); break;
//	case 'd': rRotate(); break;
//	case 'z': zIncrease(); break;
	case '\033': exit(EXIT_SUCCESS); break;
	default: break;
	}
	glutPostRedisplay();

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

	glShadeModel(GL_SMOOTH);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	// 启用光照计算      
	glEnable(GL_LIGHTING);
	// 指定环境光强度（RGBA）      
	GLfloat ambientLight[] = { 2.0f, 2.0f, 2.0f, 1.0f };

	// 设置光照模型，将ambientLight所指定的RGBA强度值应用到环境光      
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);
	// 启用颜色追踪      
	glEnable(GL_COLOR_MATERIAL);
	// 设置多边形正面的环境光和散射光材料属性，追踪glColor      
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

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
	gluPerspective(45.0f, whRatio, 1.0f, 200.0f); //透视投影          
												  //glFrustum(-3, 3, -3, 3, 3,100);        
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
	keys[c] = 1;
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
	glutPostRedisplay();
}

void keyUpHandler(unsigned char c, int mx, int my)
{
	keys[c] = 0.9;
	glutPostRedisplay();
}
void mouseMoveHandler(int x, int y)
{
	mnx = x;
	mny = y;
	glutPostRedisplay();
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
	else if (button == 3) z += mouseSpeed;
	else if (button == 4) z -= mouseSpeed;
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

	glutIdleFunc(idle);//注册全局回调函数：空闲时调用             

	init();
	glutMainLoop();  // glut事件处理循环    
	return 0;
}
