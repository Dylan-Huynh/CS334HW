#pragma once

#include "V3.h"

class PPC;
class FrameBuffer;

class TM {
public:
	V3 *verts;
	int vertsN;
	V3 *cols, *normals;
	unsigned int *tris;
	int trisN;
	TM() : verts(0), vertsN(0), tris(0), trisN(0), cols(0), normals(0) {};
	void SetAsAACube(V3 cc, float sideLength);
	void LoadBin(char *fname);
	void RenderAsPoints(int psize, PPC *ppc, FrameBuffer *fb);
	void RenderWireFrame(PPC *ppc, FrameBuffer *fb);
	V3 GetCenter();
	void Translate(V3 tv);
	void SetCenter(V3 newCenter);
	void Rotate(V3 aO, V3 aD, float theta);
	void Light(V3 matColor, float ka, V3 ld, float kp); //Light Source
	void VisualizeVertexNormals(PPC *ppc, FrameBuffer *fb, float vlength);
	void SetUnshared(TM *tm);
	void SetAsCopy(TM* tm);
	void Explode(float t);

	void Scale(float mag);
	void RenderTriangles(PPC* ppc, FrameBuffer* fb);
	void Light(V3 matColor, float ka, PPC* ppc);
};