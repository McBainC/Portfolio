//Basic texture loading functions
//Matthew Bett 2006

#include <stdio.h>	
#include "Tex.h"

GLubyte uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0};	// Uncompressed TGA Header
GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0};	// Compressed TGA Header
TGAHeader tgaheader;									// TGA header
TGA tga;
GLuint texture_type ;									//RGB or RGBA
GLubyte	*texture_imageData;
GLuint texture_width ; 
GLuint texture_height ;
GLuint texture_bpp;

//TGA Loader
bool LoadTGA( char *fname)
{
	// TGA image data
	FILE * fTGA;			   // File pointer to texture file
	fTGA = fopen(fname, "rb"); // Open file for reading

	if(fTGA == NULL)		   // If it didn't open....
	{
		//possibly do something other than a messagebox here. 
		MessageBox(NULL, "Could not open texture file", "ERROR", MB_OK); // Display an error message
		return false;													 // Exit function
	}

	//we need to see what type of TGA we are loading. 
	if(fread(&tgaheader, sizeof(TGAHeader), 1, fTGA) == 0)				 // Attempt to read 12 byte header from file
	{
		MessageBox(NULL, "Could not read file header", "ERROR", MB_OK);	 // If it fails, display an error message 
		if(fTGA != NULL)												 // Check to seeiffile is still open
		{
			fclose(fTGA);												 // If it is, close it
		}
		return false;													 // Exit function
	}

	if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)				        // See if header matches the predefined header of 
	{																		        // an Uncompressed TGA image
		LoadUncompressedTGA( fname, fTGA);						                    // If so, jump to Uncompressed TGA loading code
	}
	else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)		        // See if header matches the predefined header of
	{																		        // an RLE compressed TGA image
		LoadCompressedTGA( fname, fTGA);							                // If so, jump to Compressed TGA loading code
	}
	else																	        // If header matches neither type
	{
		MessageBox(NULL, "TGA file be type 2 or type 10 ", "Invalid Image", MB_OK);	// Display an error
		fclose(fTGA);
		return false;																// Exit function
	}

	return true;		                                                            //fingers crossed!								
}


bool LoadUncompressedTGA(char * filename, FILE * fTGA)	                // Load an uncompressed TGA (note, much of this code is based on NeHe's 
{																		// TGA Loading code nehe.gamedev.net)
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)				// Read TGA header
	{										
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);	// Display error
		if(fTGA != NULL)												// if file is still open
		{
			fclose(fTGA);												// Close it
		}
		return false;													// Return failure
	}	

	texture_width  = tga.header[1] * 256 + tga.header[0];			    // Determine The TGA Width	(highbyte*256+lowbyte)
	texture_height = tga.header[3] * 256 + tga.header[2];			    // Determine The TGA Height	(highbyte*256+lowbyte)
	texture_bpp	= tga.header[4];									    // Determine the bits per pixel
	tga.Width		= texture_width;									// Copy width into local structure						
	tga.Height		= texture_height;									// Copy height into local structure
	tga.Bpp			= texture_bpp;										// Copy BPP into local structure
	
	
	if((texture_width <= 0) || (texture_height <= 0) || ((texture_bpp != 24) && (texture_bpp !=32))) // Make sure all information is valid
	{
		MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);		 // Display Error
		if(fTGA != NULL)														 // Check if file is still open
		{
			fclose(fTGA);														 // If so, close it
		}
		return false;															 // Return failed
	}

	if(texture_bpp == 24)														 // If the BPP of the image is 24...
		texture_type	= GL_RGB;												 // Set Image type to GL_RGB
	else																		 // Else if its 32 BPP
		texture_type	= GL_RGBA;												 // Set image type to GL_RGBA

	tga.bytesPerPixel	= (tga.Bpp / 8);										 // Compute the number of BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);			 // Compute the total amout ofmemory needed to store data
	texture_imageData	= (GLubyte *)malloc(tga.imageSize);						 // Allocate that much memory

	if(texture_imageData== NULL)											     // If no space was allocated
	{
		MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK); // Display Error
		fclose(fTGA);															 // Close the file
		return false;															 // Return failed
	}

	if(fread(texture_imageData, 1, tga.imageSize, fTGA) != tga.imageSize)		 // Attempt to read image data
	{
		MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);			 // Display Error
		if(texture_imageData != NULL)											 // If imagedata has data in it
		{
			free(texture_imageData);											 // Delete data from memory
		}
		fclose(fTGA);															 // Close file
		return false;															 // Return failed
	}

	// Byte Swapping Optimized By Steve Thomas
	for(GLuint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
	{
		texture_imageData[cswap] ^= texture_imageData[cswap+2] ^=
		texture_imageData[cswap] ^= texture_imageData[cswap+2];
	}

	fclose(fTGA); // Close file*/
	return true;  // Return success
}


bool LoadCompressedTGA( char * filename, FILE * fTGA)									// Load COMPRESSED TGAs
{ 
	if(fread(tga.header, sizeof(tga.header), 1, fTGA) == 0)								// Attempt to read header
	{
		MessageBox(NULL, "Could not read info header", "ERROR", MB_OK);					// Display Error
		if(fTGA != NULL)																// If file is open
		{
			fclose(fTGA);																// Close it
		}
		return false;																	// Return failed
	}

	texture_width  = tga.header[1] * 256 + tga.header[0];								// Determine The TGA Width	(highbyte*256+lowbyte)
	texture_height = tga.header[3] * 256 + tga.header[2];								// Determine The TGA Height	(highbyte*256+lowbyte)
	texture_bpp	   = tga.header[4];														// Determine Bits Per Pixel
	tga.Width		= texture_width;								 					// Copy width to local structure
	tga.Height		= texture_height;								   					// Copy width to local structure
	tga.Bpp			= texture_bpp;														// Copy width to local structure

	if((texture_width <= 0) || (texture_height <= 0) || ((texture_bpp != 24) && (texture_bpp!=32)))	//Make sure all texture info is ok
	{
		MessageBox(NULL, "Invalid texture information", "ERROR", MB_OK);				// If it isnt...Display error
		if(fTGA != NULL)																// Check if file is open
		{
			fclose(fTGA);																// Ifit is, close it
		}
		return false;																	// Return failed
	}

	if(texture_bpp == 24)																// If the BPP of the image is 24...
		texture_type	= GL_RGB;									   					// Set Image type to GL_RGB
	else																				// Else if its 32 BPP
		texture_type	= GL_RGBA;														// Set image type to GL_RGBA

	tga.bytesPerPixel	= (tga.Bpp / 8);												// Compute BYTES per pixel
	tga.imageSize		= (tga.bytesPerPixel * tga.Width * tga.Height);					// Compute amout of memory needed to store image
	texture_imageData	= (GLubyte *)malloc(tga.imageSize);								// Allocate that much memory

	if(texture_imageData== NULL)														// If it wasnt allocated correctly..
	{
		MessageBox(NULL, "Could not allocate memory for image", "ERROR", MB_OK);		// Display Error
		fclose(fTGA);																	// Close file
		return false;																	// Return failed
	}

	GLuint pixelcount	= tga.Height * tga.Width;										// Nuber of pixels in the image
	GLuint currentpixel	= 0;															// Current pixel being read
	GLuint currentbyte	= 0;															// Current byte 
	GLubyte * colorbuffer = (GLubyte *)malloc(tga.bytesPerPixel);						// Storage for 1 pixel

	do
	{
		GLubyte chunkheader = 0;														// Storage for "chunk" header

		if(fread(&chunkheader, sizeof(GLubyte), 1, fTGA) == 0)							// Read in the 1 byte header
		{
			MessageBox(NULL, "Could not read RLE header", "ERROR", MB_OK);				// Display Error
			if(fTGA != NULL)															// If file is open
			{
				fclose(fTGA);															// Close file
			}
			if(texture_imageData!= NULL)												// If there is stored image data
			{
				free(texture_imageData);												// Delete image data
			}
			return false;																// Return failed
		}

		if(chunkheader < 128)															// If the ehader is < 128, it means the that is the number of RAW color packets minus 1
		{																				// that follow the header
			chunkheader++;																// add 1 to get number of following color values
			for(short counter = 0; counter < chunkheader; counter++)					// Read RAW color values
			{
				if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel) // Try to read 1 pixel
				{
					MessageBox(NULL, "Could not read image data", "ERROR", MB_OK);		// IF we cant, display an error

					if(fTGA != NULL)													// See if file is open
					{
						fclose(fTGA);													// If so, close file
					}

					if(colorbuffer != NULL)												// See if colorbuffer has data in it
					{
						free(colorbuffer);												// If so, delete it
					}

					if(texture_imageData!= NULL)										// See if there is stored Image data
					{
						free(texture_imageData);										// If so, delete it too
					}

					return false;														// Return failed
				}
																						// write to memory
				texture_imageData[currentbyte		] = colorbuffer[2];				    // Flip R and B vcolor values around in the process 
				texture_imageData[currentbyte + 1	] = colorbuffer[1];
				texture_imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												// if its a 32 bpp image
				{
					texture_imageData[currentbyte + 3] = colorbuffer[3];				// copy the 4th byte
				}

				currentbyte += tga.bytesPerPixel;										// Increase thecurrent byte by the number of bytes per pixel
				currentpixel++;															// Increase current pixel by 1

				if(currentpixel > pixelcount)											// Make sure we havent read too many pixels
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!

					if(fTGA != NULL)													// If there is a file open
					{
						fclose(fTGA);													// Close file
					}	

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(texture_imageData != NULL)										// If there is Image data
					{
						free(texture_imageData);										// delete it
					}

					return false;														// Return failed
				}
			}
		}
		else																			// chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
		{
			chunkheader -= 127;															// Subteact 127 to get rid of the ID bit
			if(fread(colorbuffer, 1, tga.bytesPerPixel, fTGA) != tga.bytesPerPixel)		// Attempt to read following color values
			{	
				MessageBox(NULL, "Could not read from file", "ERROR", MB_OK);			// If attempt fails.. Display error (again)

				if(fTGA != NULL)														// If thereis a file open
				{
					fclose(fTGA);														// Close it
				}

				if(colorbuffer != NULL)													// If there is data in the colorbuffer
				{
					free(colorbuffer);													// delete it
				}

				if(texture_imageData!= NULL)											// If thereis image data
				{
					free(texture_imageData);											// delete it
				}

				return false;															// return failed
			}

			for(short counter = 0; counter < chunkheader; counter++)					// copy the color into the image data as many times as dictated 
			{																			// by the header
				texture_imageData[currentbyte		] = colorbuffer[2];					// switch R and B bytes areound while copying
				texture_imageData[currentbyte + 1	] = colorbuffer[1];
				texture_imageData[currentbyte + 2	] = colorbuffer[0];

				if(tga.bytesPerPixel == 4)												// If TGA images is 32 bpp
				{
					texture_imageData[currentbyte + 3] = colorbuffer[3];				// Copy 4th byte
				}

				currentbyte += tga.bytesPerPixel;										// Increase current byte by the number of bytes per pixel
				currentpixel++;															// Increase pixel count by 1

				if(currentpixel > pixelcount)											// Make sure we havent written too many pixels
				{
					MessageBox(NULL, "Too many pixels read", "ERROR", NULL);			// if there is too many... Display an error!

					if(fTGA != NULL)													// If there is a file open
					{
						fclose(fTGA);													// Close file
					}	

					if(colorbuffer != NULL)												// If there is data in colorbuffer
					{
						free(colorbuffer);												// Delete it
					}

					if(texture_imageData != NULL)										// If there is Image data
					{
						free(texture_imageData);										// delete it
					}

					return false;														// Return failed
				}
			}
		}
	}

	while(currentpixel < pixelcount);													// Loop while there are still pixels left
	fclose(fTGA);																		// Close the file*/
	return true;																		// return success
}


bool CreateGLTexture(char *name, GLuint & TexID )
{
	LoadTGA(name);

	// Typical Texture Generation Using Data From The TGA ( CHANGE )
	glGenTextures(1, &TexID);	 // Create The Texture ( CHANGE )
	glBindTexture(GL_TEXTURE_2D, TexID);
	glTexImage2D(GL_TEXTURE_2D, 0, texture_bpp / 8, texture_width, texture_height, 0, texture_type, GL_UNSIGNED_BYTE, texture_imageData);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	if (texture_imageData)		 // If Texture Image Exists ( CHANGE )
	{
		free(texture_imageData); // Free The Texture Image Memory ( CHANGE )
	}

	return false;
}