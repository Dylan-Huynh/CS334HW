#pragma once

#include "v3.h"
#include "framebuffer.h"

class PPC {
public:
	V3 a, b, c, C;
	int w, h;
	PPC(float hfov, int _w, int _h);
	int Project(V3 P, V3 &pP);
	void Unproject(V3 pP, V3 &P);
	V3 GetVD();
	void Rotate(V3 aDir, float theta);
	void PositionAndOrient(V3 Cn, V3 L, V3 upg);
	float GetF();

	void Tranlate(int dir, float step);
	void Zoom(float zoom);
	PPC* Interpolation(PPC *cam2, float N); //not implemented
	void RenderCamera(FrameBuffer *fb, PPC *cam2); //not implemented
	void LoadText(char* fname); //not implemented
	void SaveAsText(char* fname); // not implemented

};