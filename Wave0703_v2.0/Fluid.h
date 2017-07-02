#pragma once
#include "VectorClasses.h" 
#define WAVE_NUM 6//波的个数
//为了尽可能地减小计算量，用两种固定形状的gerstner波，每种波用11对坐标表示，
//计算z值时，只需在这11个点中计算线性内插即可
//第一个波峰较尖，用来绘制细小的水波，第二个波峰较宽，用来绘制波长较长的水波

class  Fluid
{
public:
	//xnum:x方向上点的数量,ynum:y方向上点的数量,zz初始时刻z坐标的位置
	 Fluid(int xnum, int ynum,float zz);
	 Fluid(int xnum, int ynum, float zz,int tex);
	~ Fluid();

	struct {
		float time;
		float wave_length[WAVE_NUM],//波的长度
			wave_height[WAVE_NUM],//波的宽度
			wave_dir[WAVE_NUM],//波的角度
			wave_speed[WAVE_NUM],//波的速度
			wave_start[WAVE_NUM][2];//波的起始坐标
	} Wave_Value;//刻画每个波的所有参数值

	void initWave(float values[WAVE_NUM][6],float start_x,float start_y,float x_dist,float y_dist);//初始化所有波的参数，给定海面的初始位置，和相邻点的横向间隔，纵向间隔
	void Evaluate();//计算整张海面的值;
	float gerstnerZ(float w_length, float w_height, float x_in, float gest[22]);//计算每个点的z值
	void draw();


private:
	float *gerstner[22];//初始化的波的参数
	int gerNum;//初始化的波的个数
	int width;//海面的宽度，用点的个数计算
	int length;//海面的长度，用点的个数计算
	float z_start;//海面起始位置的z坐标

	Vector3D        *normal; //法线  
	Vector3D        *tangent; //切线  

	Vector3D        *buffer; //缓冲区  
	long            renderBuffer;  //当前渲染的缓冲区  

	int             *indices[3]; //索引  
	float       *texcoords[3][2]; //纹理坐标  
	bool			isTexed = false;//是否加载纹理

	int texture;
};

int normalizeF(Vector3D , Vector3D );
