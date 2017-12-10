#pragma once
#include "VectorClasses.h" 
#include "Tools.h"
#define WAVE_NUM 3//���ĸ���
class  Fluid_1
{
public:
	//xnum:x�����ϵ������,ynum:y�����ϵ������,zz��ʼʱ��z�����λ��
	 Fluid_1(int xnum, int ynum,float zz);
	 Fluid_1(int xnum, int ynum, float zz,int tex);
	~ Fluid_1();

	struct WaveValue{
		float time;
		float wave_length[WAVE_NUM],//���ĳ���
			wave_height[WAVE_NUM],//���Ŀ��
			wave_dir[WAVE_NUM],//���ĽǶ�
			wave_speed[WAVE_NUM],//�����ٶ�
			wave_start[WAVE_NUM][2];//������ʼ����
	} Wave_Value;//�̻�ÿ���������в���ֵ

	void initWave(float values[WAVE_NUM][6],float start_x,float start_y,float x_dist,float y_dist);//��ʼ�����в��Ĳ�������������ĳ�ʼλ�ã������ڵ�ĺ�������������
	void Evaluate();//�������ź����ֵ;
	float gerstnerZ(float w_length, float w_height, float x_in, float gest[22]);//����ÿ�����zֵ
	void draw();
	void ChangeHeight(float height);


	float HEIGHT_SCALE = 0.6f;//����ĸ߶ȳ߶�
private:
	float *gerstner[22];//��ʼ���Ĳ��Ĳ���
	int gerNum;//��ʼ���Ĳ��ĸ���
	int width;//����Ŀ�ȣ��õ�ĸ�������
	int length;//����ĳ��ȣ��õ�ĸ�������
	float z_start;//������ʼλ�õ�z����


	Vector3D        *normal; //����  
	Vector3D        *tangent; //����  

	Vector3D        *buffer; //������  
	int             *indices[3]; //����  
	float       *texcoords[3][2]; //��������  
	bool			isTexed = false;//�Ƿ��������

	int texture;
};
