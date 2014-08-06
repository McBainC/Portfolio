# Name:        Python Vector3 Class
# Author:      Chris McBain - April 2014
# Description:
# Based on <crowd_sim_diy.py> by G Clarke
# Basic implementation of a Vector3, includes:
# - Addition, Subtraction, Scaling, Division, Normalisation - as both class and static methods 
# - Static methods - length, line vector between vector3's, distance between vector3's, angle -> direction vector3, direction vector3 -> angle
# - Static methods are prefixed with 'vector_' or 'get_'
# No matrix functionality - all based on 2D scene in the X/Z plane
# Declared as per other Maya Objects <insert_vector_name> = Vector3( x = <x_component>, y = <y_component>, z = <z_component> )
# Failure to initialise x/y/z components will return errors! 
# The x/y/x components are created by the __init__ function - will NOT be created if not passed as an argument in the constructor
# STANDALONE


# Imports #
import maya.cmds as cmds
import maya.mel as mel
import math


# Vector3 #
# Python implementation of a Vector3
class Vector3( object ):

    # __init__ (internal init function)
    # Create x/y/z coords and initialize
    def __init__( self, *args, **kwargs ):

        self.x = kwargs.setdefault( "x" )
        self.y = kwargs.setdefault( "y" )
        self.z = kwargs.setdefault( "z" )


    # Vector3_str
    # Overload for __str__ 
    # So print calls return useful information - the vector3's values
    def Vector3_str( self ):

        return "%s %s %s" %( self.x, self.y, self.z )


    # Reassign __str__ internal function to Vector3_str above
    #Vector3.__str__ = Vector3_str


    # Add
    # Sum of two vector3's
    def add( self, vector ):
        self.x += vector.x 
        self.y += vector.y
        self.z += vector.z


    # Subtract
    # Self minus vector
    def subtract( self, vectorB ):
        self.x -= vectorB.x
        self.y -= vectorB.y
        self.z -= vectorB.z


    # Scale
    # Scaling a vector3
    def scale( self, scale ):
        self.x *= scale
        self.y *= scale
        self.z *= scale


    # Divide
    # Divides by passed number
    def divide( self, number ):
        self.x /= number
        self.y /= number
        self.z /= number


    # Normalise
    # Divides by length plus safeguard for dividing by 0
    def normalise( self ):
    
        length = get_vector_length( self )

        if( length > 0.0 ):

            normalised_vector = Vector3( x =( self.x / length ), y =( self.y / length ), z =( self.z / length ) ) 


# 'Static' Vector3 Functions - dont have 'self' as a parameter #


# Vector Add
# Sum of two vector3's
def vector_add( vectorA, vectorB ):

    addition_vector = Vector3( x =( vectorA.x + vectorB.x ), y =( vectorA.y + vectorB.y ), z =( vectorA.z + vectorB.z ) )

    return addition_vector


# Vector Subtract
# A minus B
def vector_subtract( vectorA, vectorB ):

    subtraction_vector = Vector3( x =( vectorA.x - vectorB.x ), y =( vectorA.y - vectorB.y ), z =( vectorA.z - vectorB.z ) )

    return subtraction_vector


# Vector Scale
# Scaling a vector3
def vector_scale( vector, scale ):

    scale_vector = Vector3( x =( vector.x * scale ), y =( vector.y * scale ), z =( vector.z * scale ) )

    return scale_vector


# Vector Divide
# Divides by passed number
def vector_divide( vector, number ):

    division_vector = Vector3( x =( vector.x / number ), y =( vector.y / number ), z =( vector.z / number ) )

    return division_vector


# Vector Normalise
# Divides by length plus safeguard for dividing by 0
def vector_normalise( vector ):

    vector_length = get_vector_length( vector )
    normalised_vector = vector

    if( vector_length > 0.0 ):

        normalised_vector = Vector3( x =( vector.x / vector_length ), y =( vector.y / vector_length ), z =( vector.z / vector_length ) )

    return normalised_vector 


# Get Vector Length
# Returns length of the vector3
def get_vector_length( vector ):

    return math.sqrt( ( math.pow( vector.x, 2 ) ) + ( math.pow( vector.y, 2 ) ) + ( math.pow( vector.z, 2 ) ) )


# Get Vector Between Points
# Vector AB = B - A
def get_vector_between_points( pointA, pointB ):

    vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )
    vector.x = ( pointB.x - pointA.x )
    vector.y = ( pointB.y - pointA.y )
    vector.z = ( pointB.z - pointA.z )

    return vector


# Get Distance Between Points
# Gets vector between given points and returns its length
def get_distance_between_points( pointA, pointB ):

    distance_vector = get_vector_between_points( pointA, pointB )

    return get_vector_length( distance_vector )


# Get Vector From Angle
# Calculates vector from angle - returns Vector3
def get_vector_from_angle( angle ):

    vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )
    vector.x = ( math.sin( math.radians( angle ) ) )
    vector.z = ( math.cos( math.radians( angle ) ) )

    return vector


# Get Angle From Vector
# Uses atan2 to convert vector3 to an angle in degrees
def get_angle_from_vector( vector ):

    return math.degrees( math.atan2( vector.x, vector.z ) )
