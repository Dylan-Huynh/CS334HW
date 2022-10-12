#pragma once

#include "V3.h"

using namespace std;


class M33 {
public:
	V3 rows[3];
	M33() {};
	V3& operator[](int i);
	V3 operator*(V3 v);
	V3 GetColumn(int i);
	void SetColumn(int i, V3 c);
	void SetRotY(float theta);
	M33 Inverted();

	M33 identityMatrix();
	M33 matrixMultiplication(M33 m);
	M33 Transposed();
	void SetRotX(float theta);
	void SetRotZ(float theta);
	M33 operator>>(istream& istr);
	friend ostream& operator<<(ostream& ostr, M33 m);
};