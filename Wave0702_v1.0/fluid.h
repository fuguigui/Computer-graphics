#pragma once  
#include "VectorClasses.h"    


class Fluid
{
private:

	long            width; //��  
	long            height; //��  

	Vector3D        *buffer[2]; //������  
	long            renderBuffer;  //��ǰ��Ⱦ�Ļ�����  

								   Vector3D        *normal; //����  
								   Vector3D        *tangent; //����  

	int             *indices[3]; //����  
	float       *texcoords[3][2]; //��������  
	float           k1, k2, k3; //����ʽϵ��  
	bool			isTexed = false;//�Ƿ��������

	int texture;
public:

	//n:����� m������� d�������֮��ľ���  
	//t:ʱ�� c������ mu������ϵ��  
	float r, g, b, a;	//���ڼ�¼��ɫֵ
	Fluid(long n, long m, float d, float t, float c, float mu);
	Fluid(long n, long m, float d, float t, float c, float mu, int tex);
	~Fluid();

	void Evaluate(void);
	void draw();
	void setColor(int type,float z);//������ɫ���ͺ͸߶�������ɫ
};