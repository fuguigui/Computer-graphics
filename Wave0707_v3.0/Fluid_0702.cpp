#include<iostream>
#include "Fluid_0702.h"
#include "Tools.h"

//为了尽可能地减小计算量，用两种固定形状的gerstner波，每种波用11对坐标表示，
//计算z值时，只需在这11个点中计算线性内插即可
//第一个波峰较尖，用来绘制细小的水波，第二个波峰较宽，用来绘制波长较长的水波

float gerstner_pt_a[22] = {
	0.0,0.0, 41.8,1.4, 77.5,5.2, 107.6,10.9,
	132.4,17.7, 152.3,25.0, 167.9,32.4, 179.8,39.2,
	188.6,44.8, 195.0,48.5, 200.0,50.0
};
float gerstner_pt_b[22] = {
	0.0,0.0, 27.7,1.4, 52.9,5.2, 75.9,10.8,
	97.2,17.6, 116.8,25.0, 135.1,32.4, 152.4,39.2,
	168.8,44.8, 184.6,48.5, 200.0,50.0
};
GLint gerstner_sort[6] = {
	0, 0, 1, 1, 1, 1 
};


Fluid_1::Fluid_1(int xnum,int ynum,float z)
{
	width = xnum; //宽度  
	length = ynum;  //长度 
	z_start = z;//初始z位置
	int count = width*length;

	buffer = new Vector3D[count];  //缓冲区
//	renderBuffer = 0;  //渲染缓冲区  

	normal = new Vector3D[count]; //法线  
	tangent = new Vector3D[count]; //切线  
//	normal = new Vector3D[2 * (width - 1)*(length - 1)];//法线，一个三角形一条法线
//	tangent = new Vector3D[2 * (width - 1)*(length - 1)];//切线，一个三角形对应一条切线

	for (int i = 0; i < 3; i++) {
		indices[i] = new int[2 * (width - 1)*(length - 1)];
	}
}

Fluid_1::Fluid_1(int xnum, int ynum, float zz, int tex) {
	Fluid_1(xnum, ynum, zz);
	isTexed = true;
	texture = tex;

	for (int i = 0; i < 6; i++) {
		texcoords[i % 3][i / 3] = new float[2 * (width - 1)*(length - 1)];
	}
}

Fluid_1::~Fluid_1() {
		delete[] tangent;
		delete[] normal;
		delete[] buffer;
		delete[] indices;
		delete[] texcoords;
}


//初始化波，给出每个波的参数，海浪开始的x,y坐标，x,y间距
void Fluid_1::initWave(float wave_para[WAVE_NUM][6],float x_pos,float y_pos,float x_dist,float y_dist) {
	//Initialize 波的值
	Wave_Value.time = 0.0;
	for (int w = 0; w<WAVE_NUM; w++)
	{
		Wave_Value.wave_length[w] = wave_para[w][0];
		Wave_Value.wave_height[w] = wave_para[w][1];
		Wave_Value.wave_dir[w] = wave_para[w][2];
		Wave_Value.wave_speed[w] = wave_para[w][3];
		Wave_Value.wave_start[w][0] = wave_para[w][4];
		Wave_Value.wave_start[w][1] = wave_para[w][5];
	}

	//Initialize buffer
	int index = 0;
	for (int j = 0; j<length; j++)
	{
		for (int i = 0; i<width; i++)
		{
			buffer[index].x = x_pos + i*x_dist;
			buffer[index].y = y_pos + j*y_dist;

			normal[index].Set(0.0F, 0.0F, 2.0F * x_dist);
			tangent[index].Set(2.0F * x_dist, 0.0F, 0.0F);
			index++;
		}
	}
	//初始化索引网格，建立海面的三角形表示
	int a = 0;
	for (int j = 0; j < length - 1; j++) {
		for (int i = 0; i < width - 1; i++) {
			int t = j*width + i;
			indices[0][a] = t;
			indices[1][a] = t + 1;
			indices[2][a] = t + width;
			if (isTexed) {

				texcoords[0][0][a] = i*width;
				texcoords[0][1][a] = j*length;

				texcoords[1][0][a] = (i + 1)*width;
				texcoords[1][1][a] = j*length;

				texcoords[2][0][a] = i*width;
				texcoords[2][1][a] = (j + 1)*length;


			}
			a++;
		}
	}

	for (int j = 1; j < length; j++) {
		for (int i = 1; i < width; i++) {
			long t = j*width + i;
			indices[0][a] = t;
			indices[1][a] = t - 1;
			indices[2][a] = t - width;

			if (isTexed) {
				texcoords[0][0][a] = i*width;
				texcoords[0][1][a] = j*length;

				texcoords[1][0][a] = (i - 1)*width;
				texcoords[1][1][a] = j*length;

				texcoords[2][0][a] = i*width;
				texcoords[2][1][a] = (j - 1)*length;

			}
			a++;
		}
	}
}
//计算每个点的z值
float Fluid_1::gerstnerZ(float w_length, float w_height, float x_in, float gerstner[22])
{
	int totalNum = width*length;
	x_in = x_in * totalNum / w_length;

	while (x_in < 0.0)
		x_in += totalNum;
	while (x_in > totalNum)
		x_in -= totalNum;
	if (x_in > totalNum/2)
		x_in = totalNum - x_in;

	int i = 0;
	float yScale = w_height / length;
	while (i<18 && (x_in<gerstner[i] || x_in >= gerstner[i + 2]))
		i += 2;
	if (x_in == gerstner[i])
		return gerstner[i + 1] * yScale;
	if (x_in > gerstner[i])
		return ((gerstner[i + 3] - gerstner[i + 1]) * (x_in - gerstner[i]) / (gerstner[i + 2] - gerstner[i]) + gerstner[i + 3]) * yScale;
}

void Fluid_1::Evaluate() {
	//把各个波的效果叠加，计算每个点的Z值
	int index = 0;
	float d, wave;
	for (int j = 0; j<length; j++)
	{
		for (int i = 0; i<width; i++)
		{
			
			wave = 0.0;
			for (int w = 0; w<WAVE_NUM; w++) {
				d = (buffer[index].x - Wave_Value.wave_start[w][0] + (buffer[index].y - Wave_Value.wave_start[w][1]) 
					* tan(Wave_Value.wave_dir[w])) * cos(Wave_Value.wave_dir[w]);
				if (gerstner_sort[w] == 1) {
					wave += Wave_Value.wave_height[w] - 
						gerstnerZ(Wave_Value.wave_length[w], Wave_Value.wave_height[w], d + Wave_Value.wave_speed[w] * Wave_Value.time, gerstner_pt_a);
				}
				else {
					wave += Wave_Value.wave_height[w] 
						- gerstnerZ(Wave_Value.wave_length[w], Wave_Value.wave_height[w], d + Wave_Value.wave_speed[w] * Wave_Value.time, gerstner_pt_b);
				}
			}
			buffer[index].z = z_start + wave*HEIGHT_SCALE;
			index++;
		}
	}

	// Calculate normals and tangents.    
	//计算法线和切线  

	for (long j = 1; j < length - 1; j++)
	{
		const Vector3D *next = buffer + j * width;
		Vector3D *nrml = normal + j * width;
		Vector3D *tang = tangent + j * width;

		for (long i = 1; i < width - 1; i++)
		{
			nrml[i].x = next[i - 1].z - next[i + 1].z;
			nrml[i].y = next[i - width].z - next[i + width].z;
			tang[i].z = next[i + 1].z - next[i - 1].z;
		}
	}
	/*
	//计算每一个点的单位法线
	index = 0;
	for (int j = 0; j<length; j++)
	{
		for (int i = 0; i<width; i++)
		{
			int p0 = index - width, p1 = index + 1, p2 = index + width, p3 = index - 1;
			float xa, ya, za, xb, yb, zb;
			if (i > 0) {
				if (j > 0) {
					xa = buffer[p0].x - buffer[index].x;
					ya = buffer[p0].y - buffer[index].y;
					za = buffer[p0].z - buffer[index].z;
					xb = buffer[p3].x - buffer[index].x;
					yb = buffer[p3].y - buffer[index].y;
					zb = buffer[p3].z - buffer[index].z;
					normal[index].x += ya*zb - yb*za;
					normal[index].y += xb*za - xa*zb;
					normal[index].z += xa*yb - xb*ya;
				}
				if (j < length - 1) {
					xa = buffer[p2].x - buffer[index].x;
					ya = buffer[p2].y - buffer[index].y;
					za = buffer[p2].z - buffer[index].z;
					xb = buffer[p3].x - buffer[index].x;
					yb = buffer[p3].y - buffer[index].y;
					zb = buffer[p3].z - buffer[index].z;
					normal[index].x += ya*zb - yb*za;
					normal[index].y += xb*za - xa*zb;
					normal[index].z += xa*yb - xb*ya;
				}
			}
			if (i < width - 1) {
				if (j > 0) {
					xa = buffer[p0].x - buffer[index].x;
					ya = buffer[p0].y - buffer[index].y;
					za = buffer[p0].z - buffer[index].z;
					xb = buffer[p1].x - buffer[index].x;
					yb = buffer[p1].y - buffer[index].y;
					zb = buffer[p1].z - buffer[index].z;
					normal[index].x += ya*zb - yb*za;
					normal[index].y += xb*za - xa*zb;
					normal[index].z += xa*yb - xb*ya;
				}
				if (j < length - 1) {
					xa = buffer[p2].x - buffer[index].x;
					ya = buffer[p2].y - buffer[index].y;
					za = buffer[p2].z - buffer[index].z;
					xb = buffer[p1].x - buffer[index].x;
					yb = buffer[p1].y - buffer[index].y;
					zb = buffer[p1].z - buffer[index].z;
					normal[index].x += ya*zb - yb*za;
					normal[index].y += xb*za - xa*zb;
					normal[index].z += xa*yb - xb*ya;
				}
			}
			//如果正则化失败，输出错误信息
			if (normalizeF(normal[index], normal[index]))
				printf("%d\t%d\n", index / length, index % length);
			index ++;
		}
	}
	*/
}

void Fluid_1::draw()
{
	if (isTexed) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);  //选择纹理texture[status]       
		glBegin(GL_TRIANGLES);

		for (int i = 0; i < 2 * (length - 1)*(width - 1); i++) {

			glNormal3f(normal[(int)indices[0][i]].x,
				normal[(int)indices[0][i]].y,
				normal[(int)indices[0][i]].z);

			glTexCoord2f(texcoords[0][0][i], texcoords[0][1][i]);

			glVertex3f(buffer[indices[0][i]].x,
				buffer[indices[0][i]].y,
				buffer[indices[0][i]].z);


			glNormal3f(normal[(int)indices[1][i]].x,
				normal[(int)indices[1][i]].y,
				normal[(int)indices[1][i]].z);

			glTexCoord2f(texcoords[1][0][i], texcoords[1][1][i]);
			glVertex3f(buffer[indices[1][i]].x,
				buffer[indices[1][i]].y,
				buffer[indices[1][i]].z);

			glNormal3f(normal[(int)indices[2][i]].x,
				normal[(int)indices[2][i]].y,
				normal[(int)indices[2][i]].z);

			glTexCoord2f(texcoords[2][0][i], texcoords[2][1][i]);
			glVertex3f(buffer[indices[2][i]].x,
				buffer[indices[2][i]].y,
				buffer[indices[2][i]].z);
		}

		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	else {
		glColor3ub(0, 150, 200);
		glBegin(GL_TRIANGLES);

		for (int i = 0; i < 2 * (length - 1)*(width - 1); i++) {

			glNormal3f(normal[(int)indices[0][i]].x,
				normal[(int)indices[0][i]].y,
				normal[(int)indices[0][i]].z);

			glVertex3f(buffer[indices[0][i]].x,
				buffer[indices[0][i]].y,
				buffer[indices[0][i]].z);


			glNormal3f(normal[(int)indices[1][i]].x,
				normal[(int)indices[1][i]].y,
				normal[(int)indices[1][i]].z);

			glVertex3f(buffer[indices[1][i]].x,
				buffer[indices[1][i]].y,
				buffer[indices[1][i]].z);

			glNormal3f(normal[(int)indices[2][i]].x,
				normal[(int)indices[2][i]].y,
				normal[(int)indices[2][i]].z);

			glVertex3f(buffer[indices[2][i]].x,
				buffer[indices[2][i]].y,
				buffer[indices[2][i]].z);
		}

		glEnd();
	}
}



void Fluid_1::ChangeHeight(float height) {
	if (height < 0.0f)return;
	HEIGHT_SCALE = height;
}
