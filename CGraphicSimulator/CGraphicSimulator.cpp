// GoldFaker.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <windows.h>
#include "svpng.h"
using namespace std;

/*!
	\brief 以 PNG 格式存储 RGB/RGBA 影像
	\param out 输出流（缺省使用 FILE*）。
	\param w 影像宽度。(<16383)
	\param h 影像高度。
	\param img 影像像素数据，内容为 24 位 RGB 或 32 位 ARGB 格式。
	\param alpha 影像是否含有 alpha 通道。
*/
const int radius = 50;

bool DrawCircle(int x, int y) {
	return (x * x + y * y) < (radius * radius);
}

#define MAX_STEP 10
#define MAX_DISTANCE 2.0f
#define EPSILON 1e-6f

float circleSDF(float x, float y, float cx, float cy, float r) {
	float ux = x - cx;
	float uy = y - cy;
	return sqrtf(ux * ux + uy * uy) - r;
}

float trace(float ox, float oy, float dx, float dy) {
	float t = 0.0f;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		float sd = circleSDF(ox + dx * t, oy + dy * t, 0.1f, 0.1f, 0.1f);
		if (sd < EPSILON) {
			return 2.0f;
		}
		t += sd;
	}
	return 0.0f;
}

float trace2(float ox, float oy, float dx, float dy) {
	float t = 0.0f;
	for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
		float sd = circleSDF(ox + dx * t, oy + dy * t, 0.5f, 0.5f, 0.1f);
		if (sd < EPSILON) {
			return 2.0f;
		}
		t += sd;
	}
	return 0.0f;
}

#define TWO_PI 6.28318530718f
#define N 64 //采样次数

///
float sample(float x, float y) {
	float sum = 0.0f;
	for (int i = 0; i < N; i++) {
		//float a = TWO_PI * rand() / RAND_MAX; //随机均匀采样
		//float a = TWO_PI * i / N;	//分层采样
		float a = TWO_PI * ((float)rand() / RAND_MAX + i) / N;	//抖动采样
		sum += trace(x, y, cosf(a), sinf(a));
		sum += trace2(x, y, cosf(a), sinf(a));
	}
	return sum / N;
}

#define W 512
#define H 512

void test_rgb(void) {
	unsigned char rgb[W * H * 3], *p = rgb;
	unsigned x, y;
	FILE *fp = fopen("rgb.png", "wb");
	for (y = 0; y < H; y++)
		for (x = 0; x < W; x++, p += 3) {
			//*p++ = (unsigned char)x;    /* R */
			//*p++ = (unsigned char)y;    /* G */ 
			//*p++ = 128;                 /* B */
			//if (DrawCircle(x - 128, y - 128)) {
			//	*p++ = 255;
			//	*p++ = 255;
			//	*p++ = 255;
			//}
			//else {
			//	*p++ = 0;
			//	*p++ = 0;
			//	*p++ = 0;
			//}
			p[0] = p[1] = p[2] = (int)(fminf(sample((float)x / W, (float)y / H) * 255.0f, 255.0f));
		}
	svpng(fp, W, H, rgb, 0);
	fclose(fp);
}

int main(int count, char* input_parameters[])
{
	test_rgb();
}

