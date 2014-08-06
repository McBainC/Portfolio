#include "Texture.h"
#include <stdio.h>	


/* Headers for comparison */
GLubyte uTGAcompare[ 12 ] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };  // Uncompressed TGA Header
GLubyte cTGAcompare[ 12 ] = { 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // Compressed TGA Header


/*
===========
Constructor
===========
*/
TextureCreator::TextureCreator( void ) {
}


/*
==================
Destructor
Nothing to tidy up
==================
*/
TextureCreator::~TextureCreator( void ) {
}


/*
==========
TGA Loader
==========
*/
TextureInfo TextureCreator::LoadTGA( char *fname ) {
	/* Create temp TextureInfo */
	TextureInfo temp;
	temp.mCorrectlyLoaded = false;

	/* TGA image data */
	FILE * fTGA;			     // File pointer to texture file
	fTGA = fopen( fname, "rb" ); // Open file for reading

	if( fTGA == NULL ) {		 // If it didn't open....
		/* Possibly do something other than a messagebox here */ 
		MessageBox( NULL, "Could not open texture file", "ERROR", MB_OK ); // Display an error message
		return temp;													   // Exit function
	}

	/* We need to see what type of TGA we are loading */
	if( fread( &tgaheader, sizeof( TGAHeader ), 1, fTGA ) == 0 ) {		   // Attempt to read 12 byte header from file
		MessageBox( NULL, "Could not read file header", "ERROR", MB_OK );  // If it fails, display an error message 
		if( fTGA != NULL ) {											   // Check to seeiffile is still open
			fclose( fTGA );												   // If it is, close it
		}
		return temp;													   // Exit function
	}

	/* See if header matches the predefined header of */ 
	if(memcmp( uTGAcompare, &tgaheader, sizeof( tgaheader ) ) == 0 )	{			  // an Uncompressed TGA image
		temp = LoadUncompressedTGA( fname, fTGA );						              // If so, jump to Uncompressed TGA loading code
	/* See if header matches the predefined header of */
	} else if( memcmp( cTGAcompare, &tgaheader, sizeof( tgaheader ) ) == 0 ) {	      // an RLE compressed TGA image														        
		temp = LoadCompressedTGA( fname, fTGA );							          // If so, jump to Compressed TGA loading code
	/* If header matches neither type */
	} else {
		MessageBox( NULL, "TGA file be type 2 or type 10 ", "Invalid Image", MB_OK ); // Display an error
		fclose( fTGA );
		return temp;																  // Exit function
	}

	temp.mCorrectlyLoaded = true;                                                     // Flag as correctly loaded
	return temp;		                                                              // Fingers crossed!								
}


/*
=======================================================================================================
LoadUncompressedTGA
Load an uncompressed TGA (note, much of this code is based on NeHe's TGA Loading code nehe.gamedev.net)    
=======================================================================================================
*/
TextureInfo TextureCreator::LoadUncompressedTGA( char * filename, FILE * fTGA ) {   
	/* Create temp TextureInfo */
	TextureInfo temp;
	temp.mCorrectlyLoaded = false;

	/* Read TGA header */	
	if( fread( tga.header, sizeof( tga.header ), 1, fTGA ) == 0 ) {		  						
		MessageBox( NULL, "Could not read info header", "ERROR", MB_OK ); // Display error
		if( fTGA != NULL ) {											  // if file is still open
			fclose( fTGA );												  // Close it
		}
		return temp;													  // Return failure
	}	

	temp.mTextureWidth  = tga.header[ 1 ] * 256 + tga.header[ 0 ]; // Determine The TGA Width	(highbyte*256+lowbyte)
	temp.mTextureHeight = tga.header[ 3 ] * 256 + tga.header[ 2 ]; // Determine The TGA Height	(highbyte*256+lowbyte)
	temp.mTextureBpp    = tga.header[ 4 ];						   // Determine the bits per pixel
	tga.Width	        = temp.mTextureWidth;					   // Copy width into local structure						
	tga.Height	        = temp.mTextureHeight;					   // Copy height into local structure
	tga.Bpp		        = temp.mTextureBpp;						   // Copy BPP into local structure
	
	/* Make sure all information is valid */
	if( ( temp.mTextureWidth <= 0 ) || ( temp.mTextureHeight <= 0 ) || ( ( temp.mTextureBpp != 24 ) && ( temp.mTextureBpp != 32 ) ) ) {
		MessageBox( NULL, "Invalid texture information", "ERROR", MB_OK ); // Display Error
		if( fTGA != NULL ) {											   // Check if file is still open
			fclose( fTGA );												   // If so, close it
		}
		return temp;													   // Return failed
	}

	/* If the BPP of the image is 24... */
	if( temp.mTextureBpp == 24 ) {														   
		temp.mTextureType = GL_RGB;  // Set Image type to GL_RGB
	/* Else if its 32 BPP */
	} else {																		   
		temp.mTextureType = GL_RGBA; // Set image type to GL_RGBA
	}

	tga.bytesPerPixel      = ( tga.Bpp / 8 );								 // Compute the number of BYTES per pixel
	tga.imageSize		   = ( tga.bytesPerPixel * tga.Width * tga.Height ); // Compute the total amout ofmemory needed to store data
	temp.pTextureImageData = ( GLubyte * )malloc( tga.imageSize );			 // Allocate that much memory

	/* If no space was allocated */
	if( temp.pTextureImageData == NULL ) {										       
		MessageBox( NULL, "Could not allocate memory for image", "ERROR", MB_OK ); // Display Error
		fclose( fTGA );															   // Close the file
		return temp;															   // Return failed
	}

	/* Attempt to read image data */
	if( fread( temp.pTextureImageData, 1, tga.imageSize, fTGA) != tga.imageSize ) {     
		MessageBox( NULL, "Could not read image data", "ERROR", MB_OK ); // Display Error
		if( temp.pTextureImageData != NULL )	{						 // If imagedata has data in it
			free( temp.pTextureImageData );								 // Delete data from memory
		}
		fclose( fTGA );													 // Close file
		return temp;													 // Return failed
	}

	/* Byte Swapping Optimized By Steve Thomas */
	for( GLuint cswap = 0; cswap < ( int )tga.imageSize; cswap += tga.bytesPerPixel ) {
		temp.pTextureImageData[ cswap ] ^= temp.pTextureImageData[ cswap+2 ] ^=
		temp.pTextureImageData[ cswap ] ^= temp.pTextureImageData[ cswap+2 ];
	}

	fclose( fTGA );               // Close file
	temp.mCorrectlyLoaded = true; // Flag as correctly loaded
	return temp;                  // Return success
}


/*
====================
LoadCompressedTGA
Load COMPRESSED TGAs
====================
*/
TextureInfo TextureCreator::LoadCompressedTGA( char * filename, FILE * fTGA ) { 
	/* Create temp TextureInfo */
	TextureInfo temp;
	temp.mCorrectlyLoaded = false;

	/* Attempt to read header */
	if( fread( tga.header, sizeof( tga.header ), 1, fTGA ) == 0 ) {
		MessageBox( NULL, "Could not read info header", "ERROR", MB_OK ); // Display Error
		/* If file is open */
		if( fTGA != NULL ) {												
			fclose( fTGA );												  // Close it
		}
		return temp;													  // Return failed
	}

	temp.mTextureWidth  = tga.header[ 1 ] * 256 + tga.header[ 0 ]; // Determine The TGA Width	(highbyte*256+lowbyte)
	temp.mTextureHeight = tga.header[ 3 ] * 256 + tga.header[ 2 ]; // Determine The TGA Height	(highbyte*256+lowbyte)
	temp.mTextureBpp    = tga.header[ 4 ];						   // Determine Bits Per Pixel
	tga.Width		    = temp.mTextureWidth;					   // Copy width to local structure
	tga.Height	    	= temp.mTextureHeight;				       // Copy width to local structure
	tga.Bpp		     	= temp.mTextureBpp;						   // Copy width to local structure

	/* Make sure all texture info is ok */
	if( ( temp.mTextureWidth <= 0 ) || ( temp.mTextureHeight <= 0 ) || ( ( temp.mTextureBpp != 24 ) && ( temp.mTextureBpp != 32 ) ) ) {	
		MessageBox( NULL, "Invalid texture information", "ERROR", MB_OK ); // If it isnt...Display error
		/* Check if file is open */
		if( fTGA != NULL ) {															
			fclose( fTGA );											       // If it is, close it
		}
		return temp;													   // Return failed
	}

	/* If the BPP of the image is 24... */
	if( temp.mTextureBpp == 24 )	{															
		temp.mTextureType = GL_RGB;  // Set Image type to GL_RGB
	/* Else if its 32 BPP */
	} else {																			
		temp.mTextureType = GL_RGBA; // Set image type to GL_RGBA
	}

	tga.bytesPerPixel   	= ( tga.Bpp / 8 );								  // Compute BYTES per pixel
	tga.imageSize		    = ( tga.bytesPerPixel * tga.Width * tga.Height ); // Compute amout of memory needed to store image
	temp.pTextureImageData	= ( GLubyte * )malloc( tga.imageSize );			  // Allocate that much memory

	/* If it wasnt allocated correctly.. */
	if( temp.pTextureImageData == NULL ) {													
		MessageBox( NULL, "Could not allocate memory for image", "ERROR", MB_OK ); // Display Error
		fclose( fTGA );															   // Close file
		return temp;															   // Return failed
	}

	GLuint pixelcount	  = tga.Height * tga.Width;					  // Number of pixels in the image
	GLuint currentpixel	  = 0;										  // Current pixel being read
	GLuint currentbyte	  = 0;										  // Current byte 
	GLubyte * colorbuffer = ( GLubyte * )malloc( tga.bytesPerPixel ); // Storage for 1 pixel

	do {
		GLubyte chunkheader = 0;										     // Storage for "chunk" header

		/* Read in the 1 byte header */
		if( fread( &chunkheader, sizeof( GLubyte ), 1, fTGA ) == 0 ) {
			MessageBox( NULL, "Could not read RLE header", "ERROR", MB_OK ); // Display Error
			/* If file is open */
			if( fTGA != NULL ) {														
				fclose( fTGA );								                 // Close file
			}
			/* If there is stored image data */
			if( temp.pTextureImageData != NULL ) {
				free( temp.pTextureImageData );								 // Delete image data
			}
			return temp;													 // Return failed
		}

		/* If the ehader is < 128, it means the that is the number of RAW color packets minus 1 */
		if( chunkheader < 128 ) {												   // That follow the header
			chunkheader++;														   // Add 1 to get number of following color values
			/* Read RAW color values */
			for( short counter = 0; counter < chunkheader; counter++ ) {	
				/* Try to read 1 pixel */
				if( fread( colorbuffer, 1, tga.bytesPerPixel, fTGA ) != tga.bytesPerPixel ) { 
					MessageBox(NULL, "Could not read image data", "ERROR", MB_OK); // IF we cant, display an error

					/* See if file is open */
					if( fTGA != NULL ) {													
						fclose( fTGA );											   // If so, close file
					}

					/* See if colorbuffer has data in it */
					if( colorbuffer != NULL ) {											
						free( colorbuffer );									   // If so, delete it
					}

					/* See if there is stored Image data */
					if( temp.pTextureImageData != NULL ) {
						free( temp.pTextureImageData );							   // If so, delete it too
					}

					return temp;												   // Return failed
				}
				
				/* Write to memory */
				temp.pTextureImageData[ currentbyte ]     = colorbuffer[ 2 ]; // Flip R and B vcolor values around in the process 
				temp.pTextureImageData[ currentbyte + 1 ] = colorbuffer[ 1 ];
				temp.pTextureImageData[ currentbyte + 2 ] = colorbuffer[ 0 ];

				/* If its a 32 bpp image */
				if( tga.bytesPerPixel == 4 ) {
					temp.pTextureImageData[ currentbyte + 3 ] = colorbuffer[ 3 ]; // Copy the 4th byte
				}

				currentbyte += tga.bytesPerPixel; // Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;					  // Increase current pixel by 1

				/* Make sure we havent read too many pixels */
				if( currentpixel > pixelcount )	{
					MessageBox( NULL, "Too many pixels read", "ERROR", NULL ); // If there is too many... Display an error!

					/* If there is a file open */
					if( fTGA != NULL ) {
						fclose( fTGA );	// Close file
					}	

					/* If there is data in colorbuffer */
					if( colorbuffer != NULL ) {
						free( colorbuffer ); // Delete it
					}

					/* If there is Image data */
					if( temp.pTextureImageData != NULL )	{
						free( temp.pTextureImageData ); // Delete it
					}

					return temp; // Return failed
				}
			}
		/* chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times */
		} else {
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			/* Attempt to read following color values */
			if( fread( colorbuffer, 1, tga.bytesPerPixel, fTGA ) != tga.bytesPerPixel )	{	
				MessageBox( NULL, "Could not read from file", "ERROR", MB_OK ); // If attempt fails.. Display error (again)

				/* If there is a file open */
				if( fTGA != NULL ) {
					fclose( fTGA ); // Close it
				}

				/* If there is data in the colorbuffer */
				if( colorbuffer != NULL )	{
					free( colorbuffer ); // Delete it
				}

			    /* If thereis image data */
				if( temp.pTextureImageData != NULL ) {
					free( temp.pTextureImageData ); // Delete it
				}

				return temp; // Return failed
			}

			/* Copy the color into the image data as many times as dictated */ 
			for( short counter = 0; counter < chunkheader; counter++ ) { // By the header
				temp.pTextureImageData[ currentbyte ]     = colorbuffer[ 2 ]; // Switch R and B bytes areound while copying
				temp.pTextureImageData[ currentbyte + 1 ] = colorbuffer[ 1 ];
				temp.pTextureImageData[ currentbyte + 2 ] = colorbuffer[ 0 ];

				/* If TGA images is 32 bpp */
				if( tga.bytesPerPixel == 4 ) {
					temp.pTextureImageData[ currentbyte + 3 ] = colorbuffer[ 3 ]; // Copy 4th byte
				}

				currentbyte += tga.bytesPerPixel; // Increase current byte by the number of bytes per pixel
				currentpixel++;					  // Increase pixel count by 1

				/* Make sure we havent written too many pixels */
				if( currentpixel > pixelcount )	{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL); // If there is too many... Display an error!

					/* If there is a file open */
					if( fTGA != NULL ) {
						fclose( fTGA ); // Close file
					}	

					/* If there is data in colorbuffer */
					if( colorbuffer != NULL ) {
						free( colorbuffer ); // Delete it
					}
					 
					/* If there is Image data */
					if( temp.pTextureImageData != NULL )	{
						free( temp.pTextureImageData ); // Delete it
					}

					return temp; // Return failed
				}
			}
		}
	}

	/* Loop while there are still pixels left */
	while( currentpixel < pixelcount );

	fclose( fTGA );               // Close the file
	temp.mCorrectlyLoaded = true; // Flag as correctly loaded
	return temp;                  // Return success
}


/*
==============================================================
CreateGLTexture
Modified to take in a TextureInfo Struct of preloaded tga data
==============================================================
*/
bool TextureCreator::CreateGLTexture( TextureInfo texture, GLuint & TexID ) {
	/* Typical Texture Generation Using Data From The TGA */
	if( texture.mCorrectlyLoaded ) {
		glGenTextures( 1, &TexID ); // Create The Texture 
		glBindTexture( GL_TEXTURE_2D, TexID );
		glTexImage2D ( GL_TEXTURE_2D, 0, texture.mTextureBpp / 8, texture.mTextureWidth, texture.mTextureHeight, 0, texture.mTextureType, GL_UNSIGNED_BYTE, texture.pTextureImageData );
		glTexParameteri (GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR );
	}

	/* If Texture Image Exists */
	if( texture.pTextureImageData ) {
		free( texture.pTextureImageData ); // Free The Texture Image Memory
	}

	return false;
}