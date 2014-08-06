# Name:        Python Target, AwarenessState & Prey Classes
# Author:      Chris McBain - May 2014
# Description:
# Target - basic class to represent a position within scene
# AwarenessState - class to take on the form of an enumerated state
# Prey - inherits from Animal class
#      - contains main herding logic
# Based on <crowd_sim_diy.py> by G Clarke
# Requires -  <Animal.py>


# Imports #
import maya.cmds as cmds
import maya.mel as mel
import math


# Target Class #
# Position within the scene to act as a target location
# Represented by a purple cube - does not reset with scene
class Target( object ):

    # Init
    def __init__( self, *args, **kwargs ):

        self.targetName = kwargs.setdefault( "targetName" )

        # Create a cube to represent the target
        self.cube = cmds.polyCube( name = self.targetName, axis = [ 0, 1, 0 ], height = 2, width = 2, depth = 2 )

        # Set an initial position
        cmds.setAttr( self.targetName+".translateX", kwargs.setdefault( "x" ) )
        cmds.setAttr( self.targetName+".translateY", kwargs.setdefault( "y" ), lock = True )
        cmds.setAttr( self.targetName+".translateZ", kwargs.setdefault( "z" ) )

        # Purple cube
        cmds.select( self.targetName, visible = True )
        cmds.polyColorPerVertex( colorRGB = [ 1, 0, 1 ], colorDisplayOption = True )


    # Get Position
    # Returns target position as a Vector3
    def get_position( self ):

        # Create and return position Vector3 using objects position
        position =  Vector3( x = cmds.getAttr( self.targetName+".translateX" ), y = cmds.getAttr( self.targetName+".translateY" ), z = cmds.getAttr( self.targetName+".translateZ" ) )
        return position


# State Class #
# States to control Prey objects behavior
class AwarenessState:

    dead, grazing, separated, running = range( 4 )


# Prey Class #
# Representation of Prey
# Inherits from Animal Class
class Prey( Animal ):

    # Init
    def __init__( self, *args, **kwargs ):

        # Call animal constructor
        Animal.__init__( self, *args, **kwargs )

        # Define weightings #
        # Flocking weights are averaged
        self.separationWeight = 0.33333
        self.cohesionWeight = 0.33333
        self.alignmentWeight = 0.33333

        # Threat initialized to zero
        self.threatWeight = 0.0

        # Create Awareness State and initialize to Grazing
        self.state = AwarenessState.grazing

        # Set leader flag
        self.isLeader = kwargs.setdefault( "leader" )

        # Set alerted flag
        self.isAlerted = False

        # Select then change the cones colour to green
        cmds.select( self.animalName, visible = True )

        # Set initial prey colour
        if self.isLeader is False:

            cmds.polyColorPerVertex( colorRGB = [ 0, 1, 0 ], colorDisplayOption = True )

        else:

            cmds.polyColorPerVertex( colorRGB = [ 0, 0.5, 0 ], colorDisplayOption = True )


    # Set Weights
    # Calculate then set movement weights
    def set_weights( self, threat, separated, leaderless ):

        # Set threat weight
        self.threatWeight = threat

        # IF separated
        if( separated ):

            # 0% weighting on separation
            # Therefore 100% scaling on alignment
            self.separationWeight = 0.0
            self.cohesionWeight = 0.0

        # IF leaderless
        elif( leaderless ):

            # Normal weightings
            self.separationWeight = 0.33333
            self.cohesionWeight = 0.33333

        # ELSE 'normal behaviour'
        else:

            # Cohesion weight scales from a 3rd to zero based on threat weight
            self.cohesionWeight = self.threatWeight / 3.0

            # Separation weight scales from 0.5 (max) to 0.33333 (min)
            self.separationWeight = ( 1 - self.cohesionWeight ) / 2 

        # Alignment weight is the remainder from 1.0 (max) to ~0.33333 (min)
        self.alignmentWeight = ( 1 - self.separationWeight ) - self.cohesionWeight


    # Update
    # Sets the prey's state based on passed lists
    def update( self, neighbours, predators ):

        # Get current state
        current_state = self.state

        # Initialise threat
        threat = 1.0

        # IF any predators have been passed
        if( len( predators ) or self.isAlerted ):

            # Prey starts running
            self.state = AwarenessState.running

            # Loop through them
            for predator in predators:

                distance = distance_length = get_distance_between_points( self.get_position(), predator.get_position() )
                new_threat = distance / 10.0
                if( new_threat < threat ):
                    threat = new_threat

                if( self.has_been_eaten_by( predator ) ):
            
                    self.state = AwarenessState.dead

            # Set weights with current threat weighting 
            self.set_weights( threat, False, False )

        # IF no predators but there ARE neighbours AND a leader
        #elif( len( neighbours ) and self.contains_leader( neighbours ) ):
        elif( len( neighbours ) ):

            # 'Normal' grazing state
            self.state = AwarenessState.grazing

            # Set weights with current threat weighting 
            self.set_weights( threat, False, False )

        # No predators or neighbours = separated
        else:

            # Separated state
            self.state = AwarenessState.separated

            # Set weights with current threat weighting and flag as separated 
            self.set_weights( threat, True, False )

        # Check for state change
        if( self.state != current_state ):

            self.set_speed_and_colour()

        # Reset flag
        self.isAlerted = False


    # Contains Leader
    # Bool function to return if neighbour list contains the leader
    def contains_leader( self, neighbours ):

        # Loop through neighbours list
        for neighbour in neighbours:

            # IF one is flagged as a leader
            if( neighbour.isLeader ):

                return True

        return False


    # Set Speed And Colour
    # Sets the preys speed and colour based on the current state
    def set_speed_and_colour( self ):

        # RGB colour
        colour = 1.0

        # Darker colour for leader
        if( self.isLeader ):
            colour = 0.5

        # Dead = Red
        # Also rotating cone to point downward
        if( self.state == AwarenessState.dead ):

            cmds.setAttr( self.animalName+".speed", 0.0 )
            cmds.setAttr( self.animalName+".rotateX", 90 )
            self.change_shape_colour( colour, 0, 0 )

        # Grazing = Green
        elif( self.state == AwarenessState.grazing ):

            cmds.setAttr( self.animalName+".speed", 0.2 )
            self.change_shape_colour( 0, colour, 0 )

        # Separated = Yellow
        elif( self.state == AwarenessState.separated ):

            cmds.setAttr( self.animalName+".speed", 1.0 )
            self.change_shape_colour( colour, colour, 0 )

        # Running = Orange
        elif( self.state == AwarenessState.running ):

            cmds.setAttr( self.animalName+".speed", 1.5 )
            self.change_shape_colour( colour, ( colour / 2 ), 0 )

        # Error!
        else:

            print "Error in set_speed_and_colour for -" + self.animalName


    # Has Been Eaten By <insert predator>
    # Bool function that measures distance between object and passed predator
    def has_been_eaten_by( self, predator ):

        # Get distance between prey and predator
        distance_length = get_distance_between_points( self.get_position(), predator.get_position() )
        
        # True
        if( distance_length < 1 ):

            return True

        else:

            return False


    # Get Flocking Heading
    # Using the passed list of neighbour agents caluclates the three heading vectors
    # Then combines them using their weightings for the final flocking heading
    def get_flocking_heading( self, neighbours, predators, target ):

        # Initialise the heading vector
        heading_vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )

        # Get the three heading vectors
        separation_heading_vector = self.get_separation_heading( neighbours )
        cohesion_heading_vector = self.get_cohesion_heading( neighbours )

        # IF leader or separated use target to calculate alignment heading
        if( self.isLeader or self.state == AwarenessState.separated ):

            # Heading is vector from this prey to target position
            alignment_heading_vector = get_vector_between_points( self.get_position(), target.get_position() )

            # Normalise it for direction vector
            alignment_heading_vector.normalise()  

        # ELSE use alignment rule
        else:

            alignment_heading_vector = self.get_alignment_heading( neighbours )

        # Get the threat heading vector
        threat_heading_vector = self.get_threat_heading( predators )

        # Scale alignment and threat heading vectors due to threat weight
        alignment_heading_vector.scale( ( 1.0 - ( 1.0 - self.threatWeight ) ) )
        threat_heading_vector.scale( ( 1.0 - self.threatWeight ) )
        
        # Add alignment and threat for final direction vector
        alignment_heading_vector.add( threat_heading_vector )

        # Reusing get separtion heading vector but passing predators
        threat_separation_heading_vector = self.get_separation_heading( predators )

        # Scale threat and separation heading vectors due to threat weight
        separation_heading_vector.scale( ( 1.0 - ( 1.0 - self.threatWeight ) ) )
        threat_separation_heading_vector.scale( ( 1.0 - self.threatWeight ) )

        # Add threat separation for final separation vector
        separation_heading_vector.add( threat_separation_heading_vector )

        # Apply the weighting to each heading vector and then add them altogether
        separation_heading_vector.scale( self.separationWeight )
        cohesion_heading_vector.scale( self.cohesionWeight )
        alignment_heading_vector.scale( self.alignmentWeight )

        # Combine to get the overall heading vector
        heading_vector.add( separation_heading_vector )
        heading_vector.add( cohesion_heading_vector )
        heading_vector.add( alignment_heading_vector )

        # Return result
        return heading_vector


    # Get Alignment Heading
    # Returns the average alignment heading based on how many neighbour there are
    def get_alignment_heading( self, neighbours ):

        # Initialise the heading vector that will hold the alignment heading
        alignment_heading_vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )

        # Get the number of neighbouring agents in the neighbours list [find the length of the list]
        num_of_neighbours = len( neighbours )

        # If we have 1 or more agents in the neighbours list
        if( num_of_neighbours > 0 ):

            # For every neighbour in the neighbours list
            for neighbour in neighbours:

                # Get the heading of the neighbour as a vector then add this heading to the alignment_heading_vector
                alignment_heading_vector.add( neighbour.get_heading_vector() )

            # Calculate the averaged alignment_heading_vector
            alignment_heading_vector.divide( num_of_neighbours )

            # Normalise alignment vector
            alignment_heading_vector.normalise()

        return alignment_heading_vector


    # Get Separation Heading
    def get_separation_heading( self, neighbours ):

        # Initialise the heading vector that will hold the separation heading
        separation_heading_vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )

        # Get the number of neighbouring agents in the neighbours list [find the length of the list]
        num_of_neighbours = len( neighbours )

        # IF we have 1 or more agents in the neighbours list
        if( num_of_neighbours > 0 ):

            # For every neighbour in the neighbours list
            for neighbour in neighbours:

                # Calculate the heading vector from the neighbours position to the position of the agent called agent_name
                heading_vector = get_vector_between_points( neighbour.get_position(), self.get_position() )
    
                # Normalise the heading vector to give it a size of 1 [needed to averaging the vector will work as expected]
                heading_vector.normalise()
    
                # Add this heading vector to the separation_heading_vector
                separation_heading_vector.add( heading_vector )

            # Calculate the averaged separation_heading_vector
            separation_heading_vector.divide( num_of_neighbours )

            # Normalise the separation_heading_vector
            separation_heading_vector.normalise()

        return separation_heading_vector


    # Get Cohesion Heading
    def get_cohesion_heading( self, neighbours ):

        # Initialise the heading vector that will hold the separation heading
        cohesion_heading_vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )

        # Get the number of neighbouring agents in the neighbours list [find the length of the list]
        num_of_neighbours = len( neighbours )

        # If we have 1 or more agents in the neighbours list
        if( num_of_neighbours > 0 ):

            # Initialise a vector to hold the average of all the neighbours positions to [0, 0, 0]
            average_neighbour_position_vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )

            # For every neighbour in the neighbours list
            for neighbour in neighbours:

                # Add it to the vector to hold the average positions
                average_neighbour_position_vector.add( neighbour.get_position() )

            # Calculate the averaged neighbours position
            average_neighbour_position_vector.divide( num_of_neighbours )

            # Calculate the cohesion_heading_vector from the position of the agent called agent_name to the averaged neighbours position
            cohesion_heading_vector = get_vector_between_points( self.get_position(), average_neighbour_position_vector )

            # Normalise the cohesion_heading_vector
            cohesion_heading_vector.normalise()

        return cohesion_heading_vector


    # Get Threat Heading
    # Takes in a list of all nearby predators and calculates the sum of their distances
    # Then it creates a direction vector from the predator to the prey (to run away)
    # And scales each based on their distance - closer = higher threat
    def get_threat_heading( self, predators ):

        # Initialise vector3 that will hold the threat heading vector
        threat_heading_vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )

        # Get number of passed predators
        num_of_predators = len( predators )

        # IF there are one or more predators
        if( num_of_predators > 0 ):

            # Initialise variable to hold sum of distances
            sum_of_distances = 0.0

            # Loop through predators - to calculate sum of distances
            for predator in predators:

                # Get distance between prey and predator and add to sum
                sum_of_distances += get_distance_between_points( self.get_position(), predator.get_position() )

            # Loop through predators again
            for predator in predators:

                # Get distance again
                distance = get_distance_between_points( self.get_position(), predator.get_position() )

                # Divide distance by sum then subtract from one to get the weight based on its distance
                distance_weight = 1.0 - ( distance / sum_of_distances )

                # Calculate the heading vector from the predator position to the position of the prey
                heading_vector = get_vector_between_points( predator.get_position(), self.get_position() )
                #heading_vector = get_vector_between_points( self.get_position(), predator.get_position() )

                # Normalise it for a direction vector
                heading_vector.normalise()

                # Scale it by its distance weight
                heading_vector.scale( distance_weight )

                # Add to thret heading vector
                threat_heading_vector.add( heading_vector )
        
            # Divide by num of predators for averaged direction
            threat_heading_vector.divide( num_of_predators )
            
            # Normalise again
            threat_heading_vector.normalise()

        return threat_heading_vector
