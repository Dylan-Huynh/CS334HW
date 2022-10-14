#include "tm.h"
#include "ppc.h"
#include "framebuffer.h"

using namespace std;

#include <fstream>
#include <iostream>

void TM::SetAsAACube(V3 cc, float sideLength) {
	// This does a cube by makeing the 8 vertices that a cube has by using the 8 variations a 2^3 can have for a side
	// Then it makes a mesh of them by calculating the 3 points of the triangle to be its respective vertices
	// When you do that for the six sides of the cube with two triangles for each side, you will create the cube
	vertsN = 8;
	verts = new V3[vertsN];
	float sl2 = sideLength / 2.0f;
	verts[0] = cc + V3(-sl2, +sl2, +sl2);
	verts[1] = cc + V3(-sl2, -sl2, +sl2);
	verts[2] = cc + V3(+sl2, -sl2, +sl2);
	verts[3] = cc + V3(+sl2, +sl2, +sl2);
	verts[4] = cc + V3(-sl2, +sl2, -sl2);
	verts[5] = cc + V3(-sl2, -sl2, -sl2);
	verts[6] = cc + V3(+sl2, -sl2, -sl2);
	verts[7] = cc + V3(+sl2, +sl2, -sl2);

	trisN = 12;
	tris = new unsigned int[trisN*3];
	int tri = 0;
	tris[3 * tri + 0] = 0; // tris[0] = 0;
	tris[3 * tri + 1] = 1;
	tris[3 * tri + 2] = 2;
	tri++;
	tris[3 * tri + 0] = 2;
	tris[3 * tri + 1] = 3;
	tris[3 * tri + 2] = 0;
	tri++;

	tris[3 * tri + 0] = 3;
	tris[3 * tri + 1] = 2;
	tris[3 * tri + 2] = 6;
	tri++;
	tris[3 * tri + 0] = 6;
	tris[3 * tri + 1] = 7;
	tris[3 * tri + 2] = 3;
	tri++;

	tris[3 * tri + 0] = 7;
	tris[3 * tri + 1] = 6;
	tris[3 * tri + 2] = 5;
	tri++;
	tris[3 * tri + 0] = 5;
	tris[3 * tri + 1] = 4;
	tris[3 * tri + 2] = 7;
	tri++;

	tris[3 * tri + 0] = 4;
	tris[3 * tri + 1] = 5;
	tris[3 * tri + 2] = 1;
	tri++;
	tris[3 * tri + 0] = 1;
	tris[3 * tri + 1] = 0;
	tris[3 * tri + 2] = 4;
	tri++;


	tris[3 * tri + 0] = 1;
	tris[3 * tri + 1] = 5;
	tris[3 * tri + 2] = 6;
	tri++;
	tris[3 * tri + 0] = 6;
	tris[3 * tri + 1] = 2;
	tris[3 * tri + 2] = 1;
	tri++;

	tris[3 * tri + 0] = 4;
	tris[3 * tri + 1] = 0;
	tris[3 * tri + 2] = 3;
	tri++;
	tris[3 * tri + 0] = 3;
	tris[3 * tri + 1] = 7;
	tris[3 * tri + 2] = 4;
	tri++;

	cols = new V3[vertsN];
	cols[0] =
		cols[1] =
		cols[2] =
		cols[3] = V3(1.0f, 0.0f, 0.0f);
	cols[4] =
		cols[5] =
		cols[6] =
		cols[7] = V3(0.0f, 0.0f, 0.0f);
}

#if 0
    4      7

0     3
    5      6
1     2
#endif

void TM::LoadBin(char *fname) {
		//Uses binary file reading method to read in a picture
		ifstream ifs(fname, ios::binary);
		if (ifs.fail()) {
			cerr << "INFO: cannot open file: " << fname << endl;
			return;
		}

		// Lots of checks to make sure it follows form, but if it does just load array into array
		ifs.read((char*)&vertsN, sizeof(int));
		char yn;
		ifs.read(&yn, 1); // always xyz
		if (yn != 'y') {
			cerr << "INTERNAL ERROR: there should always be vertex xyz data" << endl;
			return;
		}
		if (verts)
			delete verts;
		verts = new V3[vertsN];

		ifs.read(&yn, 1); // cols 3 floats
		normals = 0;
		if (cols)
			delete cols;
		cols = 0;
		if (yn == 'y') {
			cols = new V3[vertsN];
		}

		ifs.read(&yn, 1); // normals 3 floats
		if (normals)
			delete normals;
		normals = 0;
		if (yn == 'y') {
			normals = new V3[vertsN];
		}

		ifs.read(&yn, 1); // texture coordinates 2 floats
		float *tcs = 0; // don't have texture coordinates for now
		if (tcs)
			delete tcs;
		tcs = 0;
		if (yn == 'y') {
			tcs = new float[vertsN * 2];
		}

		// Loads the character arrays and turns them into float arrays of sixe 3
		ifs.read((char*)verts, vertsN * 3 * sizeof(float)); // load verts

		if (cols) {
			ifs.read((char*)cols, vertsN * 3 * sizeof(float)); // load cols
		}

		if (normals)
			ifs.read((char*)normals, vertsN * 3 * sizeof(float)); // load normals

		if (tcs)
			ifs.read((char*)tcs, vertsN * 2 * sizeof(float)); // load texture coordinates

	ifs.read((char*)&trisN, sizeof(int));
	if (tris)
		delete tris;
	tris = new unsigned int[trisN * 3];
	ifs.read((char*)tris, trisN * 3 * sizeof(unsigned int)); // read tiangles

	ifs.close();

	cerr << "INFO: loaded " << vertsN << " verts, " << trisN << " tris from " << endl << "      " << fname << endl;
	cerr << "      xyz " << ((cols) ? "rgb " : "") << ((normals) ? "nxnynz " : "") << ((tcs) ? "tcstct " : "") << endl;

}


void TM::RenderAsPoints(int psize, PPC *ppc, FrameBuffer *fb) {
	// Renders the verticies into the framebuffer by projhects the vertices into the camera and drawing them as a disk
	for (int vi = 0; vi < vertsN; vi++) {
		V3 pp;
		if (!ppc->Project(verts[vi], pp))
			continue;
		fb->DrawDisk(pp, (float)psize, 0xFF000000);
	}

}

void TM::RenderWireFrame(PPC *ppc, FrameBuffer *fb) {
	// Project the segments connecting the vertices and turns them into a wire frame connecting into an image once projected
	for (int tri = 0; tri < trisN; tri++) {
		for (int ei = 0; ei < 3; ei++) {
			V3 v0 = verts[tris[3 * tri + ei]];
			V3 v1 = verts[tris[3 * tri + (ei + 1) % 3]];
			V3 c0 = cols[tris[3 * tri + ei]];
			V3 c1 = cols[tris[3 * tri + (ei + 1) % 3]];
			V3 pv0, pv1;
			if (!ppc->Project(v0, pv0))
				continue;
			if (!ppc->Project(v1, pv1))
				continue;
			fb->DrawSegment(pv0, pv1, c0, c1);
		}
	}

}

V3 TM::GetCenter() {
	// uses the average distance of the vertices in space and finds the point that is the average of all of it
	V3 ret(0.0f, 0.0f, 0.0f);
	for (int vi = 0; vi < vertsN; vi++)
		ret = ret + verts[vi];
	ret = ret / (float)vertsN;
	return ret;

}

void TM::Rotate(V3 aO, V3 aD, float theta) {
	// Treats every vertices as a point and rotates all of them with rotate point function in V3
	for (int vi = 0; vi < vertsN; vi++) {
		verts[vi] = verts[vi].RotateThisPoint(aO, aD, theta);
	}

}


void TM::Translate(V3 tv) {
	// Translates the triangle mesh by moving all of its vertices by the translation matrix, maybe rewrite camera translation
	// to match this?
	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi] + tv;

}

void TM::SetCenter(V3 newCenter) {
	// makes new center and checks the difference between old and new and then translates the vertices accordingly
	V3 center = GetCenter();
	V3 tv = newCenter - center;
	Translate(tv);

}


void TM::Light(V3 matColor, float ka, V3 ld, float kp) {
	//using the normals of the vertices it sends of the vertices to undergo light calculation by the function in V3
	if (!normals) {
		cerr << "INFO: need normals" << endl;
		return;
	}
	for (int vi = 0; vi < vertsN; vi++) {
		cols[vi] = verts[vi].Light(cols[vi], ka, ld, normals[vi], kp);
	}

}




void TM::VisualizeVertexNormals(PPC *ppc, FrameBuffer *fb, float vlength) {
	//Visualize the normals by starting the normals from the vertices and then offshooting them and drawing that 
	if (!normals)
		return;
	for (int vi = 0; vi < vertsN; vi++) {
		fb->Render3DSegment(ppc, verts[vi], verts[vi] + normals[vi] * vlength,
			V3(0.0f, 0.0f, 0.0f), V3(1.0f, 0.0f, 0.0f));
	}
	fb->redraw();

}

void TM::SetUnshared(TM *tm) {
	//If the two triangle meshes dont share any intermingling space, create triangles with colors and normals that correspond
	trisN = tm->trisN;
	vertsN = tm->trisN * 3;
	verts = new V3[vertsN];
	tris = new unsigned int[trisN*3];
	if (tm->cols)
		cols = new V3[vertsN];
	if (tm->normals)
		normals = new V3[vertsN];

	for (int ti = 0; ti < tm->trisN; ti++) {
		verts[3 * ti + 0] = tm->verts[tm->tris[ti * 3 + 0]];
		verts[3 * ti + 1] = tm->verts[tm->tris[ti * 3 + 1]];
		verts[3 * ti + 2] = tm->verts[tm->tris[ti * 3 + 2]];
		if (cols) {
			cols[3 * ti + 0] = tm->cols[tm->tris[ti * 3 + 0]];
			cols[3 * ti + 1] = tm->cols[tm->tris[ti * 3 + 1]];
			cols[3 * ti + 2] = tm->cols[tm->tris[ti * 3 + 2]];
		}
		if (normals) {
			normals[3 * ti + 0] = tm->normals[tm->tris[ti * 3 + 0]];
			normals[3 * ti + 1] = tm->normals[tm->tris[ti * 3 + 1]];
			normals[3 * ti + 2] = tm->normals[tm->tris[ti * 3 + 2]];
		}
		tris[3 * ti + 0] = 3 * ti + 0;
		tris[3 * ti + 1] = 3 * ti + 1;
		tris[3 * ti + 2] = 3 * ti + 2;
	}


}

void TM::SetAsCopy(TM* tm) {
	// Makes a copy of the given triangle mesh, with vertices, cols, and normals
	vertsN = tm->vertsN;
	trisN = tm->trisN;
	verts = new V3[vertsN];
	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = tm->verts[vi];
	tris = new unsigned int[3 * trisN];
	for (int ti = 0; ti < trisN * 3; ti++)
		tris[ti] = tm->tris[ti];
	if (tm->cols) {
		cols = new V3[vertsN];
		for (int vi = 0; vi < vertsN; vi++)
			cols[vi] = tm->cols[vi];
	}
	if (tm->normals) {
		normals = new V3[vertsN];
		for (int vi = 0; vi < vertsN; vi++)
			normals[vi] = tm->normals[vi];
	}

}

void TM::Explode(float t) {
	// explodes the mesh by shooting the traiangles out by their normal direction outwards by t
	for (int ti = 0; ti < trisN; ti++) {
		unsigned int inds[3];
		inds[0] = tris[3 * ti + 0];
		inds[1] = tris[3 * ti + 1];
		inds[2] = tris[3 * ti + 2];
		// v = (v1-v0) ^ (v2-v0)
		V3 n = (verts[inds[1]] - verts[inds[0]]) ^
			(verts[inds[2]] - verts[inds[0]]);
		n = n.UnitVector();
		verts[inds[0]] = verts[inds[0]] + n * t;
		verts[inds[1]] = verts[inds[1]] + n * t;
		verts[inds[2]] = verts[inds[2]] + n * t;
	}
}


void TM::Scale(float mag) {
	// Increases the distances of the vertices by a magnitude of all of them, probably should set a new center,
	// normals should stay the same
	for (int vi = 0; vi < vertsN; vi++)
		verts[vi] = verts[vi] * mag;

}

void TM::RenderTriangles(PPC* ppc, FrameBuffer* fb) {
	// Projects all of the triangles onto the camera and then draws them onto the framebuffer
	for (int tri = 0; tri < trisN; tri++) {
		V3 v0 = verts[tris[3 * tri + 0]];
		V3 v1 = verts[tris[3 * tri + 1]];
		V3 v2 = verts[tris[3 * tri + 2]];
		V3 r;
		r[0] = (v0[2] - ppc->C[2]);
		r[1] = (v1[2] - ppc->C[2]);
		r[2] = (v2[2] - ppc->C[2]);
		V3 pv0, pv1, pv2;
		if (!ppc->Project(v0, pv0))
			continue;
		if (!ppc->Project(v1, pv1))
			continue;
		if (!ppc->Project(v2, pv2))
			continue;
		
		V3 col0 = cols[tris[3 * tri + 0]];
		V3 col1 = cols[tris[3 * tri + 1]];
		V3 col2 = cols[tris[3 * tri + 2]];



		fb->Draw3DTriangle(ppc, V3(pv0[0], pv1[0], pv2[0]),
			V3(pv0[1], pv1[1], pv2[1]), V3(pv0[2], pv1[2], pv2[2]), col0, col1, col2);
	}
}



void TM::Light(V3 matColor, float ka, PPC* ppc) {
	//New light function that is incomplete
	for (int vi = 0; vi < vertsN; vi++) {
		V3 center = GetCenter();
		cols[vi] = (verts[vi] - center).Light(cols[vi], ka, ppc->GetVD(), normals[vi]);
	}
}

void TM::SM2(PPC* light, PPC* ppc, float ka, float kp) {
	

	for (int vi = 0; vi < vertsN; vi++) {
		V3 center = GetCenter();
		float diffuse = ((light->GetVD() - verts[vi]).UnitVector() * -1) * normals[vi].UnitVector();
		diffuse = (diffuse < 0.0f) ? 0.0f : diffuse;
		V3 reflect = (light->GetVD()-verts[vi]).UnitVector() - normals[vi].UnitVector() * (2 * (light->GetVD() * normals[vi].UnitVector()));
		float specular = pow((ppc->GetVD()).UnitVector() * reflect.UnitVector(), kp);
		cols[vi] = cols[vi] * (ka + (1.0f - ka) * diffuse + specular);
	}
}

void TM::SM3(PPC* light, PPC* ppc, FrameBuffer *fb, float ka, float kp) {


	for (int tri = 0; tri < trisN; tri++) {
		V3 v0 = verts[tris[3 * tri + 0]];
		V3 v1 = verts[tris[3 * tri + 1]];
		V3 v2 = verts[tris[3 * tri + 2]];
		V3 r;
		r[0] = (v0[2] - ppc->C[2]);
		r[1] = (v1[2] - ppc->C[2]);
		r[2] = (v2[2] - ppc->C[2]);
		V3 pv0, pv1, pv2;
		if (!ppc->Project(v0, pv0))
			continue;
		if (!ppc->Project(v1, pv1))
			continue;
		if (!ppc->Project(v2, pv2))
			continue;

		V3 col0 = cols[tris[3 * tri + 0]];
		V3 col1 = cols[tris[3 * tri + 1]];
		V3 col2 = cols[tris[3 * tri + 2]];

		V3 normal0 = normals[tris[3 * tri + 0]];
		V3 normal1 = normals[tris[3 * tri + 1]];
		V3 normal2 = normals[tris[3 * tri + 2]];


		fb->DrawPhoungTriangle(ppc, V3(pv0[0], pv1[0], pv2[0]),
			V3(pv0[1], pv1[1], pv2[1]), V3(pv0[2], pv1[2], pv2[2]), col0, col1, col2,
			normal0, normal1, normal2, ka, kp, light);
	}

}