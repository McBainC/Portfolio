#pragma once


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


#include "Model.h"


/*
===============================================
Brought up to code standard - Chris McBain 2013
===============================================
*/


/*
==================================
MilkshapeModel inherits from Model
==================================
*/
class MilkshapeModel : public Model {
public:
	/* Constructor */
	MilkshapeModel();

	/* Destructor */
	virtual ~MilkshapeModel();

	/* Load the model data into the private variables 
			filename			Model filename       */
	virtual bool loadModelData( const char *filename );
};