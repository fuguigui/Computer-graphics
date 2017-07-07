#pragma once  
#include "VectorClasses.h"    


class Fluid_0
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

	float r, g, b, a;	//���ڼ�¼��ɫֵ


	float time, v, mu;//ʱ�䣬���٣�����ϵ��
	float dist;//���������֮��ľ���

						//n:����� m������� d�������֮��ľ���  
						//t:ʱ�� c������ mu������ϵ��  
	Fluid_0(long n, long m, float d, float t, float c, float mu);
	Fluid_0(long n, long m, float d, float t, float c, float mu, int tex);
	~Fluid_0();

	void Evaluate(void);
	void draw();

	void ChangeSpeed(float v);//�ı䲨��
	void ChangeResist(float r);//�ı�����ϵ��
};