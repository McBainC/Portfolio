# Name:        Python HuntingState & Predator Classes
# Author:      Chris McBain - May 2014
# Description:
# HuntingState - class to take on the form of an enumerated state
# Predator - inherits from Animal class
# Represented by a scaled up Orange polyCone
# Basic implementation to support a pair of predators - one flagged as the 'center' leaving the other as the 'wing'
# Will seek out the prey, choose the closest target - then the center will slowly approach, while the wing flanks around
# After covering 180 degrees the wing will close in towards the target in an attempt to drive the prey towards the center
# Requires -  <Animal.py> 


# Imports #
import maya.cmds as cmds
import maya.mel as mel
import math


# State Class #
# States to control Predator objects behavior
class HuntingState:

    eating, prowling, stalking, chasing = range( 4  )


# Predator Class #
# Representation of Predatpr
# Inherits from Animal Class
class Predator( Animal ):

    # Init
    def __init__( self, *args, **kwargs ):

        # Call animal constructor
        Animal.__init__( self, *args, **kwargs )

        # Flag for center or wing
        self.isCenter = kwargs.setdefault( "center" )

        # Create Awareness State and initialize to Grazing
        self.state = HuntingState.prowling

        # Has a target prey flag
        self.hasTarget = False

        # Target prey - this will hold a <Prey> object once set, still dont know about this dynamic typing...
        self.target = 0

        # Wing angle of attack
        self.wingAngle = 0

        # Select then change the cones colour to orange
        cmds.select( self.animalName, visible = True )
        cmds.polyColorPerVertex( colorRGB = [ 1, 1, 0 ], colorDisplayOption = True )

        # Scale polyCone by 150% for Predator
        cmds.setAttr( self.animalName+".scaleX", 1.5 )
        cmds.setAttr( self.animalName+".scaleY", 1.5 )
        cmds.setAttr( self.animalName+".scaleZ", 1.5 )


    # Update
    # Sets the predator's state based on passed lists
    def update( self, herd, herd_position, target ):

        # Get current state
        current_state = self.state

        # Get distance from the herd_position vector3
        distance_to_herd = get_distance_between_points( self.get_position(), herd_position )

        # Center Behaviour
        if( self.isCenter ):

            # If within appropiate distance
            if( distance_to_herd < 20 ):

                # Now stalking
                self.state = HuntingState.stalking

                # If there are nearby prey
                if( len( herd ) ):

                    # Initialise distance
                    target_distance = 30

                    # Loop through them
                    for prey in herd:

                        # Get distance between predator and each prey
                        distance_to_prey = get_distance_between_points( self.get_position(), prey.get_position() )

                        # IF less than current target distance
                        if( distance_to_prey < target_distance ):

                            # Set this as new target distance
                            target_distance = distance_to_prey

                            # Set this prey at target
                            self.target = prey

                        # Once target gets close enough
                        if( target_distance < 7 ):

                            self.state = HuntingState.chasing

                        # Kill condition
                        if( self.has_killed( prey ) ):

                            self.state = HuntingState.eating

        # Wing behaviour
        else:

            # IF a target has been passed
            if( target ):

                # IF the target has changed, recalculate wingAngle 
                if( self.target != target ):
                    self.target = target
                    self.state = HuntingState.stalking
                    #self.wingAngle = get_angle_from_vector( get_vector_between_points( self.get_position(), target.get_position() ) )
                    self.wingAngle = get_angle_from_vector( get_vector_between_points( self.get_position(), herd_position ) )

                # Loop through herd
                for prey in herd:

                    # Kill condition
                    if( self.has_killed( prey ) ):

                        self.state = HuntingState.eating

                # Get current angle to target
                #new_wing_angle = get_angle_from_vector( get_vector_between_points( self.get_position(), target.get_position() ) )
                new_wing_angle = get_angle_from_vector( get_vector_between_points( self.get_position(), herd_position ) )

                # After +- 180 degrees = time to push towards center
                if( ( new_wing_angle - 180 ) > self.wingAngle ):

                    self.state = HuntingState.chasing 

                elif( ( new_wing_angle + 180 ) < self.wingAngle ):

                    self.state = HuntingState.chasing

        # Check for state change
        if( self.state != current_state ):

            self.set_speed_and_colour()


    # Set Speed And Colour
    # Sets the predators speed and colour based on the current state
    def set_speed_and_colour( self ):

        # RGB colour
        colour = 1.0
 
        # Darker colour for center
        if( self.isCenter ):
            colour = 0.5
 
        # Eating = Black
        if( self.state == HuntingState.eating ):

            cmds.setAttr( self.animalName+".speed", 0.0 )
            self.change_shape_colour( 0, 0, 0 )

        # Prowling = Yellow
        elif( self.state == HuntingState.prowling ):

            cmds.setAttr( self.animalName+".speed", 1.5 )
            self.change_shape_colour( colour, colour, 0 )

        # Stalking = Orange
        elif( self.state == HuntingState.stalking ):

            if( self.isCenter ):

                # Center moves slowly
                cmds.setAttr( self.animalName+".speed", 0.5 )

            else:

                # Wing flanks around
                cmds.setAttr( self.animalName+".speed", 4.0 )

            self.change_shape_colour( colour, ( colour / 2 ), 0 )

        # Chasing = Red
        elif( self.state == HuntingState.chasing ):

            cmds.setAttr( self.animalName+".speed", 4.0 )
            self.change_shape_colour( colour, 0, 0 )

        # Error!
        else:

            print "Error in set_speed_and_colour for -" + self.animalName


    # Has Killed <insert prey>
    # Bool function that measures distance between object and passed prey
    def has_killed( self, prey ):

        # Get distance between prey and predator
        distance_length = get_distance_between_points( self.get_position(), prey.get_position() )
        
        # True
        if( distance_length < 1 ):

            return True

        else:

            return False


    # Get Hunting Heading
    # Returns a vector towards either the passed herd position vector3
    # Or the target if in the stalking or chasing states
    def get_hunting_heading( self, herd_position, target ):

        # Initialise the heading vector
        hunting_vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )

        # IF in prowling state ( no target has been set )
        #if( self.state == HuntingState.prowling ):
        if( target == 0 ):

            # Vector towards the herd position
            hunting_vector = get_vector_between_points( self.get_position(), herd_position )
 
            # Normalise it 
            hunting_vector.normalise()

        # ELSE - in stalking or chasing states
        #elif( ( self.state == HuntingState.stalking ) or ( self.state== HuntingState.chasing ) ):
        else:

            # Vector towards the target 
            hunting_vector = get_vector_between_points( self.get_position(), target.get_position() )

            # Normalise it 
            hunting_vector.normalise()

        # Return result
        return hunting_vector
