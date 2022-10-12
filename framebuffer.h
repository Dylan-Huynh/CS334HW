#pragma once

#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <GL/glut.h>

#include "tm.h"
#include "V3.h"

class PPC;

class FrameBuffer : public Fl_Gl_Window {
public:
	unsigned int *pix; // pixel array
	float *zb; // z buffer to resolve visibility
	int w, h;
	FrameBuffer(int u0, int v0, int _w, int _h);
	void draw();
	void KeyboardHandle();
	int handle(int guievent);
	void SetBGR(unsigned int bgr);
	void Set(int u, int v, int col);
	void SetGuarded(int u, int v, int col);
	void SetChecker(unsigned int col0, unsigned int col1, int csize);
	void DrawAARectangle(V3 tlc, V3 brc, unsigned int col);
	void DrawDisk(V3 center, float r, unsigned int col);
	void DrawSegment(V3 p0, V3 p1, unsigned int col);
	void DrawSegment(V3 p0, V3 p1, V3 c0, V3 c1);
	void Render3DSegment(PPC *ppc, V3 v0, V3 v1, V3 c0, V3 c1);
	void Render3DPoint(PPC *ppc, V3 p, V3 c, float psize);
	void LoadTiff(char* fname);
	void SaveAsTiff(char* fname);
	void SetZB(float zf);
	int IsCloserThenSet(float currz, int u, int v);

	void DrawTriangle(float x[3], float y[3], unsigned int col);
	void ComputeBBox(float x[3], float y[3], float bbox[2][2]);
	float smallest(int x, int y, int z);
	float biggest(int x, int y, int z);
	
	void FrameBuffer::SetZB(PPC* ppc, int pixX, int pixY, V3 px, V3 py, V3 pz);
	void Draw3DTriangle(PPC *ppc, V3 x, V3 y, V3 r, V3 col0, V3 col1, V3 col2); //3-d to 2-d triangle rasterization
};