
#pragma once

#include <ostream>
#include <istream>

using namespace std;

class V3 {
public:
	float xyz[3]; //vector class contains 3 floats in an array for direciton
	V3() {}; //empty initialization
	V3(float x, float y, float z); //initialization with 3 floats for direction
	V3 operator+(V3 v1); //adding two vectors together, with operator allowing for fast equations
	V3 operator-(V3 v1); //subtracting two vectors, self - new vector
	V3 operator/(float x); //dividing all elements of vector by a float
	V3 operator*(float x); //multiplying all elements of vector by a float
	float operator*(V3 v1); //dot product, same operator can be used because of c++ override
	float& operator[](int i); //operator for getting an element of the vector 
	friend ostream& operator<<(ostream& ostr, V3 v); //operator for outputting a string of the vector elements to a stream
	V3 operator^(V3 v1); //cross product
	V3 UnitVector(); //returns normalized length of all directions to have their sums equal 1
	float Length(); //returns the total distance the vector moves
	void SetFromColor(unsigned int col); //turns the singular int color into a 3 element float vector
	unsigned int GetColor(); //returns the color within the vector
	V3 RotateThisPoint(V3 aO, V3 aD, float theta); //Rotates a point around a y axis
	V3 RotateThisVector(V3 aD, float theta); //Rotation a vector around another vector
	V3 Light(V3 matColor, float ka, V3 ld, V3 norm, float kp); //Calculates the light intensity vector for a normal

	V3 Light(V3 matColor, float ka, V3 ld); //Calculates light intensity by checking the distance of the point

};

