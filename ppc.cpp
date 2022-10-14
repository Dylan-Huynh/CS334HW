#include "ppc.h"
#include "m33.h"
#include "framebuffer.h"
#include <fstream>
#include <iostream>
using namespace std;

PPC::PPC(float hfov, int _w, int _h) : w(_w), h(_h) {
	//hov isa the horizontal field of view
	//initializes C at origin initializes a as x dir initializes b as y dir and w and h as given ones
	//finds fov by taking given taking given fov and turning it into radians to find c vector
	C = V3(0.0f, 0.0f, 0.0f);
	a = V3(1.0f, 0.0f, 0.0f);
	b = V3(0.0f, -1.0f, 0.0f);
	float hfovd = hfov / 180.0f * 3.1415926f;
	c = V3(-(float)w / 2.0f, (float)h / 2.0f, -(float)w / (2.0f * tan(hfovd / 2.0f)));

}


int PPC::Project(V3 P, V3 &pP) {
	// Projects the scene, given point P we look at, and a point pP we want to project on
	// sets the matrix as a direction, b direction, and c direction
	// we then invert m and multiply that by the product of the point - origin C and we dont project if the point is behind C
	// then we calculate projected point P, by dividing P points by z which is the new depth we want it at
	// This downsizes the points by a factor of z
	// Use this to find q for zbuffer interpolation
	M33 M;
	M.SetColumn(0, a);
	M.SetColumn(1, b);
	M.SetColumn(2, c);
	V3 q = M.Inverted()*(P - C);
	if (q[2] <= 0.0f)
		return 0;
	pP[0] = q[0] / q[2];
	pP[1] = q[1] / q[2];
	pP[2] = 1.0f / q[2];
	return 1;

}

V3 PPC::GetVD() {
	// this returns vector direction by finding perpendicular direction of a ^ b and normalizes it
	return (a^b).UnitVector();

}

void PPC::Rotate(V3 aDir, float theta) {
	// rotates the camera by rotating all of the camera's vectors towards a given direction, C does not change
	a = a.RotateThisVector(aDir, theta);
	b = b.RotateThisVector(aDir, theta);
	c = c.RotateThisVector(aDir, theta);

}

float PPC::GetF() {
	// Gets a dot product between c and direction vector, for focal length
	return c*GetVD();

}

void PPC::PositionAndOrient(V3 Cn, V3 L, V3 upg) {
	// Positions camera at Cn to start gets direction vd from new C and direction towards observed thing
	// Calculates new a and b by finding perpendicular vectors to vd with cross product
	// finds new c multiplying a vector by half the width, multiplying b vector by half the height, and
	// the view direction by the focal length, subtracting the vd by a and b
	V3 vdn = (L - Cn).UnitVector();
	V3 an = (vdn ^ upg).UnitVector();
	V3 bn = vdn ^ an;
	float f = GetF();
	V3 cn = vdn*f - an* (float)w / 2.0f - bn* (float)h / 2.0f;

	a = an;
	b = bn;
	c = cn;
	C = Cn;

}

void PPC::Unproject(V3 pP, V3 &P) {
	// makes original point by adding the point in projected space * their small space in the field of view, and then
	// dividing them by the projected space's z direction to make them big or small to their original size
	P = C + (a*pP[0] + b*pP[1] + c) / pP[2];

}


PPC* PPC::Interpolation(PPC* cam2, float dis)
{
	// This is supposed to make a camera in the middle of two other cameras
	// I would position them between by making the new Cn in between the two
	// I would do the same for the view directions and find L by multiplying them together
	// then find position and orient the new camera given new C and view direction
	PPC* inter = this;
	V3 Cn = this->C + (cam2->C - this->C) / 2;
	V3 upg = V3(0.0f, 1.0f, 0.0f);
	V3 vd = this->GetVD() + (cam2->GetVD() - this->GetVD()) / 2;
	V3 L = Cn + vd * dis;
	inter->PositionAndOrient(Cn, L, upg);
	return inter;
}

void PPC::RenderCamera(FrameBuffer* fb3, PPC* cam2)
{

	//Draw rectangle out of segments of a and b
	fb3->SetBGR(0xFFFFFFFF);
	fb3->SetZB(0.0f);
	fb3->Render3DSegment(cam2, this->C, this->C + this->GetVD() * 4000.0f,
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
	fb3->Render3DSegment(cam2, this->C, this->C + this->c,
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
	fb3->Render3DSegment(cam2, this->C, this->C + this->c + V3((float)w, 0, 0),
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
	fb3->Render3DSegment(cam2, this->C, this->C + this->c + V3(0, (float)h, 0),
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
	fb3->Render3DSegment(cam2, this->C, this->C + this->c + V3((float)w, (float)h, 0),
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));

	//Making photo box
	fb3->Render3DSegment(cam2, this->C + this->c, this->c + V3((float)w, 0, 0),
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
	fb3->Render3DSegment(cam2, this->C + this->c, this->c + V3(0, (float)h, 0),
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
	fb3->Render3DSegment(cam2, this->c + V3(0, (float)h, 0), this->c + V3(0, (float)h, 0) + V3((float)w, 0, 0),
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
	fb3->Render3DSegment(cam2, this->c + V3((float)w, 0, 0), this->c + V3((float)w, 0, 0) + V3(0, (float)h, 0),
		V3(1.0f, 0.0f, 0.0f), V3(0.0f, 0.0f, 0.0f));
	fb3->redraw();
	fb3->SaveAsTiff("mydbg/camera.tiff");


}

void PPC::LoadText(char* fname)
{
	// Loads a text file, but as I do not know the format of the textfile  it is prone to errors but it loads givens
	ifstream inFile;
	inFile.open(fname);
	if (!inFile) {
		cerr << "Unable to open file datafile.txt";
		return;
	}

	float hfov;
	int w;
	int h;

	//Assuming load fov followed by width and then height
	inFile >> hfov;
	inFile >> w;
	inFile >> h;

	PPC(hfov, w, h);



	inFile.close();

}

void PPC::SaveAsText(char* fname)
{
	// outputs the key elements of the camera to a text file
	ofstream outFile;
	outFile.open(fname, ios_base::app);

	outFile << "a: " << a[0] << " " << a[1] << " " << a[2] << endl;
	outFile << "b: " << b[0] << " " << b[1] << " " << b[2] << endl;
	outFile << "c: " << c[0] << " " << c[1] << " " << c[2] << endl;
	outFile << "C: " << C[0] << " " << C[1] << " " << C[2] << endl;
	outFile << "w: " << w << endl;
	outFile << "h: " << h << endl;

	outFile.close();


}

void PPC::Tranlate(int dir, float step)
{
	//given a direction to translate, change C, the other vecotrs are the same
	if (dir == 1) {
		C = a.UnitVector() * step;
	}
	else if (dir == 2) {
		C = b.UnitVector() * step;
	}
	else {
		C = (a ^ b).UnitVector() * step;
	}

}

void PPC::Zoom(float zoom)
{
	// Finds the new c given a change in focal length by a zoom variable that multiplies with old focal length
	V3 vd = GetVD();
	float f = GetF();
	float fnew = f * zoom;
	c = vd * fnew - a * (float)w / 2.0f - b * (float)h / 2.0f;

}