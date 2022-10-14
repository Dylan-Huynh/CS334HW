
#include "framebuffer.h"
#include "math.h"
#include <iostream>
#include <fstream>
#include <limits>
#include "scene.h"
#include "ppc.h"
#include "M33.h"

#include <tiffio.h>

using namespace std;

FrameBuffer::FrameBuffer(int u0, int v0, int _w, int _h) : 
	// initializes it with FL_GL_Window to get an OpenGL window to work with
	// inserts start u and v as top corners pixels (maybe?), and then total width and height
	// makes pixel array of size w * h to access given pixel
	// makes zb which is supposed to calculate depth of each pixels sight, this doesn't really as no interpolation is used
	// Should use the projection in PPC to find it out
	Fl_Gl_Window(u0, v0, _w, _h, 0) {

	w = _w;
	h = _h;
	pix = new unsigned int[w*h];
	zb = new float[w*h];

}


void FrameBuffer::draw() {
	// Draw using pixel array and the w and h to turn every pixel to what it is setup its color to be
	glDrawPixels(w, h, GL_RGBA, GL_UNSIGNED_BYTE, pix);

}

int FrameBuffer::handle(int event) {
	// is supposed to take an event, ie keyboard press or mouse press and make a movement
	switch (event)
	{
	case FL_KEYBOARD: {
		KeyboardHandle();
		return 0;
	}
	case FL_MOVE: {
		int u = Fl::event_x();
		int v = Fl::event_y();
		cerr << u << " " << v << "      \r";
		return 0;
	}
	default:
		return 0;
	}
	return 0;
}

void FrameBuffer::KeyboardHandle() {
	// should use this when left keyboard is pressed it will output left is pressed
	// don't think it works as of now
	int key = Fl::event_key();
	switch (key) {
	case FL_Left: {
		cerr << "INFO: pressed left" << endl;
		break;
	}
	default:
		cerr << "INFO: do not understand keypress" << endl;
		return;
	}

}

void FrameBuffer::SetBGR(unsigned int bgr) {
	// sets every pixel to a certain color
	for (int uv = 0; uv < w*h; uv++)
		pix[uv] = bgr;

}

void FrameBuffer::SetZB(float zf) {
	// sets every pixel represented in zb to a singular depth
	for (int uv = 0; uv < w*h; uv++)
		zb[uv] = zf;

}

void FrameBuffer::SetChecker(unsigned int col0, unsigned int col1, int csize) {
	// Sets a given pixel, but first it checks if the columnn and row is odd or even to make checker board background
	for (int v = 0; v < h; v++) {
		for (int u = 0; u < w; u++) {
			int cu = u / csize;
			int cv = v / csize;
			if ((cu + cv) % 2)
				Set(u, v, col0);
			else
				Set(u, v, col1);
		}
	}

}

void FrameBuffer::DrawAARectangle(V3 tlc, V3 brc, unsigned int col) {

	// entire rectangle off screen
	if (tlc[0] > (float)w)
		return;
	if (brc[0] < 0.0f)
		return;
	if (tlc[1] > (float)h)
		return;
	if (brc[1] < 0.0f)
		return;

	// rectangle partially off screen
	if (tlc[0] < 0.0f)
		tlc[0] = 0.0f;
	if (brc[0] > (float)w)
		brc[0] = (float)w;
	if (tlc[1] < 0.0f)
		tlc[1] = 0.0f;
	if (brc[1] > (float)h)
		brc[1] = (float)h;

	//for every pixel that is in the box the made the cut, it will turn to the box color
	int umin = (int)(tlc[0]+0.5f);
	int umax = (int)(brc[0]-0.5f);
	int vmin = (int)(tlc[1]+0.5f);
	int vmax = (int)(brc[1]-0.5f);
	for (int v = vmin; v <= vmax; v++) {
		for (int u = umin; u <= umax; u++) {
			Set(u, v, col);
		}
	}

}

void FrameBuffer::DrawDisk(V3 center, float r, unsigned int col) {

	//makes a center and if the distance from the pixel to the center squared is less than the radius squared it will
	// set the pixel, but guarded (effectively could use abs as well)
	int umin = (int)(center[0] - r);
	int umax = (int)(center[0] + r);
	int vmin = (int)(center[1] - r);
	int vmax = (int)(center[1] + r);
	center[2] = 0.0f;
	for (int v = vmin; v <= vmax; v++) {
		for (int u = umin; u <= umax; u++) {
			V3 pixCenter(.5f + (float)u, .5f + (float)v, 0.0f);
			V3 distVector = pixCenter - center;
			if (r*r < distVector * distVector)
				continue;
			if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
				continue;
			if (IsCloserThenSet(center[2], u, v))
				SetGuarded(u, v, col);
		}
	}


}


void FrameBuffer::Set(int u, int v, int col) {
	// Sets a given pixel to the given color
	pix[(h - 1 - v)*w + u] = col;

}


void FrameBuffer::SetGuarded(int u, int v, int col) {
	// Sets the guarded pixel only if it is within the box
	if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
		return;
	Set(u, v, col);

}

void FrameBuffer::DrawSegment(V3 p0, V3 p1, unsigned int col) {
	// Draws a segment of one color if it ids only one color
	V3 cv;
	cv.SetFromColor(col);
	DrawSegment(p0, p1, cv, cv);

}

void FrameBuffer::DrawSegment(V3 p0, V3 p1, V3 c0, V3 c1) {
	// Finds whether the segments x or y is larger, than uses that for maxSpan + 1
	float maxSpan = (fabsf(p0[0] - p1[0]) < fabsf(p0[1] - p1[1])) ?
		fabsf(p0[1] - p1[1]) : fabsf(p0[0] - p1[0]);
	int segsN = (int)maxSpan + 1;
	// Sets a starting point and starting color
	V3 currPoint = p0;
	V3 currColor = c0;
	// Makes steps pixels as it moves along the loop to change color and change which pixel it is looking at
	V3 stepv = (p1 - p0) / (float)segsN;
	V3 stepcv = (c1 - c0) / (float)segsN;
	int si;
	// in this loop, the segments are moved along the steps and truned into points to check if they are in the box
	// if they are in the box, they are set
	for (si = 0; 
		si <= segsN; 
		si++, currPoint = currPoint + stepv, currColor = currColor + stepcv) {
		int u = (int)currPoint[0];
		int v = (int)currPoint[1];
		if (u < 0 || u > w - 1 || v < 0 || v > h - 1)
			continue;
		if (IsCloserThenSet(currPoint[2], u, v))
			SetGuarded(u, v, currColor.GetColor());
	}

}


// load a tiff image to pixel buffer
void FrameBuffer::LoadTiff(char* fname) {
	TIFF* in = TIFFOpen(fname, "r");
	if (in == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}
	// a tiff file has field to set bounds and uses that to initialize all of this stuff
	int width, height;
	TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
	TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
	if (w != width || h != height) {
		w = width;
		h = height;
		delete[] pix;
		pix = new unsigned int[w*h];
		size(w, h);
		glFlush();
		glFlush();
	}

	if (TIFFReadRGBAImage(in, w, h, pix, 0) == 0) {
		cerr << "failed to load " << fname << endl;
	}

	TIFFClose(in);
}

// save as tiff image
void FrameBuffer::SaveAsTiff(char *fname) {

	//maybe one day I'll figure out how to save each char* as it's own thing
	// other than that it saves all of the fields, and then writes each pixels color into it
	TIFF* out = TIFFOpen(fname, "w");

	if (out == NULL) {
		cerr << fname << " could not be opened" << endl;
		return;
	}

	TIFFSetField(out, TIFFTAG_IMAGEWIDTH, w);
	TIFFSetField(out, TIFFTAG_IMAGELENGTH, h);
	TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);
	TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

	for (uint32 row = 0; row < (unsigned int)h; row++) {
		TIFFWriteScanline(out, &pix[(h - row - 1) * w], row);
	}

	TIFFClose(out);
}

void FrameBuffer::Render3DSegment(PPC *ppc, V3 v0, V3 v1, V3 c0, V3 c1) {
	// Renders 3-d segment to 2-d space by using the projection of the camera
	V3 pv0, pv1;
	if (!ppc->Project(v0, pv0))
		return;
	if (!ppc->Project(v1, pv1))
		return;

	DrawSegment(pv0, pv1, c0, c1);
}

void FrameBuffer::Render3DPoint(PPC *ppc, V3 p, V3 c, float psize) {
	// projects the 3-d point into the camera, and initializes its size
	V3 pv;
	if (!ppc->Project(p, pv))
		return;
	DrawDisk(pv, psize, c.GetColor());
}


int FrameBuffer::IsCloserThenSet(float currz, int u, int v) {
	// Checks if the zb is closer than the setter
	float zbz = zb[(h - 1 - v)*w + u];
	if (zbz > currz)
		return 0;
	zb[(h - 1 - v)*w + u] = currz;
	return 1;

}


void FrameBuffer::DrawTriangle(float x[3], float y[3], unsigned int col) {
	// Draws triangle first by checking the sidedness of the point equations
	// makes lines with two vertices and then checks if the 3 vertices is on the positive side of line or negative
	// these are all saved within the a b and c vectors
	float a[3]{};
	float b[3]{};
	float c[3]{};
	a[0] = y[1] - y[0];
	b[0] = -x[1] + x[0];
	c[0] = -x[0] * y[1] + y[0] * x[1];
	float sidedness;
	sidedness = a[0] * x[2] + b[0] * y[2] + c[0];
	if (sidedness < 0) {
		a[0] = -a[0]; b[0] = -b[0]; c[0] = -c[0];
	}


	a[1] = y[2] - y[1];
	b[1] = -x[2] + x[1];
	c[1] = -x[1] * y[2] + y[1] * x[2];
	sidedness = a[1] * x[0] + b[1] * y[0] + c[1];
	if (sidedness < 0) {
		a[1] = -a[1]; b[1] = -b[1]; c[1] = -c[1];
	}
	a[2] = y[0] - y[2];
	b[2] = -x[0] + x[2];
	c[2] = -x[2] * y[0] + y[2] * x[0];
	sidedness = a[2] * x[1] + b[2] * y[1] + c[2];
	if (sidedness < 0) {
		a[2] = -a[2]; b[2] = -b[2]; c[2] = -c[2];
	}

	// I did not clip the BBox, I assume that's if the box happens to end up outside of the viewing box, but with
	// set guarded I didn't need that
	// Anyways, make a box that contains the maximum pixels the triangle could have but taking the highest lowest, leftest, rightest
	// points and putting them in the box
	float bbox[2][2]{}; // for each x and y, store the min and max values
	ComputeBBox(x, y, bbox);
	int left = (int)(bbox[0][0] + .5), right = (int)(bbox[0][1] - .5);
	int top = (int)(bbox[1][0] + .5), bottom = (int)(bbox[1][1] - .5);

	int currPixX, currPixY; // current pixel considered
	float currEELS[3]{}, currEE[3]{}; // edge expression values for line starts and within line

	//walkthrough each pixel, and updates the edge so that each edge  of the triangle is also walking along the edge
	for (currPixY = top; currPixY <= bottom; currPixY++) {
		currEELS[0] = a[0] * (left + .5) + b[0] * (top + .5) + c[0];
		currEELS[0] += b[0] * (currPixY - top);
		currEELS[1] = a[1] * (left + .5) + b[1] * (top + .5) + c[1];
		currEELS[1] += b[1] * (currPixY - top);
		currEELS[2] = a[2] * (left + .5) + b[2] * (top + .5) + c[2];
		currEELS[2] += b[2] * (currPixY - top);

		for (currPixX = left; currPixX <= right; currPixX++) {

			currEE[0] = currEELS[0];
			currEE[0] += a[0] * (currPixX - left);

			currEE[1] = currEELS[1];
			currEE[1] += a[1] * (currPixX - left);


			currEE[2] = currEELS[2];
			currEE[2] += a[2] * (currPixX - left);
			// make sure that the pixel is on the right side of the triangle as the pdated edge equations should be positive
			// if the pixel matches the correct equations of the edge lines
			if (currEE[1] > 0) {
				if (currEE[2] > 0) {
					if (currEE[0] > 0) {
						SetGuarded(currPixX, currPixY, col);
					}
				}
			}
		}

	}


}

void FrameBuffer::ComputeBBox(float x[3], float y[3], float bbox[2][2]) {
	// finds the most up and left by smallest, down and right by biggest
	bbox[0][0] = smallest(x[0], x[1], x[2]);
	bbox[0][1] = biggest(x[0], x[1], x[2]);
	bbox[1][0] = smallest(y[0], y[1], y[2]);
	bbox[1][1] = biggest(y[0], y[1], y[2]);
}

float FrameBuffer::smallest(int x, int y, int z) {
	// checks each value and updates whichever if it is smaller than the current smallest
	int smallest = 99999;

	if (x < smallest)
		smallest = x;
	if (y < smallest)
		smallest = y;
	if (z < smallest)
		smallest = z;

	return smallest;
}

float FrameBuffer::biggest(int x, int y, int z)
{// checks each value and updates whichever if it is bigger than the current bigger
	int biggest = -99999;

	if (x > biggest)
		biggest = x;
	if (y > biggest)
		biggest = y;
	if (z > biggest)
		biggest = z;

	return biggest;
}

void FrameBuffer::Draw3DTriangle(PPC* ppc, V3 x, V3 y, V3 z, V3 col0, V3 col1, V3 col2) {
	M33 m33; m33.SetColumn(0, x); m33.SetColumn(1, y); m33.SetColumn(2, V3(1, 1, 1));
	V3 r = V3(col0[0], col1[0], col2[0]); V3 g = V3(col0[1], col1[1], col2[1]); V3 bcolor = V3(col0[2], col1[2], col2[2]);
	V3 rabc = m33.Inverted() * r; V3 gabc = m33.Inverted() * g; V3 babc = m33.Inverted() * bcolor;

	float a[3]{};
	float b[3]{};
	float c[3]{};
	a[0] = y[1] - y[0];
	b[0] = -x[1] + x[0];
	c[0] = -x[0] * y[1] + y[0] * x[1];
	float sidedness;
	sidedness = a[0] * x[2] + b[0] * y[2] + c[0];
	if (sidedness < 0) {
		a[0] = -a[0]; b[0] = -b[0]; c[0] = -c[0];
	}


	a[1] = y[2] - y[1];
	b[1] = -x[2] + x[1];
	c[1] = -x[1] * y[2] + y[1] * x[2];
	sidedness = a[1] * x[0] + b[1] * y[0] + c[1];
	if (sidedness < 0) {
		a[1] = -a[1]; b[1] = -b[1]; c[1] = -c[1];
	}
	a[2] = y[0] - y[2];
	b[2] = -x[0] + x[2];
	c[2] = -x[2] * y[0] + y[2] * x[0];
	sidedness = a[2] * x[1] + b[2] * y[1] + c[2];
	if (sidedness < 0) {
		a[2] = -a[2]; b[2] = -b[2]; c[2] = -c[2];
	}

	float bbox[2][2]{}; // for each x and y, store the min and max values
	ComputeBBox(x.xyz, y.xyz, bbox);
	int left = (int)(bbox[0][0] - .5), right = (int)(bbox[0][1] + .5);
	int top = (int)(bbox[1][0] - .5), bottom = (int)(bbox[1][1] + .5);

	int currPixX, currPixY; // current pixel considered

	float currEELS[3]{}, currEE[3]{}; // edge expression values for line starts and within line
	
	V3 currCol = col0;
	for (currPixY = top; currPixY <= bottom; currPixY++) {
		currEELS[0] = a[0] * (left + .5) + b[0] * (top + .5) + c[0];
		currEELS[0] += b[0] * (currPixY - top);
		currEELS[1] = a[1] * (left + .5) + b[1] * (top + .5) + c[1];
		currEELS[1] += b[1] * (currPixY - top);
		currEELS[2] = a[2] * (left + .5) + b[2] * (top + .5) + c[2];
		currEELS[2] += b[2] * (currPixY - top);

		for (currPixX = left; currPixX <= right; currPixX++) {

			currEE[0] = currEELS[0];
			currEE[0] += a[0] * (currPixX - left);

			currEE[1] = currEELS[1];
			currEE[1] += a[1] * (currPixX - left);


			currEE[2] = currEELS[2];
			currEE[2] += a[2] * (currPixX - left);
			if (currEE[1] > 0) {
				if (currEE[2] > 0) {
					if (currEE[0] > 0) {
						//Basically this calculates the same as a triangle but checks light by making sure that the 
						// closer the pixel is to a vertice, it will have more influence on the color of the pixel in 
						// a weighted average calculation
						float currZB = zb[(h - 1 - currPixY) * w + currPixX];

						SetZB(ppc, currPixX, currPixY, x, y, z);
						if (currZB == zb[(h - 1 - currPixY) * w + currPixX] && pix[(h - 1 - currPixY) * w + currPixX] != 0xFFFFFF) {
							continue;
						}
						currCol[0] = rabc[0] * currPixX + rabc[1] * currPixY + rabc[2];
						currCol[1] = gabc[0] * currPixX + gabc[1] * currPixY + gabc[2];
						currCol[2] = babc[0] * currPixX + babc[1] * currPixY + babc[2];
						SetGuarded(currPixX, currPixY, currCol.GetColor());
					}
				}
			}
		}

	}


}

void FrameBuffer::SetZB(PPC* ppc, int pixX, int pixY, V3 px, V3 py, V3 pz) {
	M33 uv1;
	uv1.SetColumn(0, px);
	uv1.SetColumn(1, py);
	uv1.SetColumn(2, V3(1, 1, 1));
	V3 abc = uv1.Inverted() * pz;

	float z = (float(pixX) * abc[0] + float(pixY) * abc[1] + abc[2]);
	if (fabs(z) > fabs(zb[(h - 1 - pixY) * w + pixX]) ) {
		
		zb[(h - 1 - pixY) * w + pixX] = z;
		//cout << z << endl;
	}
}

void FrameBuffer::DrawPhoungTriangle(PPC* ppc, V3 x, V3 y, V3 z, V3 col0, V3 col1, V3 col2, 
	V3 normal0, V3 normal1, V3 normal2, float ka, float kp, PPC* light) {
	
	M33 m33; m33.SetColumn(0, x); m33.SetColumn(1, y); m33.SetColumn(2, V3(1, 1, 1));
	V3 r = V3(col0[0], col1[0], col2[0]); V3 g = V3(col0[1], col1[1], col2[1]); V3 bcolor = V3(col0[2], col1[2], col2[2]);
	V3 rabc = m33.Inverted() * r; V3 gabc = m33.Inverted() * g; V3 babc = m33.Inverted() * bcolor;

	V3 normx = V3(normal0[0], normal1[0], normal2[0]);
	V3 normy = V3(normal0[1], normal1[1], normal2[1]);
	V3 normz = V3(normal0[2], normal1[2], normal2[2]);

	V3 normxabc = m33.Inverted() * normx; V3 normyabc = m33.Inverted() * normy; V3 normzabc = m33.Inverted() * normz;

	float a[3]{};
	float b[3]{};
	float c[3]{};
	a[0] = y[1] - y[0];
	b[0] = -x[1] + x[0];
	c[0] = -x[0] * y[1] + y[0] * x[1];
	float sidedness;
	sidedness = a[0] * x[2] + b[0] * y[2] + c[0];
	if (sidedness < 0) {
		a[0] = -a[0]; b[0] = -b[0]; c[0] = -c[0];
	}


	a[1] = y[2] - y[1];
	b[1] = -x[2] + x[1];
	c[1] = -x[1] * y[2] + y[1] * x[2];
	sidedness = a[1] * x[0] + b[1] * y[0] + c[1];
	if (sidedness < 0) {
		a[1] = -a[1]; b[1] = -b[1]; c[1] = -c[1];
	}
	a[2] = y[0] - y[2];
	b[2] = -x[0] + x[2];
	c[2] = -x[2] * y[0] + y[2] * x[0];
	sidedness = a[2] * x[1] + b[2] * y[1] + c[2];
	if (sidedness < 0) {
		a[2] = -a[2]; b[2] = -b[2]; c[2] = -c[2];
	}

	float bbox[2][2]{}; // for each x and y, store the min and max values
	ComputeBBox(x.xyz, y.xyz, bbox);
	int left = (int)(bbox[0][0] - .5), right = (int)(bbox[0][1] + .5);
	int top = (int)(bbox[1][0] - .5), bottom = (int)(bbox[1][1] + .5);

	int currPixX, currPixY; // current pixel considered

	float currEELS[3]{}, currEE[3]{}; // edge expression values for line starts and within line

	V3 currCol = col0;
	for (currPixY = top; currPixY <= bottom; currPixY++) {
		currEELS[0] = a[0] * (left + .5) + b[0] * (top + .5) + c[0];
		currEELS[0] += b[0] * (currPixY - top);
		currEELS[1] = a[1] * (left + .5) + b[1] * (top + .5) + c[1];
		currEELS[1] += b[1] * (currPixY - top);
		currEELS[2] = a[2] * (left + .5) + b[2] * (top + .5) + c[2];
		currEELS[2] += b[2] * (currPixY - top);

		for (currPixX = left; currPixX <= right; currPixX++) {

			currEE[0] = currEELS[0];
			currEE[0] += a[0] * (currPixX - left);

			currEE[1] = currEELS[1];
			currEE[1] += a[1] * (currPixX - left);


			currEE[2] = currEELS[2];
			currEE[2] += a[2] * (currPixX - left);
			if (currEE[1] > 0) {
				if (currEE[2] > 0) {
					if (currEE[0] > 0) {
						//Basically this calculates the same as a triangle but checks light by making sure that the 
						// closer the pixel is to a vertice, it will have more influence on the color of the pixel in 
						// a weighted average calculation
						float currZB = zb[(h - 1 - currPixY) * w + currPixX];

						SetZB(ppc, currPixX, currPixY, x, y, z);
						if (currZB == zb[(h - 1 - currPixY) * w + currPixX] && pix[(h - 1 - currPixY) * w + currPixX] != 0xFFFFFF) {
							continue;
						}
						currCol[0] = rabc[0] * currPixX + rabc[1] * currPixY + rabc[2];
						currCol[1] = gabc[0] * currPixX + gabc[1] * currPixY + gabc[2];
						currCol[2] = babc[0] * currPixX + babc[1] * currPixY + babc[2];
						float currnormx = normxabc[0] * currPixX + normxabc[1] * currPixY + normxabc[2];
						float currnormy = normyabc[0] * currPixX + normyabc[1] * currPixY + normyabc[2];
						float currnormz = normzabc[0] * currPixX + normzabc[1] * currPixY + normzabc[2];
						V3 currNorm = V3(currnormx, currnormy, currnormz);
						V3 pCurr = V3(float(currPixX), float(currPixY), zb[(h - 1 - currPixY) * w + currPixX]);
						V3 Curr;
						ppc->Unproject(pCurr, Curr);
						V3 lightedColor = currNorm.Light(currCol, ka, kp, (Curr - light->C), (Curr - ppc->C), currNorm);
						SetGuarded(currPixX, currPixY, lightedColor.GetColor());
					}
				}
			}
		}

	}

}