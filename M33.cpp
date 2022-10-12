
#include "M33.h"

V3& M33::operator[](int i) {
	// get function for a certain row
	return rows[i];

}

V3 M33::operator*(V3 v) {
	// multiplies each row in matrix by the given vector to return dot product and puts them all together
	M33 &m = *this;
	V3 ret;
	ret[0] = m[0] * v;
	ret[1] = m[1] * v;
	ret[2] = m[2] * v;
	return ret;

}


V3 M33::GetColumn(int i){
	//returns the collumns by accessing the certain element of each vector in the matrix
	return V3(rows[0][i], rows[1][i], rows[2][i]);

}

void M33::SetColumn(int i, V3 c) {
	// sets a particular element of each vector by the corresponding element in given vector
	rows[0][i] = c[0];
	rows[1][i] = c[1];
	rows[2][i] = c[2];

}

void M33::SetRotY(float theta) {
	// Rotation of y matrix by turning theta into radians, calculating sin and cos of the theta
	// Then making rotation matrix with sin cos and y=1
	float thetaRad = theta / 180.0f * 3.1415926f;
	float cost = cosf(thetaRad);
	float sint = sinf(thetaRad);
	M33 &m = *this;
	m[0] = V3(cost, 0.0f, sint);
	m[1] = V3(0.0f, 1.0f, 0.0f);
	m[2] = V3(-sint, 0.0f, cost);

}


// ^ stands for cross product
// * for dot product
M33 M33::Inverted() {
	// inverts matrix, first by finding cross product of the rows that is not corresponding to the column,
	// then divide the product of that by the product of the product and the corresponding row
	M33 ret;
	V3 a = GetColumn(0), b = GetColumn(1), c = GetColumn(2);
	V3 _a = b ^ c; _a = _a / (a * _a);
	V3 _b = c ^ a; _b = _b / (b * _b);
	V3 _c = a ^ b; _c = _c / (c * _c);
	ret[0] = _a;
	ret[1] = _b;
	ret[2] = _c;

	return ret;

}

M33 M33::identityMatrix() {
	// initializes an identity matrix, this isn't an identity matrix :/
	// I should make each one correspond to a 1 in the specified row, 0 elsewhere
	M33 ret;
	V3 identityVector = V3(1, 1, 1);
	//ret[0] = identityVector;
	//ret[1] = identityVector;
	//ret[2] = identityVector;
	ret[0] = V3(1, 0, 0);
	ret[1] = V3(0, 1, 0);
	ret[2] = V3(0, 0, 1);
	return ret;

}

M33 M33::matrixMultiplication(M33 m) {

	//multiplies matrxices together, so for each element of the new matrix, get the dot product of the this vectors row,
	// and the m matrix's column vector
	M33& m1 = *this;
	M33 ret;
	for (int u = 0; u < 3; u++) {
		for (int v = 0; v < 3; v++) {
			ret[u][v] = m1[u].operator*(m.GetColumn(v));
		}
	}
	return ret;
}

void M33::SetRotX(float theta) {

	// made the same things for x as with y, turn theta to degrees and make matrix contasining sines and cosines
	float thetaRad = theta / 180.0f * 3.1415926f;
	float cost = cosf(thetaRad);
	float sint = sinf(thetaRad);
	M33& m = *this;
	m[0] = V3(1.0f, 0.0f, 0.0f);
	m[1] = V3(0.0f, cost, sint);
	m[2] = V3(0.0f, -sint, cost);

}


void M33::SetRotZ(float theta) {
	// made the same things for z as with y, turn theta to degrees and make matrix contasining sines and cosines
	float thetaRad = theta / 180.0f * 3.1415926f;
	float cost = cosf(thetaRad);
	float sint = sinf(thetaRad);
	M33& m = *this;
	m[0] = V3(cost, sint, 0.0f);
	m[1] = V3(-sint, cost, 0.0f);
	m[2] = V3(0.0f, 0.0f, 1.0f);

}

M33 M33::Transposed() {
	// move the matrix elements of the original matrix, and turn them into return matrix with into the index with the
	// rows and columns reversed, in hindsight I didn't need to make the u == v case it'd do it itself
	M33 ret;
	M33& m0 = *this;
	for (int u = 0; u < 3; u++) {
		for (int v = 0; v < 3; v++) {
			if (u == v) {
				ret[u][v] = m0[u][v];
			}
			else {
				ret[u][v] = m0[v][u];
			}
		}
	}

	return ret;

}

M33 M33::operator>>(istream& istr) {
	// takes in an istream where I get each element of a matrix possibly
	M33 ret;
	istr >> ret[0][0] >> ret[0][1] >> ret[0][2] >> ret[1][0] >> ret[1][1] >> ret[1][2] >> ret[2][0] >> ret[2][1] >> ret[2][2];
	return ret;
}

ostream& operator<<(ostream& ostr, M33 m) {
	//outputs the matrix over each row and then uses endl to make it 3 x 3
	return ostr << m[0][0] << " " << m[0][1] << " " << m[0][2] << endl << m[1][0] << " " << m[1][1] <<
		" " << m[1][2] << endl << m[2][0] << " " << m[2][1] << " " << m[2][2] << endl;

}