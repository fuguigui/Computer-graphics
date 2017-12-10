#include<stdlib.h>  
#include"fluid_0.h"  
#include"Tools.h"  

Fluid_0::Fluid_0(long n, long m, float d, float t, float c, float _mu)
//n:����� m������� d�������֮��ľ��� t:ʱ�� c������ mu������ϵ��  
{

	width = n; //���  
	height = m;  //�߶�  
	long count = n * m;  //��������  

	buffer[0] = new Vector3D[count];  //������1  
	buffer[1] = new Vector3D[count];  //������2  
	renderBuffer = 0;  //��Ⱦ������  

	normal = new Vector3D[count]; //����  
	tangent = new Vector3D[count]; //����  


	for (int i = 0; i < 3; i++) {
		indices[i] = new int[2 * (n - 1)*(m - 1)];
	}

	time = t;
	v = c;
	mu = _mu;
	dist = 10*d;
	float f1 = v * v * time * time / (dist * dist);
	float f2 = 1.0F / (mu * time + 2);
	k1 = (4.0F - 8.0F * f1) * f2;
	k2 = (mu * time - 2) * f2;
	k3 = 2.0F * f1 * f2;
	// Initialize buffers.    
	//��ʼ��������  
	long a = 0;

	for (long j = 0; j < m; j++)
	{
		float y = d * j;
		for (long i = 0; i < n; i++)
		{
			if (i == 0 || j == 0 || i == n - 1 || j == m - 1)buffer[0][a].Set(d * i, y, 0.0F);
			else {
				int r = rand() % 2;
				if (r == 0)buffer[0][a].Set(d * i, y, 1.0F);
				else buffer[0][a].Set(d * i, y, 0.0F);
			}
			buffer[1][a] = buffer[0][a];
			normal[a].Set(0.0F, 0.0F, 2.0F * d);
			tangent[a].Set(2.0F * d, 0.0F, 0.0F);
			a++;
		}
	}

	a = 0;
	float w = 1.0f / width;
	float h = 1.0f / height;
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < m - 1; j++) {
			/*	long t = i*n + j;
			indices[0][a] = t;
			indices[1][a] = t + 1;
			indices[2][a] = t + n;*/
			long t = j*n + i;
			indices[0][a] = t;
			indices[1][a] = t + 1;
			indices[2][a] = t + n;

			a++;
		}
	}

	for (int i = 1; i < n; i++) {
		for (int j = 1; j < m; j++) {
			/*	long t = i*n + j;
			indices[0][a] = t;
			indices[1][a] = t - 1;
			indices[2][a] = t - n;*/

			long t = j*n + i;
			indices[0][a] = t;
			indices[1][a] = t - 1;
			indices[2][a] = t - n;

			a++;
		}
	}
}

Fluid_0::Fluid_0(long n, long m, float d, float t, float c, float _mu, int tex)
{
	isTexed = true;
	texture = tex;
	width = n; //���  
	height = m;  //�߶�  
	long count = n * m;  //��������  

	buffer[0] = new Vector3D[count];  //������1  
	buffer[1] = new Vector3D[count];  //������2  
	renderBuffer = 0;  //��Ⱦ������  

	normal = new Vector3D[count]; //����  
	tangent = new Vector3D[count]; //����  


	for (int i = 0; i < 3; i++) {
		indices[i] = new int[2 * (n - 1)*(m - 1)];
	}
	for (int i = 0; i < 6; i++) {
		texcoords[i % 3][i / 3] = new float[2 * (n - 1)*(m - 1)];
	}

	// Precompute constants for Equation (15.25).    
	//Ԥ�ȼ��㣺������淽�̵Ķ���ʽϵ��  
	time = t;
	v = c;
	mu = _mu;
	dist = 10*d;
	float f1 = v * v * time * time / (dist * dist);
	float f2 = 1.0F / (mu * time + 2);
	k1 = (4.0F - 8.0F * f1) * f2;
	k2 = (mu * time - 2) * f2;
	k3 = 2.0F * f1 * f2;

	// Initialize buffers.    
	//��ʼ��������  
	long a = 0;

	for (long j = 0; j < m; j++)
	{
		float y = d * j;
		for (long i = 0; i < n; i++)
		{
			if (i == 0 || j == 0 || i == n - 1 || j == m - 1)buffer[0][a].Set(d * i, y, 0.0F);
			else {
				int r = rand() % 2;
				if (r == 0)buffer[0][a].Set(d * i, y, 1.0F);
				else buffer[0][a].Set(d * i, y, 0.0F);
			}
			buffer[1][a] = buffer[0][a];
			normal[a].Set(0.0F, 0.0F, 2.0F * d);
			tangent[a].Set(2.0F * d, 0.0F, 0.0F);
			a++;
		}
	}

	a = 0;
	float w = 1.0f / width;
	float h = 1.0f / height;
	for (int i = 0; i < n - 1; i++) {
		for (int j = 0; j < m - 1; j++) {
			long t = i*n + j;
			indices[0][a] = t;
			indices[1][a] = t + 1;
			indices[2][a] = t + n;

			texcoords[0][0][a] = i*w;
			texcoords[0][1][a] = j*h;

			texcoords[1][0][a] = (i + 1)*w;
			texcoords[1][1][a] = j*h;

			texcoords[2][0][a] = i*w;
			texcoords[2][1][a] = (j + 1)*h;

			a++;
		}
	}

	for (int i = 1; i < n; i++) {
		for (int j = 1; j < m; j++) {
			long t = i*n + j;
			indices[0][a] = t;
			indices[1][a] = t - 1;
			indices[2][a] = t - n;

			texcoords[0][0][a] = i*w;
			texcoords[0][1][a] = j*h;

			texcoords[1][0][a] = (i - 1)*w;
			texcoords[1][1][a] = j*h;

			texcoords[2][0][a] = i*w;
			texcoords[2][1][a] = (j - 1)*h;

			a++;
		}
	}
}

Fluid_0::~Fluid_0()
{
	delete[] tangent;
	delete[] normal;
	delete[] buffer[1];
	delete[] buffer[0];
}

void Fluid_0::Evaluate(void)
{
	// Apply Equation (15.25).    
	//����������淽��  
	for (long j = 1; j < height - 1; j++)
	{
		//��ǰ����λ��  
		const Vector3D *crnt = buffer[renderBuffer] + j * width;
		//ǰһ����λ��  
		Vector3D *prev = buffer[1 - renderBuffer] + j * width;


		for (long i = 1; i < width - 1; i++)
		{
			prev[i].z = k1 * crnt[i].z + k2 * prev[i].z +
				k3 * (crnt[i + 1].z + crnt[i - 1].z +
					crnt[i + width].z + crnt[i - width].z);
		}
	}

	//Swap buffers.  
	//����������  
	renderBuffer = 1 - renderBuffer;

	// Calculate normals and tangents.    
	//���㷨�ߺ�����  

	for (long j = 1; j < height - 1; j++)
	{
		const Vector3D *next = buffer[renderBuffer] + j * width;
		Vector3D *nrml = normal + j * width;
		Vector3D *tang = tangent + j * width;

		for (long i = 1; i < width - 1; i++)
		{
			nrml[i].x = next[i - 1].z - next[i + 1].z;
			nrml[i].y = next[i - width].z - next[i + width].z;
			tang[i].z = next[i + 1].z - next[i - 1].z;
		}
	}

}

void Fluid_0::draw()
{
	if (isTexed) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);  //ѡ������texture[status]       
		glBegin(GL_TRIANGLES);

		for (int i = 0; i < 2 * (height - 1)*(width - 1); i++) {

			glNormal3f(normal[(int)indices[0][i]].x,
				normal[(int)indices[0][i]].y,
				normal[(int)indices[0][i]].z);

			glTexCoord2f(texcoords[0][0][i], texcoords[0][1][i]);

			glVertex3f(buffer[renderBuffer][indices[0][i]].x,
				buffer[renderBuffer][indices[0][i]].y,
				buffer[renderBuffer][indices[0][i]].z);


			glNormal3f(normal[(int)indices[1][i]].x,
				normal[(int)indices[1][i]].y,
				normal[(int)indices[1][i]].z);

			glTexCoord2f(texcoords[1][0][i], texcoords[1][1][i]);
			glVertex3f(buffer[renderBuffer][indices[1][i]].x,
				buffer[renderBuffer][indices[1][i]].y,
				buffer[renderBuffer][indices[1][i]].z);

			glNormal3f(normal[(int)indices[2][i]].x,
				normal[(int)indices[2][i]].y,
				normal[(int)indices[2][i]].z);

			glTexCoord2f(texcoords[2][0][i], texcoords[2][1][i]);
			glVertex3f(buffer[renderBuffer][indices[2][i]].x,
				buffer[renderBuffer][indices[2][i]].y,
				buffer[renderBuffer][indices[2][i]].z);
		}

		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	else {
		glColor3ub(0, 200, 200);
		glBegin(GL_TRIANGLES);

		for (int i = 0; i < 2 * (height - 1)*(width - 1); i++) {

			glNormal3f(normal[(int)indices[0][i]].x,
				normal[(int)indices[0][i]].y,
				normal[(int)indices[0][i]].z);

			glVertex3f(buffer[renderBuffer][indices[0][i]].x,
				buffer[renderBuffer][indices[0][i]].y,
				buffer[renderBuffer][indices[0][i]].z);


			glNormal3f(normal[(int)indices[1][i]].x,
				normal[(int)indices[1][i]].y,
				normal[(int)indices[1][i]].z);

			glVertex3f(buffer[renderBuffer][indices[1][i]].x,
				buffer[renderBuffer][indices[1][i]].y,
				buffer[renderBuffer][indices[1][i]].z);

			glNormal3f(normal[(int)indices[2][i]].x,
				normal[(int)indices[2][i]].y,
				normal[(int)indices[2][i]].z);

			glVertex3f(buffer[renderBuffer][indices[2][i]].x,
				buffer[renderBuffer][indices[2][i]].y,
				buffer[renderBuffer][indices[2][i]].z);
		}

		glEnd();
	}
}

void Fluid_0::ChangeSpeed(float _v) {
	if (_v < 0)return;
	v = _v;
	float f1 = v * v * time * time / (dist * dist);
	float f2 = 1.0F / (mu * time + 2);
	k1 = (4.0F - 8.0F * f1) * f2;
	k2 = (mu * time - 2) * f2;
	k3 = 2.0F * f1 * f2;
}
void Fluid_0::ChangeResist(float _r) {
	if (_r < 0)return;
	mu = _r;
	float f1 = v * v * time * time / (dist * dist);
	float f2 = 1.0F / (mu * time + 2);
	k1 = (4.0F - 8.0F * f1) * f2;
	k2 = (mu * time - 2) * f2;
	k3 = 2.0F * f1 * f2;
}
