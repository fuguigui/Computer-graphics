#pragma once

#define GLUT_DISABLE_ATEXIT_HACK        
#include <glut.h>        

#include "VectorClasses.h"

float randnormal(float mean, float stdev);

int normalizeF(Vector3D, Vector3D);
void loadTex(int i, char *filename, GLuint* texture);

void *file_contents(const char *filename, GLint *length);
void *read_tga(const char *filename, int *width, int *height);
