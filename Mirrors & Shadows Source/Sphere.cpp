#include "Sphere.h"


/*
=========================================================================
Constructor
Sets up the thread structs, then creates 4 threads to generate the sphere
=========================================================================
*/
Sphere::Sphere( void ) {
	Create( 5, 2.0f );
	ThreadSphere();
}


/*
===================
Destructor
Nothing to clean up
===================
*/
Sphere::~Sphere( void ) {
}


/*
===============================================
GetTime
To return time taken for threaded functionality
===============================================
*/
long long int Sphere::GetTime( void ) {
	struct _timeb timebuffer;
	_ftime64_s( &timebuffer );
	return ( timebuffer.time * 1000LL ) + timebuffer.millitm;
}


/*
=========================================================
GenerateSegment
Thread Function to generate a portion of a sphere 
Sphere generation code is based on the Swiftless Tutorial
http://www.swiftless.com/tutorials/opengl/sphere.html
=========================================================
*/
unsigned int __stdcall Sphere::GenerateSegment( void *data ) {
	/* Get Data */
	SphereSegment* args = ( SphereSegment* ) data;

	/* Setup Variables */
	const float PI = 3.1415f;
	float radius = args -> mRadius;
	int divisions = args -> mDivisions;
	int vectorIndex = args -> mVectorIndexStart;

	/* Generate vectices for sphere drawn with triangle strip */
	for( float latitude = args -> mLatitudeStart; latitude < ( args -> mLatitudeStart + ( 180 / NUM_OF_THREADS ) ); latitude += divisions ) {
		for( float longitude = 0; longitude < 360; longitude += divisions, vectorIndex += 4 ) {
			/* Vertex 1 */
			Vector3 temp1;
			Vector2 uvTemp1;
		
			temp1.x = radius * sin( ( longitude ) / 180 * PI ) * sin( ( latitude ) / 180 * PI );
			temp1.y = radius * cos( ( latitude ) / 180 * PI );
			temp1.z = radius * cos( ( longitude ) / 180 * PI ) * sin( ( latitude ) / 180 * PI );     

			uvTemp1.x = longitude / 360;
			uvTemp1.y = -latitude / 180;
           
			(*args -> PRIM_VERTSReference )[ vectorIndex ] = temp1;
			(*args -> PRIM_TCOORDSReference )[ vectorIndex ] = uvTemp1;

			/* Vertex 2 */
			Vector3 temp2;
			Vector2 uvTemp2;

			temp2.x = radius * sin( ( longitude ) / 180 * PI ) * sin( ( latitude + divisions ) / 180 * PI );
			temp2.y = radius * cos( ( latitude + divisions ) / 180 * PI );           
			temp2.z = radius * cos( ( longitude ) / 180 * PI ) * sin( ( latitude + divisions ) / 180 * PI ); 

			uvTemp2.x = longitude / 360;       
			uvTemp2.y = -( latitude + divisions ) / 180;              
           
			(*args -> PRIM_VERTSReference )[ vectorIndex + 1 ] = temp2;
			(*args -> PRIM_TCOORDSReference )[ vectorIndex + 1 ] = uvTemp2;

			/* Vertex 3 */
			Vector3 temp3;
			Vector2 uvTemp3;

			temp3.x = radius * sin( ( longitude + divisions ) / 180 * PI ) * sin( ( latitude ) / 180 * PI );
			temp3.y = radius * cos( ( latitude ) / 180 * PI );
			temp3.z = radius * cos( ( longitude + divisions ) / 180 * PI ) * sin( ( latitude ) / 180 * PI );

			uvTemp3.x = (longitude + divisions ) / 360; 
			uvTemp3.y = -latitude / 180;   
           
			(*args -> PRIM_VERTSReference )[ vectorIndex + 2 ] = temp3;
			(*args -> PRIM_TCOORDSReference )[ vectorIndex + 2 ] = uvTemp3;

			/* Vertex 4 */
			Vector3 temp4;
			Vector2 uvTemp4;

			temp4.x = radius * sin( ( longitude + divisions ) / 180 * PI ) * sin( ( latitude + divisions ) / 180 * PI ); 
			temp4.y = radius * cos( ( latitude + divisions ) / 180 * PI );       
			temp4.z = radius * cos( ( longitude + divisions ) / 180 * PI ) * sin( ( latitude + divisions ) / 180 * PI );
      
			uvTemp4.x = ( longitude + divisions ) / 360;    
			uvTemp4.y = -( latitude + divisions ) / 180;    
		
			(*args -> PRIM_VERTSReference )[ vectorIndex + 3 ] = temp4;
			(*args -> PRIM_TCOORDSReference )[ vectorIndex + 3 ] = uvTemp4;
		}
	}

	return 0;
}


/*
====================================
ThreadSphere
Generates the sphere using 4 threads
====================================
*/
void Sphere::ThreadSphere( void ) {
	/* Time Threading */
	mStartTime = GetTime();

	/* Thread Texture Loading */
	for( int i = 0; i < NUM_OF_THREADS; i++ ) {
		hThread[ i ] = ( HANDLE ) _beginthreadex( NULL, 0, GenerateSegment, ( void* )&mSegments[ i ], 0, NULL );
	}

	/* Wait for all textures to be loaded */
	WaitForMultipleObjects( NUM_OF_THREADS, hThread, TRUE, INFINITE );

	/* Time Taken */
	mFinishTime = GetTime(); 
	mTotalTime = mFinishTime - mStartTime;
}


/*
=======================================
Create
Creates a procedurally generated sphere
=======================================
*/
void Sphere::Create( int divisions, float radius ) {
	/* Generate number of vertices */
	NUM_OF_VERTS = ( ( 180 / divisions ) * ( 360 / divisions ) * 4 ); 

	/* Resize vectors */
	PRIM_VERTS.resize( NUM_OF_VERTS );
	PRIM_TCOORDS.resize( NUM_OF_VERTS );
	mSegments.resize( NUM_OF_THREADS );

	/* Set SphereSegments */
	for( int i = 0; i < NUM_OF_THREADS; i++ ) {
		/* Set Constants */
		mSegments[ i ].mDivisions = divisions;
		mSegments[ i ].mRadius = radius;
		
		/* Threading the first 180 degrees so that / number of threads */
		mSegments[ i ].mLatitudeStart = i * float( 180.0f / NUM_OF_THREADS );

		/* Total number of vertices divided by number of threads */
		mSegments[ i ].mVectorIndexStart = i * int( NUM_OF_VERTS / NUM_OF_THREADS );
		
		/* Set pointer references to vectors */
		mSegments[ i ].PRIM_VERTSReference = &PRIM_VERTS;
		mSegments[ i ].PRIM_TCOORDSReference = &PRIM_TCOORDS;
	}
}


/*
============================================
Draw
Renders sphere in OpenGl using vertex arrays
Normals are equal to Vertices for a sphere
============================================
*/
void Sphere::Draw( void ) const {
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnableClientState( GL_NORMAL_ARRAY );
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );

	glVertexPointer( 3, GL_FLOAT, 0, &PRIM_VERTS[ 0 ] );
	glNormalPointer( GL_FLOAT, 0, &PRIM_VERTS[ 0 ] );
	glTexCoordPointer( 2, GL_FLOAT, 0, &PRIM_TCOORDS[ 0 ] ); 
	glDrawArrays( GL_TRIANGLE_STRIP, 0, NUM_OF_VERTS );

	glDisableClientState( GL_VERTEX_ARRAY );
	glDisableClientState( GL_NORMAL_ARRAY );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}