#include<stdlib.h>  
#include"fluid.h"  
#include"test.h"  

Fluid::Fluid(long n, long m, float d, float t, float c, float mu)
{

	width = n; //宽度  
	height = m;  //高度  
	long count = n * m;  //网格点个数  

	buffer[0] = new Vector3D[count];  //缓冲区1  
	buffer[1] = new Vector3D[count];  //缓冲区2  
	renderBuffer = 0;  //渲染缓冲区  

					   normal = new Vector3D[count]; //法线  
					   tangent = new Vector3D[count]; //切线  


	for (int i = 0; i < 3; i++) {
		indices[i] = new int[2 * (n - 1)*(m - 1)];
	}

	// Precompute constants for Equation (15.25).    
	//预先计算：流体表面方程的多项式系数  
	float f1 = c * c * t * t / (d * d);
	float f2 = 1.0F / (mu * t + 2);
	k1 = (4.0F - 8.0F * f1) * f2;
	k2 = (mu * t - 2) * f2;
	k3 = 2.0F * f1 * f2;

	// Initialize buffers.    
	//初始化缓冲区  
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

			a++;
		}
	}

	for (int i = 1; i < n; i++) {
		for (int j = 1; j < m; j++) {
			long t = i*n + j;
			indices[0][a] = t;
			indices[1][a] = t - 1;
			indices[2][a] = t - n;

			a++;
		}
	}
}

Fluid::Fluid(long n, long m, float d, float t, float c, float mu, int tex)
{
	isTexed = true;
	texture = tex;
	width = n; //宽度  
	height = m;  //高度  
	long count = n * m;  //网格点个数  

	buffer[0] = new Vector3D[count];  //缓冲区1  
	buffer[1] = new Vector3D[count];  //缓冲区2  
	renderBuffer = 0;  //渲染缓冲区  

					   normal = new Vector3D[count]; //法线  
					   tangent = new Vector3D[count]; //切线  


	for (int i = 0; i < 3; i++) {
		indices[i] = new int[2 * (n - 1)*(m - 1)];
	}
	for (int i = 0; i < 6; i++) {
		texcoords[i % 3][i / 3] = new float[2 * (n - 1)*(m - 1)];
	}

	// Precompute constants for Equation (15.25).    
	//预先计算：流体表面方程的多项式系数  
	float f1 = c * c * t * t / (d * d);
	float f2 = 1.0F / (mu * t + 2);
	k1 = (4.0F - 8.0F * f1) * f2;
	k2 = (mu * t - 2) * f2;
	k3 = 2.0F * f1 * f2;

	// Initialize buffers.    
	//初始化缓冲区  
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

Fluid::~Fluid()
{
	delete[] tangent;  
	delete[] normal;  
	delete[] buffer[1];
	delete[] buffer[0];
}

void Fluid::Evaluate(void)
{
	// Apply Equation (15.25).    
	//调用流体表面方程  
	for (long j = 1; j < height - 1; j++)
	{
		//当前顶点位移  
		const Vector3D *crnt = buffer[renderBuffer] + j * width;
		//前一顶点位移  
		Vector3D *prev = buffer[1 - renderBuffer] + j * width;

		// z(i,j,k+1) = k1 * z(i,j,k) + k2 * z(i,j,k-1) +  
		  //   k3 * (z(i+1,j,k) + z(i-1,j,k) + z(i,j+1,k) + z(i,j-1,k)  

		for (long i = 1; i < width - 1; i++)
		{
			prev[i].z = k1 * crnt[i].z + k2 * prev[i].z +
				k3 * (crnt[i + 1].z + crnt[i - 1].z +
					crnt[i + width].z + crnt[i - width].z);
		}
	}

	 //Swap buffers.  
	//交换缓冲区  
	renderBuffer = 1 - renderBuffer;

	// Calculate normals and tangents.    
	//计算法线和切线  
	
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

void Fluid::draw()
{
	if (isTexed) {
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texture);  //选择纹理texture[status]       
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

void Fluid::setColor(int type,float z) {
		if (type == 1) {//默认模式
			r = g = b = 0.8;
		}
		else if (type == 2)//根据高度来设置颜色
		{
			float c = z;
			if (c < 0 && c >= -1) { b = r = 1 + c; g = 1; }
			else if (c < -1 && c >= -3) { b = 1 - ((c + 3) / 2); g = (c + 3) / 2; r = 0; }
			else if (c < -3) { b = 1; r = g = 0; }
			else if (c > 0 && c <= 1) { r = 1; g = b = 1 - c; }
			else if (c > 1 && c <= 3) { r = 1; g = 0; b = (c - 1) / 2; }
			else if (c > 3) { r = b = 1; g = 0; }
			else { r = 1; g = 1; b = 1; }
		}
		else if (type == 3) r = g = b = (i + j) * 1.0 / (w + h);
		else r = g = b = 0.8;
		glColor4f(r, g, b, a);
	}


}