#include <stdlib.h>	// for rand()
#include"Tools.h"

static float table[][2] = {
	{ .00f, .5f },
	{ .01f, .504f },
	{ .02f, .508f },
	{ .03f, .512f },
	{ .06f, .5239f },
	{ .08f, .5319f },
	{ .11f, .5438f },
	{ .13f, .5517f },
	{ .16f, .5636f },
	{ .18f, .5714f },
	{ .21f, .5832f },
	{ .23f, .5910f },
	{ .26f, .6026f },
	{ .28f, .6103f },
	{ .31f, .6217f },
	{ .34f, .6331f },
	{ .36f, .6406f },
	{ .39f, .6517f },
	{ .42f, .6628f },
	{ .44f, .6700f },
	{ .47f, .6808f },
	{ .50f, .6915f },
	{ .53f, .7019f },
	{ .56f, .7123f },
	{ .59f, .7224f },
	{ .62f, .7324f },
	{ .65f, .7422f },
	{ .68f, .7517f },
	{ .71f, .7611f },
	{ .74f, .7703f },
	{ .78f, .7823f },
	{ .81f, .7910f },
	{ .85f, .8023f },
	{ .88f, .8106f },
	{ .92f, .8212f },
	{ .96f, .8315f },
	{ 1.0f, .8413f },
	{ 1.04f, .8508f },
	{ 1.09f, .8621f },
	{ 1.13f, .8708f },
	{ 1.18f, .8810f },
	{ 1.23f, .8907f },
	{ 1.29f, .9015f },
	{ 1.35f, .9115f },
	{ 1.41f, .9207f },
	{ 1.48f, .9306f },
	{ 1.56f, .9406f },
	{ 1.65f, .9505f },
	{ 1.76f, .9608f },
	{ 1.89f, .9706f },
	{ 2.06f, .9803f },
	{ 2.33f, .9901f },
	{ 99.0f, 1.0f } };


float randnormal(float mean, float stdev)  
{
	int i = 0;
	float u = rand() / (float)RAND_MAX;
	float n;

	if (u >= 0.5) {
		while (u > table[i][1]) {
			i++;
		}
		n = table[i - 1][0];
	}
	else {
		u = 1 - u;
		while (u > table[i][1]) {
		i++;
	}
	n = 1 - table[i - 1][0];
	}
	return (mean + stdev * n);
}

int normalizeF(Vector3D in, Vector3D out)
{
	int t = 0;
	float l = 0.0;
	float thresh = 0.00000003;
	float and = in.x + in.y + in.z;
	if (and<thresh && and>-thresh) {
		return 1;
	}

	l = in.x*in.x + in.y*in.y + in.z*in.z;
	//如果长度太小了，就适当放大坐标分量
	if (l < 0.0000001) {
		l = 0.0;
		in.x *= 10000.0;
		in.y *= 10000.0;
		in.z *= 10000.0;
		l = in.x*in.x + in.y*in.y + in.z*in.z;
	}

	l = sqrt(l);
	out.x = in.x / l;
	out.y = in.y / l;
	out.z = in.z / l;

	return 0;
}

