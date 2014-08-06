#include "Model.h"


/*
========================================================================================
Model
Abstract base class for a model. The specific extended class will render the given model 
Author:	Brett Porter
Email: brettporter@yahoo.com
Website: http://www.geocities.com/brettporter/
Copyright (C)2000, Brett Porter. All Rights Reserved
This file may be used only as long as this copyright notice remains intact
========================================================================================
*/


/*
==========================================================================
Modified texture loading & brought up to code standard - Chris McBain 2013
==========================================================================
*/


/* 
===========
Constructor
===========
*/
Model::Model() {
	m_numMeshes = 0;
	m_pMeshes = NULL;
	m_numMaterials = 0;
	m_pMaterials = NULL;
	m_numTriangles = 0;
	m_pTriangles = NULL;
	m_numVertices = 0;
	m_pVertices = NULL;
}


/* 
==========
Destructor
==========
*/
Model::~Model() {
	int i;
	for( i = 0; i < m_numMeshes; i++ ) {
		delete[] m_pMeshes[i].m_pTriangleIndices;
	}
	for( i = 0; i < m_numMaterials; i++ ) {
		delete[] m_pMaterials[i].m_pTextureFilename;
	}

	m_numMeshes = 0;
	if( m_pMeshes != NULL ) {
		delete[] m_pMeshes;
		m_pMeshes = NULL;
	}

	m_numMaterials = 0;
	if( m_pMaterials != NULL ) {
		delete[] m_pMaterials;
		m_pMaterials = NULL;
	}

	m_numTriangles = 0;
	if ( m_pTriangles != NULL ) {
		delete[] m_pTriangles;
		m_pTriangles = NULL;
	}

	m_numVertices = 0;
	if( m_pVertices != NULL ) {
		delete[] m_pVertices;
		m_pVertices = NULL;
	}
}


/* 
====
draw
====
*/
void Model::draw() 
{
	GLboolean texEnabled = glIsEnabled( GL_TEXTURE_2D );

	/* Draw by group */
	for( int i = 0; i < m_numMeshes; i++ ) {

		/* REMOVED - was interfering with my own textures - Chris */

		//int materialIndex = m_pMeshes[i].m_materialIndex;
		//if( materialIndex >= 0 ) {
		//	glMaterialfv( GL_FRONT, GL_AMBIENT, m_pMaterials[materialIndex].m_ambient );
		//	glMaterialfv( GL_FRONT, GL_DIFFUSE, m_pMaterials[materialIndex].m_diffuse );
		//	glMaterialfv( GL_FRONT, GL_SPECULAR, m_pMaterials[materialIndex].m_specular );
		//	glMaterialfv( GL_FRONT, GL_EMISSION, m_pMaterials[materialIndex].m_emissive );
		//	glMaterialf( GL_FRONT, GL_SHININESS, m_pMaterials[materialIndex].m_shininess );
		//}
		//	
		//	if( m_pMaterials[materialIndex].m_texture > 0 ) {
		//		glBindTexture( GL_TEXTURE_2D, m_pMaterials[materialIndex].m_texture );
		//		glEnable( GL_TEXTURE_2D );
		//	} else {
		//		glDisable( GL_TEXTURE_2D );
		//	}
		//} else {
		//	/* Material properties? */
		//	glDisable( GL_TEXTURE_2D );
		//}

		glBegin( GL_TRIANGLES ); {
			for( int j = 0; j < m_pMeshes[i].m_numTriangles; j++ ) {
				int triangleIndex = m_pMeshes[i].m_pTriangleIndices[j];
				const Triangle* pTri = &m_pTriangles[triangleIndex];

				for( int k = 0; k < 3; k++ ) {
					int index = pTri->m_vertexIndices[k];

					glNormal3fv( pTri->m_vertexNormals[k] );
					glTexCoord2f( pTri->m_s[k], pTri->m_t[k] );
					glVertex3fv( m_pVertices[index].m_location );
				}
			}
		}
		glEnd();
	}

	if( texEnabled ) {
		glEnable( GL_TEXTURE_2D );
	} else {
		glDisable( GL_TEXTURE_2D );
	}
}


/* 
====================================================================
reloadTextures
REMOVED - once threaded was interfering with my own textures - Chris
====================================================================
*//*
void Model::reloadTextures() {	
	GLuint temp;
	for( int i = 0; i < m_numMaterials; i++ ) {
		if ( strlen( m_pMaterials[i].m_pTextureFilename ) > 0 ) {
			mTextureCreator.CreateGLTexture(m_pMaterials[i].m_pTextureFilename, temp );
			m_pMaterials[i].m_texture = temp;
		} else {
			m_pMaterials[i].m_texture = 0;
		}
	}
}*/