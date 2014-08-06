# Name:        Python Animal Class
# Author:      Chris McBain - April 2014
# Description:
# Abstract Base Class
# Based on <crowd_sim_diy.py> by G Clarke
# Contains all shared functionality for a moving object
# Creates custom attributes:
# - initialPosition/Heading/Speed and speed - for initialisation and resetting
# Object is represented by a polyCone
# Exposed functionality - position, heading and polyCone colour
# Values for - animalName / initialPosition / initialHeading / initialSpeed MUST be set upon creation
# Base Class for <Prey.py> and <Predator.py>


# Imports #
import maya.cmds as cmds
import maya.mel as mel
import math


# Animal Class #
# Base class for Predator & Prey classes
# Inherits from object
class Animal( object ):

    # Default 'Constructor' - automatically called by Python AFTER object creation
    # Creates and/or initialises required object attributes
    def __init__( self, *args, **kwargs ):

        # Create the animal name 
        # For object identification & name of attached polyShape
        self.animalName = kwargs.setdefault( "animalName" )

        # Create shared attributes
        self.initialPosition = kwargs.setdefault( "initialPosition" )
        self.initialHeading  = kwargs.setdefault( "initialHeading" )
        self.initialSpeed    = kwargs.setdefault( "initialSpeed" )
        self.speed           = kwargs.setdefault( "initialSpeed" )

        # Create a cone to represent the animal
        # Created aligned with positive z-axis
        self.cone = cmds.polyCone( name = self.animalName, axis = [ 0, 0, 1 ], height = 2, radius = 1 )

        # Create and set the initial position
        if cmds.objExists( self.animalName+".initialPositionX" ) is False:
            cmds.addAttr( self.animalName, longName = "initialPositionX", defaultValue = 0.0, keyable = True )
            cmds.setAttr( self.animalName+".initialPositionX", self.initialPosition.x )
        if cmds.objExists( self.animalName+".initialPositionY" ) is False:
            cmds.addAttr( self.animalName, longName = "initialPositionY", defaultValue = 0.0, keyable = True )
            cmds.setAttr( self.animalName+".initialPositionY", self.initialPosition.y )
        if cmds.objExists( self.animalName+".initialPositionZ" ) is False:
            cmds.addAttr( self.animalName, longName="initialPositionZ", defaultValue = 0.0, keyable = True )
            cmds.setAttr( self.animalName+".initialPositionZ", self.initialPosition.z )

        # Set objects position
        cmds.setAttr( self.animalName+".translateX", cmds.getAttr( self.animalName+".initialPositionX" ) )
        cmds.setAttr( self.animalName+".translateY", cmds.getAttr( self.animalName+".initialPositionY" ) )
        cmds.setAttr( self.animalName+".translateZ", cmds.getAttr( self.animalName+".initialPositionZ" ) )

        # Create and set the initial heading
        if cmds.objExists( self.animalName+".initialHeading") is False:
            cmds.addAttr( self.animalName, longName = "initialHeading", defaultValue = 0.0, keyable = True )
            cmds.setAttr( self.animalName+".initialHeading", self.initialHeading )

        # Set objects y-axis rotation to its initial heading
        cmds.setAttr( self.animalName+".rotateY", cmds.getAttr( self.animalName+".initialHeading" ) )

        # Create and set initial speed
        if cmds.objExists( self.animalName+".initialSpeed") is False:
            cmds.addAttr( self.animalName, longName = "initialSpeed", defaultValue = 0.5, keyable = True )
            cmds.setAttr( self.animalName+".initialSpeed", self.initialSpeed )

        # Create and set speed
        if cmds.objExists( self.animalName+".speed" ) is False:
            cmds.addAttr( self.animalName, longName = "speed", defaultValue=cmds.getAttr( self.animalName+".initialSpeed" ), keyable = True )

        # Set the initial speed
        cmds.setAttr( self.animalName+".speed", cmds.getAttr( self.animalName+".initialSpeed" ) )


    # Move 
    # Uses pythagoras to calculate 2D movement in XZ plane
    def move( self, frame_time ):

        # Get the heading angle
        heading = cmds.getAttr( self.animalName+".rotateY" );
        
        # Get the speed (in units per second)
        speed = cmds.getAttr( self.animalName+".speed" )
        
        # Calculate the overall distance moved between frames
        overall_distance = speed * frame_time
        
        # Calculate the direction to go in
        distanceX = overall_distance * math.sin( math.radians( heading ) );
        distanceZ = overall_distance * math.cos( math.radians( heading ) );
    
        # Update the position based on the calculated direction and speed
        cmds.setAttr( self.animalName+".translateX", cmds.getAttr( self.animalName+".translateX") + distanceX )
        cmds.setAttr( self.animalName+".translateZ", cmds.getAttr( self.animalName+".translateZ") + distanceZ )


    # Get Position
    # Returns animal position as a Vector3
    def get_position( self ):

        # Create and return position Vector3 using objects position
        position =  Vector3( x = cmds.getAttr( self.animalName+".translateX" ), y = cmds.getAttr( self.animalName+".translateY" ), z = cmds.getAttr( self.animalName+".translateZ" ) )
        return position


    # Get Heading
    # Returns the objects y-axis rotation
    def get_heading( self ):

        return cmds.getAttr( self.animalName+".rotateY" )


    # Set Heading
    # Sets the objects y-axis rotation
    def set_heading( self, heading ):

        cmds.setAttr( self.animalName+".rotateY", heading )


    # Get Heading Vector
    # Returns Vector3 based on animals y-axis rotation
    def get_heading_vector( self ):

        # Get agent heading angle
        heading_angle = cmds.getAttr( self.animalName+".rotateY" )

        # Convert heading angle to a Vector3
        heading_vector = get_vector_from_angle( heading_angle )

        return heading_vector


    # Change Shape Colour
    # Change the colour!
    def change_shape_colour( self, red, green, blue ):

        # Select then change the objects cone colour
        cmds.select( self.animalName, visible = True )
        cmds.polyColorPerVertex( colorRGB = [ red, green, blue ], colorDisplayOption = True )

        # Deselect 
        cmds.select( clear = True )
