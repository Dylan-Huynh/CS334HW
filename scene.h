#pragma once

#include "gui.h"
#include "framebuffer.h"
#include "ppc.h"
#include "tm.h"

class Scene {
public:

	GUI *gui;
	FrameBuffer *fb, *fb3;
	PPC *ppc, *ppc3;
	TM *tms;
	int tmsN;
	Scene();
	void Render(PPC *renderPPC, FrameBuffer *renderFB);
	void Render();
	void DBG();
	void SM2();
	void SM3();
	void NewButton();
	void Left();
	void Up();
	void Down();
	void Right();
	void Ambient();
	void AmbientDown();
	void WhiteUp();
	void WhiteDown();
};

extern Scene *scene;