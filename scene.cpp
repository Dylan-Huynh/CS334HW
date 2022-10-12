#include "scene.h"

#include "V3.h"
#include "M33.h"
#include "tm.h"

Scene *scene;

using namespace std;

#include <iostream>
#include <fstream>

float ka = .03f;
float kp = 1.0f;

Scene::Scene() {

	gui = new GUI();
	gui->show();

	int u0 = 60;
	int v0 = 80;
	int h = 400;
	int w = 600;
	fb = new FrameBuffer(u0, v0, w, h);
	fb->position(u0, v0);
	fb->label("first person");
	fb->show();
	fb->SetBGR(0xFF0000FF);
	fb->redraw();

	//Moves the 3rd person camera u0 * 2 + width of ramebuffer
	fb3 = new FrameBuffer(u0, v0, w, h);
	fb3->position(u0+fb->w+u0, v0);
	fb3->label("3rd person");
	fb3->show();
	fb3->SetBGR(0xFFFF0000);
	fb3->redraw();

	// makes two cameras but moves the 3rd person camera somewhere else
	float hfov = 60.0f;
	ppc = new PPC(hfov, w, h);
	ppc3 = new PPC(hfov, w, h);
	ppc3->C = V3(-200.0f, 200.0f, 120.0f);

	tmsN = 1;
	tms = new TM[tmsN];
	TM loadedTM;
	loadedTM.LoadBin("geometry/teapot1k.bin");
	tms[0].SetUnshared(&loadedTM);
//	tms[0].LoadBin("geometry/teapot1k.bin");
//	tms[0].LoadBin("geometry/teapot57k.bin");
	tms[0].SetCenter(V3(0.0f, 0.0f, -150.0f));

	ppc3->PositionAndOrient(ppc3->C, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));

	gui->uiw->position(u0, v0 + fb->h + v0);

}

void Scene::Render(PPC *rppc, FrameBuffer *rfb) {
	// Renders triangle mesh as wire frame and triangle frame functions
	rfb->SetBGR(0xFFFFFFFF);
	rfb->SetZB(0.0f);
	for (int tmi = 0; tmi < tmsN; tmi++) {
		tms[tmi].RenderWireFrame(rppc, rfb);
		tms[tmi].RenderTriangles(rppc, rfb);
	}
	
	rfb->redraw();

}

void Scene::Render() {
	// prelude to other render function with the cameras made after
	// first person
	Render(ppc, fb);
	// third person
	Render(ppc3, fb3);
	fb3->Render3DSegment(ppc3, ppc->C, ppc->C + ppc->GetVD()*100.0f, 
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
	fb3->Render3DPoint(ppc3, ppc->C, V3(1.0f, 0.0f, 0.0f), 7.0f);
	fb3->Render3DPoint(ppc, ppc->C, V3(1.0f, 0.0f, 0.0f), 7.0f);
	fb3->redraw();

}

void Scene::DBG() {

	{
		TM loadedTM;
		loadedTM.LoadBin("geometry/teapot1k.bin");
		loadedTM.SetAsCopy(&(tms[0]));
		for (int fi = 0; fi < 1000; fi++) {
			Render();
			Fl::check();
		}
		return;
	}

	{
		TM tm;
		tm.SetAsCopy(&(tms[0]));
		tms[0].SetUnshared(&tm);
		for (int fi = 0; fi < 1000; fi++) {
			Render();
			Fl::check();
			tms[0].Explode(0.1f);
		}
		return;
	}

	/* {
		Render();
		float vlength = 10.0f;
		tms[0].VisualizeVertexNormals(ppc, fb, vlength);
		tms[0].VisualizeVertexNormals(ppc3, fb3, vlength);
		return;

	} */

	{
		V3 matColor = *tms[0].cols;
		float ka = 0.03f;
		V3 lv = ppc->GetVD();
		V3 center = tms[0].GetCenter();
		for (int fi = 0; fi < 360; fi++) {
			tms[0].Light(matColor, ka, lv, kp);
			Render();
			fb3->Render3DSegment(ppc, center - lv *50.0f, center, V3(1.0f, 1.0f, 0.0f),
				V3(1.0f, 0.0f, 0.0f));
			Fl::check();
			lv = lv.RotateThisVector(V3(1.0f, 0.0f, 0.0f), 1.0f);

		}
		return;
	}

	{

		V3 P0(-5.0f, 0.0f, -10.0f);
		V3 P1(+100.0f, 0.0f, -200.0f);
		V3 c0(1.0f, 0.0f, 0.0f);
		V3 c1(0.0f, 0.0f, 0.0f);
		ppc->PositionAndOrient(V3(0.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, -10.0f), V3(0.0f, 1.0f, 0.0f));
		fb->SetBGR(0xFFFFFFFF);
		fb->Render3DSegment(ppc, P0, P1, c0, c1);
		fb->redraw();
		ppc3->PositionAndOrient(V3(0.0f, 500.0f, 100.0f), V3(0.0f, 0.0f, 0.0f), V3(0.0f, 1.0f, 0.0f));
		fb3->SetBGR(0xFFFFFFFF);
		fb3->Render3DSegment(ppc3, P0, P1, c0, c1);
		int stepsN = 4;
		for (int si = 0; si <= stepsN; si++) {
			V3 currp = P0 + (P1 - P0) * (float) si / (float)stepsN;
			V3 currc = c0 + (c1 - c0) * (float) si / (float)stepsN;
			fb->Render3DPoint(ppc, currp, currc, 9.0f);
			fb3->Render3DPoint(ppc3, currp, currc, 9.0f);
		}
		V3 P0p, P1p;
		ppc->Project(P0, P0p);
		ppc->Project(P1, P1p);
		float w0 = 1.0f / P0p[2];
		float w1 = 1.0f / P1p[2];
		for (int si = 0; si <= stepsN; si++) {
			float frac = (float)si / (float)stepsN;
			V3 currp = P0p + (P1p - P0p) * frac;
			float currw = w0 + (w1 - w0)*frac;
			fb->DrawDisk(currp, 5.0f, 0xFF00FF00);
			V3 ucurrp;
			ppc->Unproject(currp, ucurrp);
			fb3->Render3DPoint(ppc3, ucurrp, V3(0.0f, 1.0f, 0.0f), 5.0f);
			currp[2] = 1.0f / currw;
			ppc->Unproject(currp, ucurrp);
			fb3->Render3DPoint(ppc3, ucurrp, V3(0.0f, 0.0f, 1.0f), 5.0f);
		}
		fb->redraw();
		fb3->redraw();
		return;
	}

	{
		for (int fi = 0; fi < 100; fi++) {
			Render();
			Fl::check();
			ppc->C = ppc->C + V3(0.0f, 0.5f, 0.0f);
			ppc->PositionAndOrient(ppc->C, tms[0].GetCenter(), V3(0.0f, 1.0f, 0.0f));
		}
		return;
	}

	{
		V3 cc(0.0f, 0.0f, -100.0f);
		float cubeSideLength = 30.0f;
		TM tm;
//		tm.SetAsAACube(cc, cubeSideLength);
		tm.LoadBin("geometry/teapot57k.bin");
		tm.SetCenter(V3(0.0f, 0.0f, -150.0f));
		int psize = 5;
		V3 Cn(ppc->C);
		V3 L = tm.GetCenter();
		V3 upg(0.0f, 1.0f, 0.0f);
		PPC ppc0(*ppc);
		int fN = 36;
//		fN = 1;
		for (int fi = 0; fi < fN; fi++) {
			fb->SetBGR(0xFFFFFFFF);
			fb->SetZB(0.0f);
//			tm.RenderAsPoints(psize, ppc, fb);
			tm.RenderWireFrame(ppc, fb);
			fb->redraw();
			Fl::check();
			float tstep = 0.1f;
			V3 tv = ppc->GetVD()*tstep;
			tv = ppc->a*tstep;
			V3 center = tm.GetCenter();
			ppc->Rotate(ppc->b.UnitVector()*-1.0f, 0.01f);
//			tm.Rotate(center, V3(0.0f, 1.0f, 0.0f), 0.1f);
//			ppc->C = ppc->C + tv;
			ppc->PositionAndOrient(Cn, L, upg);
			Cn = Cn + V3(0.0f, 5.0f, 0.0f);			
		}
		*ppc = ppc0;
		return;
	}
	{
		V3 P(0.0f, 0.0f, -100.0f);
		V3 pP;
		ppc->Project(P, pP);
		cerr << pP << endl;
		return;
	}

	{

		fb->SetBGR(0xFF000000);
		fb->SaveAsTiff("mydbg/blackImage.tif");
		fb->SetBGR(0xFF00FF00);
		fb->redraw();
		Fl::check();
		char c;
		cin >> c;
		fb->LoadTiff("mydbg/redImage.tif");
		fb->redraw();
		return;
	}

	{

		V3 p0(20.0f, 430.0f, 0.0f);
		V3 p1(520.0f, 331.0f, 0.0f);
		fb->SetBGR(0xFFFFFFFF);
		fb->DrawSegment(p0, p1, 0xFF000000);
		fb->redraw();
		return;

	}

	{
		V3 tlc(100.5f, 45.5f, 0.0f);
		V3 brc(300.5f, 145.5f, 0.0f);
		unsigned int col = 0xFF0000FF;
		unsigned int colDisk = 0xFFFFFF00;
		V3 center(114.1f, 300.1f, 0.0f);
		float r = 110.0f;
		int fN = 1000;
		float du = 1.f;
		for (int fi = 0; fi < fN; fi++) {
			fb->SetBGR(0xFFFFFFFF);
			fb->DrawAARectangle(tlc, brc, col);
//			fb->DrawDisk(center, r, colDisk);
			fb->redraw();
			Fl::check();
			tlc[0] += du;
			brc[0] += du;
			center[0] += du;
		}
		return;
	}

	{
		M33 m;
		V3 v(1.0f, 2.0f, 3.0f);
		m[0] = v;
		m[1] = v;
		m[2] = v;
		V3 ret = m*v;
		cerr << "mv: " << ret << endl;
		return;
	}
	{
		V3 v(1.0f, 2.0f, 3.0f);
		cerr << "v: " << v << endl;
		return;
	}

	cerr << "INFO: pressed DBG button on GUI" << endl;

	{
		V3 v0(1.0f, 2.0f, 3.0f);
		V3 v1(-1.0f, -2.0f, -3.0f);
		V3 v01 = v0 + v1;
		cerr << "v0 + v1: " << v01[0] << " " << v01[1] << " " << v01[2] << endl;
		return;
	}

	return;

	unsigned int col0 = 0xFF000000;
	unsigned int col1 = 0xFFFFFFFF;
	int csize = 100;
	fb->SetChecker(col0, col1, csize);
	fb->redraw();
}

void Scene::SM2() {
	{
		TM loadedTM;
		loadedTM.LoadBin("geometry/teapot1k.bin");
		V3 matColor = *loadedTM.cols;
		V3 lv = ppc->GetVD();
		V3 center = tms[0].GetCenter();
		for (int fi = 0; fi < 360; fi++) {
			tms[0].Light(matColor, ka, ppc);
			Render();
			fb3->Render3DSegment(ppc, center - lv * 50.0f, center, V3(1.0f, 1.0f, 0.0f),
				V3(1.0f, 0.0f, 0.0f));
			fb3->Render3DPoint(ppc3, ppc->C, V3(1.0f, 1.0f, 0), 7.0f);
			Fl::check();
			lv = lv.RotateThisVector(V3(1.0f, 0.0f, 0.0f), 1.0f);

		}
	}
}

void Scene::SM3() {
	{
		TM loadedTM;
		loadedTM.LoadBin("geometry/teapot1k.bin");
		V3 matColor = *loadedTM.cols;
		V3 lv = ppc->GetVD();
		V3 center = tms[0].GetCenter();
		for (int fi = 0; fi < 360; fi++) {
			tms[0].Light(matColor, ka, lv, kp);
			Render();
			fb3->Render3DSegment(ppc, center - ppc->GetVD() * 50.0f, center, V3(1.0f, 1.0f, 0.0f),
				V3(1.0f, 0.0f, 0.0f));
			Fl::check();
			lv = lv.RotateThisVector(V3(1.0f, 0.0f, 0.0f), 1.0f);

		}
		return;
	}
}


void Scene::NewButton() {
	cerr << "INFO: pressed New button on GUI" << endl;
}

void Scene::Left() {
	V3 Cn(ppc->C);
	Cn = Cn + V3(-5.0f, 0.0f, 0.0f);
	V3 L = tms[0].GetCenter();
	V3 upg(0.0f, 1.0f, 0.0f);
	ppc->PositionAndOrient(Cn, L, upg);
}
void Scene::Up() {
	V3 Cn(ppc->C);
	Cn = Cn + V3(0.0f, -5.0f, 0.0f);
	V3 L = tms[0].GetCenter();
	V3 upg(0.0f, 1.0f, 0.0f);
	ppc->PositionAndOrient(Cn, L, upg);
}
void Scene::Down() {
	V3 Cn(ppc->C);
	Cn = Cn + V3(0.0f, 5.0f, 0.0f);
	V3 L = tms[0].GetCenter();
	V3 upg(0.0f, 1.0f, 0.0f);
	ppc->PositionAndOrient(Cn, L, upg);
}
void Scene::Right() {
	V3 Cn(ppc->C);
	Cn = Cn + V3(5.0f, 0.0f, 0.0f);
	V3 L = tms[0].GetCenter();
	V3 upg(0.0f, 1.0f, 0.0f);
	ppc->PositionAndOrient(Cn, L, upg);
}

void Scene::Ambient() {
	ka += .1f;
}

void Scene::AmbientDown() {
	ka -= .1f;
}

void Scene::WhiteUp() {
	kp += .1f;
}

void Scene::WhiteDown() {
	kp += .1f;
}