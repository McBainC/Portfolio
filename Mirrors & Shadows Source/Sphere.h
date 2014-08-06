#pragma once


#include "primitives.h"


#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>
#include <sys/types.h>
#include <sys/timeb.h>


using namespace std;


/*
=====================================================
SphereSegment
Struct for a thread to generate a segment of a sphere
=====================================================
*/
struct SphereSegment {
	int mDivisions;                           // resolution of the sphere
	float mRadius;                            // radius of the sphere

	float mLatitudeStart;                     // starting latitude
	int mVectorIndexStart;                    // where to start in vector

	vector< Vector3 > *PRIM_VERTSReference;   // pointer reference to vertices vector
	vector< Vector2 > *PRIM_TCOORDSReference; // pointer reference to uv's vector
};


/*
===================================================
Sphere
Creates and renders a procedurally generated sphere
===================================================
*/
class Sphere : public Primitives {
public:
	Sphere( void );
	~Sphere( void );

	/* Timing Code */
	long long int mStartTime, mFinishTime, mTotalTime;
	long long int GetTime( void );

	/* Threads and Arguments */
	static const int NUM_OF_THREADS = 4;
	vector< SphereSegment > mSegments;
	HANDLE hThread[ NUM_OF_THREADS ];

	/* Threading Functions */
	static unsigned int __stdcall GenerateSegment( void *data );
	void ThreadSphere( void );

	/* Sphere Functions */
	void Create( int divisions, float radius );
	void Draw( void ) const;
};