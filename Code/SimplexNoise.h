/*
===============================================================================
A C++ port of a speed-improved simplex noise algorithm for 2D in Java.
 
Based on example code by Stefan Gustavson (stegu@itn.liu.se).
Optimisations by Peter Eastman (peastman@drizzle.stanford.edu).
Better rank ordering method by Stefan Gustavson in 2012.
C++ port and minor type and algorithm changes by Josh Koch (jdk1337@gmail.com).
 
This could be speeded up even further, but it's useful as it is.
 
Version 2012-04-12
 
The original Java code was placed in the public domain by its original author,
Stefan Gustavson. You may use it as you see fit,
but attribution is appreciated.
===============================================================================
*/

#ifndef _H_SIMPLEXNOISE
#define _H_SIMPLEXNOISE

#include <cstdint>
#include <math.h>

// Inner class to speed up gradient computations
// (array access is a lot slower than member access)
class Grad{
public:
	double x, y, z, w;

	Grad(double x, double y, double z){
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

class SimplexNoise{
public:

	SimplexNoise();

	~SimplexNoise();

	// Initialize permutation arrays
	static void Init(int seed);

	// Generate 2D noise - returns a double in the range of [0,1]
	static double Noise2D(double xin, double yin);

private:

	// Fast floor
	inline static int32_t fastFloor( double x ) {
		int32_t xi = static_cast<int32_t>(x);
		return x < xi ? xi - 1 : xi;
	}

	//All variables needed for the noise computation are here
	inline static double dot(Grad g, double x, double y) {
		return g.x*x + g.y*y; }

	inline static double dot(Grad g, double x, double y, double z) {
		return g.x*x + g.y*y + g.z*z; }

	inline static double dot(Grad g, double x, double y, double z, double w) {
		return g.x*x + g.y*y + g.z*z + g.w*w; }

	// Skewing and unskewing factors
	static const double F2;
    static const double G2;


    static const Grad grad3[12];
	//Permutation arrays
    static uint8_t p[256];	
    static uint8_t perm[512];
    static uint8_t permMod12[512];
};

#endif