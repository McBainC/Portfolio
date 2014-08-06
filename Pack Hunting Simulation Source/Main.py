# Name:        Python Main
# Author:      Chris McBain - May 2014
# Description:
# Contains 'run()' which is called through a Maya Expression
# Based on <crowd_sim_diy.py> by G Clarke
# Contains
# Main Simulation Code
# Requires - <Animal.py>, <Prey.py>, <Predator.py>, <Vector3.py>


# Imports #
import maya.cmds as cmds
import maya.mel as mel
import math


# Global Variables #


# Last Frame Number 
last_frame_number = 1

# The Herd
herd = []

# The herds target
herd_target = Target( targetName = "Herd_target", x = -30.0, y = 0.0, z = -30.0 )

# The distance to determine if an prey is nearby
herd_near_distance = 5

# The Pack
pack = []

# The distance to determine if a predator is nearby
pack_near_distance = 10

# The distance to determine if a nearby prey can be targetted
pack_target_distance = 15


# Main #


# Initialize
# This function is used to initialise the simulation
def init():

    print "Custom simulation initialised"

    # Reset last frame counter
    global last_frame_number
    last_frame_number = 1

    # Initialise the herd and pack
    create_herd()
    create_pack()


# Run
# This function is called every frame the simulation is run
def run( frame_number ):

    # Get the frame rate by using an internal MEL script
    frame_rate = mel.eval( "currentTimeUnitToFPS" )

    # Calculate the amount of time in seconds between each frame
    frame_time = 1.0 / frame_rate

    # Special case if we are on the first frame then initialise the simulation
    if( ( frame_number == 0 ) or ( frame_number == 1 ) ):
        init()

    # Check to see if we have an event to process this frame
    global last_frame_number
    if( frame_number - last_frame_number ) == 1:

        # Hunting Simulation 
        global herd
        global herd_near_distance
        global pack_bear_distance

        do_flocking_behaviour( herd, herd_near_distance, pack_near_distance )

        global pack
        global pack_target_distance 
        do_hunting_behaviour( pack, pack_target_distance )

        # Move the herd
        for prey in herd:

            prey.move( frame_time )

        # Move the pack
        for predator in pack:

            predator.move( frame_time )

        print "Custom simulation run successfully at frame: "+str( frame_number )

        # We have successfully completed a run of the simulation
        # Update the last frame number
        last_frame_number = frame_number


# Create Herd
# Takes global herd list and adds 10 Prey
# Prey's __init_- function is called after creation by Python
def create_herd():

    # Global herd
    global herd
    global herd_target

    # Reset the old herd if it exists
    if( len( herd ) ):

        reset_herd()

    # Delete then recreate herd target
    cmds.delete( herd_target.targetName )
    herd_target = Target( targetName = "Herd_target", x = -30.0, y = 0.0, z = -30.0 )

    # IF the herd is empty
    if( len( herd ) == 0 ):

        # Fill the herd - flagging the first as the leader
        herd.append( Prey( animalName = "Prey_1",  initialPosition = Vector3( x = 2.2,    y = 0.0, z = 3.28   ), initialHeading = 0, initialSpeed = 0.5, leader = True  ) )
        herd.append( Prey( animalName = "Prey_2",  initialPosition = Vector3( x = 0.0,    y = 0.0, z = 0.0    ), initialHeading = 0, initialSpeed = 0.5, leader = False ) )
        herd.append( Prey( animalName = "Prey_3",  initialPosition = Vector3( x = 3.643,  y = 0.0, z = -2.52  ), initialHeading = 0, initialSpeed = 0.5, leader = False ) )
        herd.append( Prey( animalName = "Prey_4",  initialPosition = Vector3( x = -5.054, y = 0.0, z = -4.208 ), initialHeading = 0, initialSpeed = 0.5, leader = False ) )
        herd.append( Prey( animalName = "Prey_5",  initialPosition = Vector3( x = 0.0,    y = 0.0, z = -9.868 ), initialHeading = 0, initialSpeed = 0.5, leader = False ) )
        herd.append( Prey( animalName = "Prey_6",  initialPosition = Vector3( x = 5.252,  y = 0.0, z = -5.856 ), initialHeading = 0, initialSpeed = 0.5, leader = False ) )
        herd.append( Prey( animalName = "Prey_7",  initialPosition = Vector3( x = -6.252, y = 0.0, z = 0.0    ), initialHeading = 0, initialSpeed = 0.5, leader = False ) )
        herd.append( Prey( animalName = "Prey_8",  initialPosition = Vector3( x = -2.132, y = 0.0, z = 2.649  ), initialHeading = 0, initialSpeed = 0.5, leader = False ) )
        herd.append( Prey( animalName = "Prey_9",  initialPosition = Vector3( x = 0.0,    y = 0.0, z = -5.383 ), initialHeading = 0, initialSpeed = 0.5, leader = False ) )
        herd.append( Prey( animalName = "Prey_10", initialPosition = Vector3( x = 12.128, y = 0.0, z = 0.0    ), initialHeading = 0, initialSpeed = 0.5, leader = False ) )


# Create Pack
# Creates the simulations pack
def create_pack():

    # Global pack
    global pack

    # Reset the old pack if it exists
    if( len( pack ) ):

        reset_pack()

    # If the pack is empty
    if( len( pack ) == 0 ):

        pack.append( Predator( animalName = "Predator_Center", initialPosition = Vector3( x = 20, y = 0, z = 20 ), initialHeading = -135, initialSpeed = 1.0, center = True ) )
        pack.append( Predator( animalName = "Predator_Wing", initialPosition = Vector3( x = 24, y = 0, z = 22 ), initialHeading = -135, initialSpeed = 1.0, center = False ) )


# Do Flocking Behaviour
# Checks for nearby agents then gets the flocking heading based on that list
def do_flocking_behaviour( herd, herd_distance, predator_distance ):

    # Bool to alert the whole herd
    herd_alerted = False

    # Loop through herd
    for prey in herd:

        # IF alive
        if( prey.state != AwarenessState.dead ):

            # Find the agents close by
            neighbours = find_neighbours_within_distance( prey, herd_distance )
            predators = find_predators_within_distance( prey, predator_distance )
            
            # IF there are nearby predators set flag
            if( len( predators ) ):
                herd_alerted = True

            # Update the prey
            prey.update( neighbours, predators )
            
            global herd_target
            prey_heading_vector = prey.get_flocking_heading( neighbours, predators, herd_target )

            # We now have the desired heading
            # We'll just set our heading to it for now
            # Better to stear nicely towards it
            prey_heading_angle = get_angle_from_vector( prey_heading_vector )
            prey.set_heading( prey_heading_angle )

    # IF alerted is flagged
    if( herd_alerted ):

        # Loop again
        for prey in herd:
    
            # Set whole herd alerted flag
            prey.isAlerted = True


# Do Hunting Behaviour
# Calculates herds center position and passes nearby prey for possible targetting
def do_hunting_behaviour( pack, target_distance ):

    target = 0

    # Loop through pack
    for predator in pack:

        # IF not busy eating
        if( predator.state != HuntingState.eating ):

            # Get list of prey
            herd = find_neighbours_within_distance( predator, target_distance )

            # Get the center of the herd
            herd_position = get_herd_average_position()

            # IF target prey has been set
            if( predator.target ):

                target = predator.target

            # Update the predator passing:
            # - the list of possible target prey ( herd )
            # - the current center of the herd ( herd_position )
            # - a target prey ( target ) which may or may not exist
            predator.update( herd, herd_position, target )

            # Get the hunting heading vector based on current information
            predator_heading_vector = predator.get_hunting_heading( herd_position, target )

            # We now have the desired heading
            # We'll just set our heading to it for now
            # Better to stear nicely towards it
            # Center behaviour
            if( predator.isCenter ):

                predator_heading_angle = get_angle_from_vector( predator_heading_vector )
                predator.set_heading( predator_heading_angle )

            # Wing behaviour
            # Align perendicular to target ( to circle )
            elif( predator.state == HuntingState.stalking ):

                predator_heading_angle = get_angle_from_vector( predator_heading_vector )
                predator.set_heading( predator_heading_angle - 90 )

            # Align towards target
            else:

                predator_heading_angle = get_angle_from_vector( predator_heading_vector )
                predator.set_heading( predator_heading_angle )


# Get Herd Average Position
# Returns the average position of the herd (the center)
def get_herd_average_position():

    # Initialise the vector
    herd_average_position_vector = Vector3( x = 0.0, y = 0.0, z = 0.0 )

    # Loop through the herd
    global herd
    for prey in herd:

        # Add each preys position
        herd_average_position_vector.add( prey.get_position() )

    # Divide by the length of the herd list to get average
    herd_average_position_vector.divide( len( herd ) )

    return herd_average_position_vector


# Find Neighbours Within Distance
# Returns a list of the neighbour agents who are within the passed distance
def find_neighbours_within_distance( agent_name, distance ):

    # Neighbours is a list that will contain a list of nearby prey
    neighbours = []

    # For every agent in the agents list [remember to use the global keyword for the agents list]
    global herd
    for prey in herd:

        # Calculate the distance between the potential neighbour and the agent called agent_name
        distance_length = get_distance_between_points( agent_name.get_position(), prey.get_position() )

        # If this distance is less than distance passed into the function [this is the distance that defines the agent "awareness circle"] then add it to the neighbours list
        if( ( distance_length < distance ) == True ):

            if( agent_name != prey ):

                neighbours.append( prey )

    return neighbours


# Find Predators Within Distance
# Returns a list of the predator agents who are within the passed distance
def find_predators_within_distance( agent_name, distance ):

    # Predators is a list that will contain a list of nearby predators
    predators = []

    # For every predator in the pack list [global]
    global pack
    for predator in pack:

        # Calculate the distance between the potential predator and the agent called agent_name
        distance_length = get_distance_between_points( agent_name.get_position(), predator.get_position() ) 

        # If this distance is less than distance passed into the function [this is the distance that defines the agent "awareness circle"] then add it to the predators list
        if( ( distance_length < distance ) == True ):

            if( agent_name != predator ):

                predators.append( predator )

    return predators


# Reset Herd
# Returns all the herd to their starting positions
def reset_herd():

    # herd is global
    global herd

    # If herd exists
    if( len( herd ) ):

        # Loop through the herd
        for agent in herd:
        
            # Reset prey attributes to initial values
            cmds.setAttr( agent.animalName+".translateX", cmds.getAttr( agent.animalName+".initialPositionX" ) )
            cmds.setAttr( agent.animalName+".translateY", cmds.getAttr( agent.animalName+".initialPositionY" ) )
            cmds.setAttr( agent.animalName+".translateZ", cmds.getAttr( agent.animalName+".initialPositionZ" ) )
            cmds.setAttr( agent.animalName+".rotateY", cmds.getAttr( agent.animalName+".initialHeading" ) )
            cmds.setAttr( agent.animalName+".rotateX", 0 )
            cmds.setAttr( agent.animalName+".speed", cmds.getAttr( agent.animalName+".initialSpeed" ) )

            # Reset awareness state
            agent.state = AwarenessState.grazing

            # Reset state based attributes
            agent.set_speed_and_colour()


# Reset Pack
# Returns all the pack to their starting positions
def reset_pack():

    # pack is global
    global pack

    # IF pack exists
    if( len( pack ) ):

        # Loop through the pack
        for agent in pack:

            # Reset local position and heading to initial values
            cmds.setAttr( agent.animalName+".translateX", cmds.getAttr( agent.animalName+".initialPositionX" ) )
            cmds.setAttr( agent.animalName+".translateY", cmds.getAttr( agent.animalName+".initialPositionY" ) )
            cmds.setAttr( agent.animalName+".translateZ", cmds.getAttr( agent.animalName+".initialPositionZ" ) )
            cmds.setAttr( agent.animalName+".rotateY", cmds.getAttr( agent.animalName+".initialHeading" ) )
            cmds.setAttr( agent.animalName+".speed", cmds.getAttr( agent.animalName+".initialSpeed" ) )

            # Reset class specific variables
            agent.target = 0
            agent.wingAngle = 0

            # Reset awareness state
            agent.state = HuntingState.prowling

            # Reset state based attributes
            agent.set_speed_and_colour()
