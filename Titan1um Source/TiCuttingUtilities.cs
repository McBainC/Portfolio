#region Details
// File Name: Titanium Screen To World
// File Description: 
// Static Utility Functions:
// - to convert screen gestures into the scene
// - to determine if bounds are within two points 
//
// Creator: Chris McBain
// Contributors:
// Changes:
// 10-03-14 First Created
// 07-04-14 CreateExplosion moved to GameController as Instantiate cannot be called from non-GameObject...
#endregion

#region Includes
using UnityEngine;
using System.Collections;
using System.Collections.Generic;
#endregion

// TiCuttingUtilities //
public class TiCuttingUtilities
{
	// ConvertScreenToWorld //
	// Converts Vector3's from screen to world space - by raycasting from the passed camera //
	// to a plane parallel to the camera, outputs an array of 2 Vector3's                   //
	public static Vector3[] ConvertScreenToWorld( Vector3 point_a, Vector3 point_b, Vector3 position, Camera camera )
	{
		// Convert screen touches into rays from main camera
		Ray ray_a = camera.ScreenPointToRay( point_a );
		Ray ray_b = camera.ScreenPointToRay( point_b );

		// Create a plane parallel to camera, facing the screen - then rotated by the cameras y rotation
		Vector3 parallel_to_camera = RotateY( new Vector3( 0, 0, -1 ), camera.transform.rotation.y );
		Plane plane = new Plane( parallel_to_camera, position );
		
		// Hit distances
		float hit1;
		float hit2;
		
		// Hit point Vector3's
		Vector3 hit_point_a = Vector3.zero;
		Vector3 hit_point_b = Vector3.zero;
		
		// Shoot ray_a at plane
		if( plane.Raycast( ray_a, out hit1 ) )
		{
			// Get where ray_a collides with the plane
			hit_point_a = ray_a.GetPoint( hit1 );
		}
		
		// Shoot ray_b at plane
		if( plane.Raycast( ray_b, out hit2 ) )
		{
			// Get where ray_b collides with the plane
			hit_point_b = ray_b.GetPoint( hit2 );
		}

		// Return two vectors as array
		return new Vector3[ 2 ] { hit_point_a, hit_point_b };
	}

	
	// BoundsWithinPoints                                                                                      //
	// Cchecks if passed bounds contains either point (return false)                                           //
	// Then Fires rays between both points to check if the bounds are contained within both rays (return true) //
	public static bool BoundsWithinPoints( Vector3 point_a, Vector3 point_b, Bounds bounds )
	{
		// If the start or end point are within the object, do nothing
		if( bounds.Contains( point_a ) || bounds.Contains( point_b ) ) 
		{
			return false;
		}

		// If the object is fully on both rays  
		Ray ray_forward = new Ray( point_a, point_b - point_a );
		Ray ray_backward = new Ray( point_b, point_a - point_b );
		if( bounds.IntersectRay( ray_forward ) && bounds.IntersectRay( ray_backward )) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	// CreateExplosiveForce //
	// Explosions are good! //
	public static void CreateExplosiveForce( Vector3 source, float hit_radius, float explosive_force, float explosive_radius )
	{
		// Get all affected colliders
		Collider[] colliders = Physics.OverlapSphere( source, hit_radius );
		
		// Loop through colliders
		foreach( Collider hits in colliders ) 
		{
			// Add force
			if( hits && hits.rigidbody )
			{
				hits.rigidbody.AddExplosionForce( explosive_force, source, explosive_radius, 0.0f );
			}
		}
	}


	// RotateY                         //
	// Rotates a Vector3 in the Y axis //
	public static Vector3 RotateY( Vector3 vector, float angle )
	{
		Vector3 temp = new Vector3();
		
		float sin = Mathf.Sin( angle );
		float cos = Mathf.Cos( angle );
		
		float tx = vector.x;
		float tz = vector.z;
		
		temp.x =( cos * tx ) + ( sin * tz );
		temp.z =( cos * tz ) - ( sin * tx );	
		
		return temp;
	}
}