#include "TextureName.h"


/*
===========
Constructor
===========
*/
TextureName::TextureName( char *name ) :
pName( name ), mID( 0 ) {
}


/*
===================
Destructor
Nothing to clean up
===================
*/
TextureName::~TextureName( void ) {
	if( pName ) {
		pName = NULL;
	}
}