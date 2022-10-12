
#include "V3.h"
#include "M33.h"

using namespace std;

#include <ostream>
#include <iostream>

V3::V3(float x, float y, float z) {
	//saves all of these as floats
	xyz[0] = x;
	xyz[1] = y;
	xyz[2] = z;

}

V3 V3::operator+(V3 v1) {
	//creates a vector to return based of the this vector and adds each element
	V3 ret;
	V3 &v0 = *this;
	ret[0] = v0[0] + v1[0];
	ret[1] = v0[1] + v1[1];
	ret[2] = v0[2] + v1[2];
	return ret;

}

V3 V3::operator-(V3 v1) {
	//creates a vector to return based of the this vector and subtracts each element
	V3 ret;
	V3 &v0 = *this;
	ret[0] = v0[0] - v1[0];
	ret[1] = v0[1] - v1[1];
	ret[2] = v0[2] - v1[2];
	return ret;

}

float& V3::operator[](int i) {
	//calls for array element and returns that
	return xyz[i];

}

float V3::operator*(V3 v1) {
	//dot product of which x * x + y * y + z * z
	V3 &v0 = *this;
	return v0[0] * v1[0] + v0[1] * v1[1] + v0[2] * v1[2];
}

ostream& operator<<(ostream& ostr, V3 v) {
	//c++ print statement, no endl weird
	return ostr << v[0] << " " << v[1] << " " << v[2];

}

float V3::Length() {
	//uses dot product to get square roots of each element and square roots it all
	V3 &v = *this;
	return sqrtf(v*v);

}

V3 V3::UnitVector() {
	//divides vector by the length to get normal
	V3 ret = *this;
	ret = ret / Length();
	return ret;
}


V3 V3::operator/(float scval) {
	//divides this vector's elements by a float and returns the result
	V3 ret = *this;
	ret[0] /= scval;
	ret[1] /= scval;
	ret[2] /= scval;
	return ret;

}

V3 V3::operator*(float scval) {
	//multiplies this vector's elements by a float and returns the result
	V3 ret = *this;
	ret[0] *= scval;
	ret[1] *= scval;
	ret[2] *= scval;
	return ret;

}

V3 V3::operator^(V3 v2) {
	//cross producr, starts by labeling all alements of both vectors into their respectives names in the formula
	//and completes equation, y1*z2 - z1*y2, z1*x2 - x1*z2, x1*y2 - y1*x2, though I like the second element as
	// -(x1*z2 - z1 * x2) for continuity, btw its found by the determinant of the remaining elements when the specific is taken out
	V3 &v1 = *this;
	float x1 = v1[0];
	float y1 = v1[1];
	float z1 = v1[2];
	float x2 = v2[0];
	float y2 = v2[1];
	float z2 = v2[2];
	return V3(y1*z2 - z1*y2, z1*x2 - x1*z2, x1*y2 - y1*x2);
}


void V3::SetFromColor(unsigned int col) {
	//Somehow this unsigned int of form 0x???????? is being casted to a char to I assume take the specified 2 characters
	// of each element to find its individual red, green and blue, casting it to a float and then dividing by the max 255 it can have
	V3 &v = *this;
	v[0] = (float)(((unsigned char*)(&col))[0]) / 255.0f;
	v[1] = (float)(((unsigned char*)(&col))[1]) / 255.0f;
	v[2] = (float)(((unsigned char*)(&col))[2]) / 255.0f;

}

unsigned int V3::GetColor() {
	//contained within the vectors elements is what was in set color, I assume this first 7 lines is to make sure
	//wrong cases are just set to something so it doesn't freak, but the last few lines take each element back to
	//a byte hexadecimal int, and meshes them back together to get a full color
	V3 v = *this;
	v[0] = (v[0] < 0.0f) ? 0.0f : v[0];
	v[1] = (v[1] < 0.0f) ? 0.0f : v[1];
	v[2] = (v[2] < 0.0f) ? 0.0f : v[2];

	v[0] = (v[0] > 1.0f) ? 1.0f : v[0];
	v[1] = (v[1] > 1.0f) ? 1.0f : v[1];
	v[2] = (v[2] > 1.0f) ? 1.0f : v[2];

	unsigned int ret;
	int rgb[3];
	rgb[0] = (int)(v[0] * 255.0f);
	rgb[1] = (int)(v[1] * 255.0f);
	rgb[2] = (int)(v[2] * 255.0f);
	ret = 0xFF000000 + 256 * 256 * rgb[2] + 256 * rgb[1] + rgb[0];
	return ret;
}


V3 V3::RotateThisVector(V3 aD, float theta) {
	// Uses rotate this point where the 
	return RotateThisPoint(V3(0.0f, 0.0f, 0.0f), aD, theta);

}

V3 V3::RotateThisPoint(V3 aO, V3 aD, float theta) {
	// givens, a0 = origin point, aD = direction, theta is roation angle
	// sets up new coordinate system matrix where direction is the y component
	M33 laxes;
	laxes[1] = aD;
	V3 aux(1.0f, 0.0f, 0.0f);
	// checks is x or y element is small, as it would mean that axis is closer to being perpendicular with a
	if (fabsf(aD[0]) > fabsf(aD[1]))
		aux = V3(0.0f, 1.0f, 0.0f);
	// finishes new coordinate system matrix by cross product in order to make them all perpendicular
	laxes[2] = aux ^ aD;
	laxes[0] = laxes[1] ^ laxes[2];

	// moves this vector to new coordinate system, then makes a rotation matrix about y
	// creates a new vector by multiplying rotation vector by the vector in new coordinate system, which gives rotated vector
	//inverts new coordinate system to change the old vector to the new coordinate system
	V3 &P = *this;
	V3 Pl = laxes*(P - aO);
	M33 roty; roty.SetRotY(theta);
	V3 Pr = roty * Pl;
	return laxes.Inverted()*Pr + aO;

}

V3 V3::Light(V3 matColor, float ka, V3 ld, V3 norm, float kp) {
	// Finds Light diffusion factor by multiplying the norm by the light source direction vector
	V3 ret;
	float kd = (ld*-1.0f)*norm;
	//calculates Phong coefficient by finding reflecting power * all of this stuff
	V3 reflection = norm.UnitVector() * 2 * (norm.UnitVector() * ld.UnitVector()) - ld.UnitVector();
	float ks = pow(norm.UnitVector() * *this, kp);
	kd = (kd < 0.0f) ? 0.0f : kd;
	//this is the phong equation, ambient + (1 - ambient) * diffuse + specular which is (reflected * eye vector) ^kp
	ret = matColor*(ka + (1.0f - ka)*kd);
	return ret;
}

V3 V3::Light(V3 matColor, float ka, V3 ld) {
	//this didn't work so well, but i have a better idea on what I'd do
	V3 ret;
	float kd = (ld * -1.0f) * *this;
	kd = (kd < 0.0f) ? 0.0f : kd;
	//cout << kd << endl;
	ret = matColor * (ka + (1.0f - ka) * kd);
	return ret;
}