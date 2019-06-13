#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include <shader.hpp>
#include <rc_spline.h>

struct Orientation {
	// Front
	glm::vec3 Front;
	// Up
	glm::vec3 Up;
	// Right
	glm::vec3 Right;
	// origin
	glm::vec3 origin;
};

class Track
{
public:

	// VAO
	unsigned int VAO;

	//VAOPlank
	unsigned int VAOPlank;

	// Control Points Loading Class for loading from File
	rc_Spline g_Track;

	// Vector of control points
	std::vector<glm::vec3> controlPoints;

	// Track data
	std::vector<Vertex> vertices;
	std::vector<Vertex> vertices_plank;
	std::vector<Orientation> camera; 
	// indices for EBO
	std::vector<unsigned int> indices;

	// hmax for camera
	float hmax = 0.0f;

	// the gap between interpolated points
	const float uGap = 0.05f;

	const float g_tau = 0.5f;
	const float railGap = 0.3f;
	const float cameraHeight = 4.0f;

	// constructor, just use same VBO as before, 
	Track(const char* trackPath)
	{
		// load Track data
		load_track(trackPath);

		create_track();

		setup_track();

		setup_track_plank();
	}

	// render the mesh
	void Draw(Shader shader, unsigned int textureID, unsigned int texturePlank)
	{
		/*
			Draw the objects here.
		*/
		// You must:
		// -  active proper texture unit before binding
		// -  bind the texture
		// -  draw mesh (using GL_TRIANGLES is the most reliable way)
		glm::mat4 model_track;

		// Bind new textures to both texture positions (do both since it has 2 textures in the vertex shader)
		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, textureID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set model in shaders


		shader.setMat4("model", model_track);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);

		////////////////////////////PLANK///////////////////////////////////////////
		// Bind new textures to both texture positions (do both since it has 2 textures in the vertex shader)

		glActiveTexture(GL_TEXTURE0);

		glBindTexture(GL_TEXTURE_2D, texturePlank);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texturePlank);

		shader.setMat4("model", model_track);
		glBindVertexArray(VAOPlank);
		glDrawArrays(GL_TRIANGLES, 0, vertices_plank.size());

		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

	// given an s float, find the point
	//  S is defined as the distance on the spline, so s=1.5 is the at the halfway point between the 1st and 2nd control point
	glm::vec3 get_point(float s)
	{
		float sDecimal = (float)s - (int)s;
		int pA = ((int)s) - 1;
		int pB = ((int)s);
		int pC = ((int)s) + 1;
		int pD = ((int)s) + 2;

		return interpolate(controlPoints[pA], controlPoints[pB], controlPoints[pC], controlPoints[pD], 0.5f, sDecimal);
	}


	void delete_buffers()
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}

private:
	

	/*  Render data  */
	unsigned int VBO, EBO;
	unsigned int VBOplank, EBOplank;

	void load_track(const char* trackPath)
	{
		// Set folder path for our projects (easier than repeatedly defining it)
		g_Track.folder = "../Project_2/Media/";

		// Load the control points
		g_Track.loadSplineFrom(trackPath);

	}

	// Implement the Catmull-Rom Spline here
	//     Given 4 points, a tau and the u value 
	//     Since you can just use linear algebra from glm, just make the vectors and matrices and multiply them.  
	//     This should not be a very complicated function
	glm::vec3 interpolate(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, glm::vec3 pointD, float tau, float u)
	{
		glm::mat4 catmull_rom(
			0, -tau, 2 * tau, -tau,
			1, 0, tau - 3, 2 - tau,
			0, tau, 3 - (2 * tau), tau - 2,
			0, 0, -tau, tau);

		glm::mat4 pointsABCD(
			pointA.x, pointB.x, pointC.x, pointD.x,
			pointA.y, pointB.y, pointC.y, pointD.y,
			pointA.z, pointB.z, pointC.z, pointD.z,
			0, 0, 0, 0);

		glm::vec4 uvector(1, u, u*u, u*u*u);

		glm::vec3 result = uvector * catmull_rom * pointsABCD;

		// Just returning the first point at the moment, you need to return the interpolated point.  
		return result;
	}

	// Here is the class where you will make the vertices or positions of the necessary objects of the track (calling subfunctions)
	//  For example, to make a basic roller coster:
	//    First, make the vertices for each rail here (and indices for the EBO if you do it that way).  
	//        You need the XYZ world coordinates, the Normal Coordinates, and the texture coordinates.
	//        The normal coordinates are necessary for the lighting to work.  
	//    Second, make vector of transformations for the planks across the rails
	void create_track()
	{
		// Create the vertices and indices (optional) for the rails
		//    One trick in creating these is to move along the spline and 
		//    shift left and right (from the forward direction of the spline) 
		//     to find the 3D coordinates of the rails.

		// Create the plank transformations or just creating the planks vertices
		//   Remember, you have to make planks be on the rails and in the same rotational direction 
		//       (look at the pictures from the project description to give you ideas).  

		// Here is just visualizing of using the control points to set the box transformatins with boxes. 
		//       You can take this code out for your rollercoster, this is just showing you how to access the control points
		
		glm::vec3 currentpos = glm::vec3(-2.0f, 0.0f, -4.5f);
		/* iterate throught  the points	g_Track.points() returns the vector containing all the control points */
		for (pointVectorIter ptsiter = g_Track.points().begin(); ptsiter != g_Track.points().end(); ptsiter++)
		{
			/* get the next point from the iterator */
			glm::vec3 pt(*ptsiter);

			// Print the Box
			std::cout << pt.x << "  " << pt.y << "  " << pt.z << std::endl;

			/* now just the uninteresting code that is no use at all for this project */
			currentpos += pt;
			//  Mutliplying by two and translating (in initialization) just to move the boxes further apart.  
			controlPoints.push_back(currentpos*2.0f);
		}
		//glm::vec3 prev_interpolated_point;
		//glm::vec3 cur_interpolated_point;

		Orientation ori_prev;
		Orientation ori_cur;
		//first point
		ori_cur.origin = controlPoints[1] + glm::vec3(1.7f, 0, 0); //Pn
		ori_cur.Up    = glm::vec3(0.0f, 1.0f, 0.0f);	//B(t)
		ori_cur.Front = glm::vec3(0.0f, 0.0f, 1.0f);	//T(t)
		ori_cur.Right = glm::vec3(1.0f, 0.0f, 0.0f);	//N(t)
		std::cout << "Control points size: " << controlPoints.size() << std::endl;
		for (int i = 1; i < (controlPoints.size()-3); i++) { 
			for (float u = 0; u < 1; u += uGap) {
				//std::cout << "i = "<< i << " u = " << u << "/n";
				//setting the previous as current
				ori_prev.origin = ori_cur.origin;
				ori_prev.Up = ori_cur.Up;
				ori_prev.Front = ori_cur.Front;
				ori_prev.Right = ori_cur.Right;

				//updating the current
				ori_cur.origin = interpolate(controlPoints[i], controlPoints[i + 1], controlPoints[i + 2], controlPoints[i + 3], g_tau, u);
				ori_cur.Front = glm::normalize((ori_cur.origin - ori_prev.origin));
				ori_cur.Right = glm::normalize(glm::cross(ori_prev.Up, ori_cur.Front));
				ori_cur.Up = glm::normalize(glm::cross(ori_cur.Front, ori_cur.Right));
				makePlankPart(ori_prev, ori_cur, glm::vec2(0, 0), true);
				//create the rail between the two orientations
				makeRailPart(ori_prev, ori_cur, glm::vec2(0, 0));
				if ((u >= 0.45 && u <=0.5) || (u >= 0.2 && u <= 0.24) || (u >= 0.74 && u <= 0.79))
				{
					makePlankPart(ori_prev, ori_cur, glm::vec2(0, 0), false);
				}
			} 
			if ((i % 1 == 0) && ori_prev.Up.y>0 && i !=97 && i !=99 && i!=186 && i!=187 && i !=183 && i !=209 && i !=213 && i !=214 && i!=237 && i != 238 && i != 239 && i != 240 && i != 241 && !((i<266 ) && (i > 255)) && i != 272 && i !=306)
			{
				makePillars(ori_prev, ori_cur, glm::vec2(0, 0));
			}
			makePlankPart(ori_prev, ori_cur, glm::vec2(0, 0), false);
		}
	}

	Vertex make_vertex(glm::vec3 myPoint, int a)
	{
		Vertex myVertex;

		myVertex.Position.x = myPoint.x;
		myVertex.Position.y = myPoint.y;
		myVertex.Position.z = myPoint.z;

		if (a == 1){
			myVertex.TexCoords.x = 0;
			myVertex.TexCoords.y = 1;
		}
		else if (a == 2) {
			myVertex.TexCoords.x = 0;
			myVertex.TexCoords.y = 0;
		}
		else {
			myVertex.TexCoords.x = 1;
			myVertex.TexCoords.y = 0;
		}
		return myVertex;
	}

	// Given 3 Points, create a triangle and push it into vertices (and EBO if you are using one)
		// Optional boolean to flip the normal if you need to
	void make_triangle(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, bool flipNormal)
	{
		//making vertex for each point
		Vertex vertexA = make_vertex(pointA, 1);
		Vertex vertexB = make_vertex(pointB, 2);
		Vertex vertexC = make_vertex(pointC, 3);

		//making the normals for each vertex
		set_normals(vertexA, vertexB, vertexC);
		if (flipNormal) {
			vertexA.Normal = -vertexA.Normal;
			vertexB.Normal = -vertexB.Normal;
			vertexC.Normal = -vertexC.Normal;
		}

		//pushing all the 3 vertex into vertices
		vertices.push_back(vertexA);
		vertices.push_back(vertexB);
		vertices.push_back(vertexC);
	}

	void make_triangle_plank(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC, bool flipNormal)
	{
		//making vertex for each point
		Vertex vertexA = make_vertex(pointA, 1);
		Vertex vertexB = make_vertex(pointB, 2);
		Vertex vertexC = make_vertex(pointC, 3);

		//making the normals for each vertex
		set_normals(vertexA, vertexB, vertexC);
		if (flipNormal) {
			vertexA.Normal = -vertexA.Normal;
			vertexB.Normal = -vertexB.Normal;
			vertexC.Normal = -vertexC.Normal;
		}

		//pushing all the 3 vertex into vertices
		vertices_plank.push_back(vertexA);
		vertices_plank.push_back(vertexB);
		vertices_plank.push_back(vertexC);
	}

	void makePillars(Orientation ori_prev, Orientation ori_cur, glm::vec2 offset) {
		glm::vec3 p1a = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) - (float(uGap) * 3 * ori_prev.Up) - (0.4f*ori_prev.Up);
		glm::vec3 p2a = ori_cur.origin - (float(uGap) / 1.5f  * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(uGap) * 3 * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p3a = ori_cur.origin - (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) - (float(uGap) * 3 * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p4a = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) - (float(uGap) * 3 * ori_prev.Up) - (0.4f*ori_prev.Up);

		glm::vec3 p1b = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) - (float(uGap) * 3 * ori_prev.Up) - (30.0f*glm::vec3(0,1,0)) - (0.4f*ori_prev.Up);
		glm::vec3 p2b = ori_cur.origin - (float(uGap) / 1.5f  * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(uGap) * 3 * ori_cur.Up) - (30.0f*glm::vec3(0, 1, 0)) - (0.4f*ori_cur.Up);
		glm::vec3 p3b = ori_cur.origin - (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) - (float(uGap) * 3 * ori_cur.Up) - (30.0f*glm::vec3(0, 1, 0)) - (0.4f*ori_cur.Up);
		glm::vec3 p4b = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) - (float(uGap) * 3 * ori_prev.Up) - (30.0f*glm::vec3(0, 1, 0)) - (0.4f*ori_prev.Up);

		make_triangle_plank(p1a, p4a, p1b, false);
		make_triangle_plank(p1b, p4b, p4a, true);

		make_triangle_plank(p4a, p3a, p4b, false);
		make_triangle_plank(p4b, p3b, p3a, true);

		make_triangle_plank(p2a, p1a, p2b, false);
		make_triangle_plank(p2b, p1b, p1a, true);

		make_triangle_plank(p2a, p3a, p2b, true);
		make_triangle_plank(p2b, p3b, p3a, false);

	}

	// Given two orintations, create the plank between them.  Offset can be useful if you want to call this for more than for multiple rails
	void makePlankPart(Orientation ori_prev, Orientation ori_cur, glm::vec2 offset, bool isSupport) {
		/*
		Design Strategy: 

				      6 + + + + + + + + + + + 7                                         F + + + + + + + + + + + G
				      +\                      +\                                        +\                      +\
				      + \                     + \                                       + \                     + \
		      Y-------+--\--------------------+--\--------------------------------------+--\--------------------+--\----U
		      |\      +\  \                   +   \                                     +\  \                   +   \   |\
		      | \     + \  5 + + + + + + + + +++ + 8                                    + \  E + + + + + + + + +++ + H  | \
		      |  \    +  \ +      c           +    +               O'                   +  \ +      c           +    +  |  \
		      |   \   +   \+                  +    +                                    +   \+                  +    +  |   \
		      |    T--+----+------------------+----+------------------------------------+----+------------------+----+--|----I
		      W----|--+----+------------------+----+------------------------------------+----+------------------+----+--E    |
		       \   |  +    +                  +    +                                    +    +                  +    +   \   |
		        \  |  +    +           p      +    +                    O               +    +           p      +    +    \  |
		         \ |  2 + +++ + + + + + + + + 3    +                                    B + +++ + + + + + + + + C    +     \ |
			      \|   \   +                   \   +                                     \   +                   \   +      \|
			       Q----\--+--------------------\--+--------------------------------------\--+--------------------\--+-------R
				         \ +                     \ +                                       \ +                     \ +
					      \+                      \+                                        \+                      \+
					       1 + + + + + + + + + + + 4                                         A + + + + + + + + + + + D

		*/
		
		//the wodden support below
		glm::vec3 p1b = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) - (float(uGap) * 3 * ori_prev.Up) - (0.4f*ori_prev.Up);
		glm::vec3 p2b = ori_cur.origin - (float(uGap) / 1.5f  * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(uGap) * 3 * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p3b = ori_cur.origin - (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) - (float(uGap) * 3 * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p4b = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) - (float(uGap) * 3 * ori_prev.Up) - (0.4f*ori_prev.Up);

		glm::vec3 p5b = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) - (float(uGap) * 3 * ori_prev.Up) - (0.4f*ori_prev.Up);
		glm::vec3 p6b = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(uGap) * 3 * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p7b = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) - (float(uGap) * 3 * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p8b = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) - (float(uGap) * 3 * ori_prev.Up) - (0.4f*ori_prev.Up);

		if (isSupport == false) {
			//plank at the bottom
			glm::vec3 pQ = ori_prev.origin - (float(uGap) / 4 * ori_prev.Up) - (float(uGap) * 2.0f * ori_prev.Right) - (float(railGap) / 1.7f * ori_prev.Right) - (0.4f*ori_prev.Up);
			glm::vec3 pW = ori_cur.origin - (float(uGap) / 4 * ori_cur.Up) - (float(uGap) * 2.0f * ori_cur.Right) - (float(railGap) / 1.7f * ori_cur.Right) - (0.4f*ori_cur.Up);
			glm::vec3 pE = ori_cur.origin - (float(uGap) / 4 * ori_cur.Up) + (float(uGap) * 2.0f * ori_cur.Right) + (float(railGap) / 1.7f * ori_cur.Right) - (0.4f*ori_cur.Up);
			glm::vec3 pR = ori_prev.origin - (float(uGap) / 4 * ori_prev.Up) + (float(uGap) * 2.0f * ori_prev.Right) + (float(railGap) / 1.7f * ori_prev.Right) - (0.4f*ori_prev.Up);

			glm::vec3 pT = ori_prev.origin + (float(uGap) / 4 * ori_prev.Up) - (float(uGap) * 2.0f * ori_prev.Right) - (float(railGap) / 1.7f * ori_prev.Right) - (0.4f*ori_prev.Up);
			glm::vec3 pY = ori_cur.origin + (float(uGap) / 4 * ori_cur.Up) - (float(uGap) * 2.0f * ori_cur.Right) - (float(railGap) / 1.7f * ori_cur.Right) - (0.4f*ori_cur.Up);
			glm::vec3 pU = ori_cur.origin + (float(uGap) / 4 * ori_cur.Up) + (float(uGap) * 2.0f * ori_cur.Right) + (float(railGap) / 1.7f * ori_cur.Right) - (0.4f*ori_cur.Up);
			glm::vec3 pI = ori_prev.origin + (float(uGap) / 4 * ori_prev.Up) + (float(uGap) * 2.0f * ori_prev.Right) + (float(railGap) / 1.7f * ori_prev.Right) - (0.4f*ori_prev.Up);

			//railing left bar
			glm::vec3 p5a = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) - (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) - (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);
			glm::vec3 p6a = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) - (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);

			glm::vec3 p5l = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) - (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) - (float(railGap) / 3.0f * ori_prev.Right) - (float(uGap) / 2.0f * ori_prev.Right) - (0.4f*ori_prev.Up);
			glm::vec3 p6l = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) - (float(railGap) / 3.0f * ori_cur.Right) - (float(uGap) / 2.0f * ori_cur.Right) - (0.4f*ori_cur.Up);

			glm::vec3 pQl = ori_prev.origin - (float(uGap) / 4 * ori_prev.Up) - (float(uGap) * 2.0f * ori_prev.Right) - (float(railGap) / 1.7f * ori_prev.Right) - (float(uGap) / 2.0f * ori_prev.Right) - (0.4f*ori_prev.Up);
			glm::vec3 pWl = ori_cur.origin - (float(uGap) / 4 * ori_cur.Up) - (float(uGap) * 2.0f * ori_cur.Right) - (float(railGap) / 1.7f * ori_cur.Right) - (float(uGap) / 2.0f * ori_cur.Right) - (0.4f*ori_cur.Up);

			//railing right bar
			glm::vec3 pGa = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) + (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) + (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
			glm::vec3 pHa = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) + (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) + (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);

			glm::vec3 pGr = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) + (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) + (float(railGap) / 3.0f * ori_cur.Right) + (float(uGap) / 2.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
			glm::vec3 pHr = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) + (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) + (float(railGap) / 3.0f * ori_prev.Right) + (float(uGap) / 2.0f * ori_prev.Right) - (0.4f*ori_prev.Up);

			glm::vec3 pEr = ori_cur.origin - (float(uGap) / 4 * ori_cur.Up) + (float(uGap) * 2.0f * ori_cur.Right) + (float(railGap) / 1.7f * ori_cur.Right) + (float(uGap) / 2.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
			glm::vec3 pRr = ori_prev.origin - (float(uGap) / 4 * ori_prev.Up) + (float(uGap) * 2.0f * ori_prev.Right) + (float(railGap) / 1.7f * ori_prev.Right) + (float(uGap) / 2.0f * ori_prev.Right) - (0.4f*ori_prev.Up);

			//slanted support extra points
			glm::vec3 pQr = ori_prev.origin - (float(uGap) / 4 * ori_prev.Up) - (float(uGap) * 2.0f * ori_prev.Right) - (float(railGap) / 1.5f * ori_prev.Right) + (float(uGap) * 2 * ori_prev.Right) - (0.4f*ori_prev.Up);
			glm::vec3 pRl = ori_prev.origin - (float(uGap) / 4 * ori_prev.Up) + (float(uGap) * 2.0f * ori_prev.Right) + (float(railGap) / 1.5f * ori_prev.Right) - (float(uGap) * 2 * ori_prev.Right) - (0.4f*ori_prev.Up);
			glm::vec3 pWr = ori_cur.origin - (float(uGap) / 4 * ori_cur.Up) - (float(uGap) * 2.0f * ori_cur.Right) - (float(railGap) / 1.5f * ori_cur.Right) + (float(uGap) * 2 * ori_cur.Right) - (0.4f*ori_cur.Up);
			glm::vec3 pEl = ori_cur.origin - (float(uGap) / 4 * ori_cur.Up) + (float(uGap) * 2.0f * ori_cur.Right) + (float(railGap) / 1.5f * ori_cur.Right) - (float(uGap) * 2 * ori_cur.Right) - (0.4f*ori_cur.Up);

			//bottom
			make_triangle_plank(pQ, pW, pR, true);
			make_triangle_plank(pR, pE, pW, false);
			//top
			make_triangle_plank(pT, pY, pI, false);
			make_triangle_plank(pI, pU, pY, true);
			//left
			make_triangle_plank(pQ, pW, pT, true);
			make_triangle_plank(pT, pY, pW, false);
			//right
			make_triangle_plank(pR, pE, pI, true);
			make_triangle_plank(pI, pU, pE, false);
			//front
			make_triangle_plank(pQ, pR, pT, true);
			make_triangle_plank(pT, pI, pR, false);
			//back
			make_triangle_plank(pW, pE, pY, false);
			make_triangle_plank(pY, pU, pE, true);

			//left side railing bars

			make_triangle_plank(pQ, pW, p5a, true);
			make_triangle_plank(p5a, p6a, pW, false);

			make_triangle_plank(p5l, p6l, p5a, false);
			make_triangle_plank(p5a, p6a, p6l, true);

			make_triangle_plank(pQl, pWl, pQ, true);
			make_triangle_plank(pQ, pW, pWl, false);

			make_triangle_plank(pWl, pQl, p6l, true);
			make_triangle_plank(p6l, p5l, pQl, false);

			make_triangle_plank(pQl, pQ, p5l, true);
			make_triangle_plank(p5l, p5a, pQ, false);

			make_triangle_plank(pWl, pW, p6l, false);
			make_triangle_plank(p6l, p6a, pW, true);

			//right side railing bars
			make_triangle_plank(pR, pE, pHa, false);
			make_triangle_plank(pHa, pGa, pE, true);

			make_triangle_plank(pHa, pHr, pGa, true);
			make_triangle_plank(pGa, pGr, pHr, false);

			make_triangle_plank(pRr, pEr, pHr, true);
			make_triangle_plank(pHr, pGr, pEr, false);

			make_triangle_plank(pR, pE, pRr, true);
			make_triangle_plank(pRr, pEr, pE, false);

			make_triangle_plank(pRr, pR, pHr, false);
			make_triangle_plank(pHr, pHa, pR, true);

			make_triangle_plank(pEr, pE, pGr, true);
			make_triangle_plank(pGr, pGa, pE, false);

			//slanted support
			//left bottom
			make_triangle_plank(p2b, p1b, pWl, true);
			make_triangle_plank(pWl, pQl, p1b, false);
			//right bottom
			make_triangle_plank(p4b, p3b, pRr, true);
			make_triangle_plank(pRr, pEr, p3b, false);
			//left top
			make_triangle_plank(p6b, p5b, pWr, false);
			make_triangle_plank(pWr, pQr, p5b, true);
			//right top
			make_triangle_plank(p8b, p7b, pRl, false);
			make_triangle_plank(pRl, pEl, p7b, true);
			//left front
			make_triangle_plank(p5b, p1b, pQr, false);
			make_triangle_plank(pQr, pQl, p1b, true);
			//right front
			make_triangle_plank(p4b, p8b, pRr, false);
			make_triangle_plank(pRr, pRl, p8b, true);
			//left back
			make_triangle_plank(p6b, p2b, pWr, true);
			make_triangle_plank(pWr, pWl, p2b, false);
			//right back
			make_triangle_plank(p7b, p3b, pEl, false);
			make_triangle_plank(pEl, pEr, p3b, true);
		}
		else
		{

			//bottom 
			make_triangle_plank(p1b, p2b, p4b, true);
			make_triangle_plank(p4b, p3b, p2b, false);
			//top
			make_triangle_plank(p5b, p6b, p8b, false);
			make_triangle_plank(p8b, p7b, p6b, true);
			//left
			make_triangle_plank(p1b, p2b, p5b, false);
			make_triangle_plank(p5b, p6b, p2b, true);
			//right
			make_triangle_plank(p3b, p4b, p8b, false);
			make_triangle_plank(p8b, p7b, p3b, false);
		}
		
	}

	// Given two orintations, create the rail between them.  Offset can be useful if you want to call this for more than for multiple rails
	void makeRailPart(Orientation ori_prev, Orientation ori_cur, glm::vec2 offset)
	{
		/*
		Design Strategy: 
		6 + + + + + + + + + + + 7                                                                        F + + + + + + + + + + + G
		+\          |           +\                                                                       +\          |           +\
		+ \         |           + \                                                                      + \         |           + \
		+  \        |           +  \                                                                     +  \        |           +  \
		+   \       |           +   \                                                                    +   \       |           +   \
		+    5 + + +|+ + + + + +++ + 8                                                                   +    E + + +|+ + + + + +++ + H
		+    +      c    |      +    +                                                                   +    +      c    |      +    +
		+    +      |    |      +    +                                                                   +    +      |    |      +    +
		+    +      |    |      +    +                                                                   +    +      |    |      +    +
		+    +      |    |      +    +                                                                   +    +      |    |      +    +
		+    +      |    |      +    +                                                                   +    +      |    |      +    +
		+    +      |    p      +    +                                                                   +    +      |    p      +    +
		2 + +++ + + + + +|+ + + 3    +                                                                   B + +++ + + + + +|+ + + C    +
		 \   +           |       \   +                                                                    \   +           |       \   +
		  \  +           |        \  +                                                                     \  +           |        \  +
		   \ +           |         \ +                                                                      \ +           |         \ +
			\+           |          \+                                                                       \+           |          \+
			 1 + + + + + + + + + + + 4                                                                        A + + + + + + + + + + + D
		                 6 + + + + + + + + + + + 7                                         F + + + + + + + + + + + G
		                 +\          |           +\                                        +\          |           +\
		                 + \         |           + \                                       + \         |           + \
		                 +  \        |           +  \                                      +  \        |           +  \
		                 +   \       |           +   \                                     +   \       |           +   \
		                 +    5 + + +|+ + + + + +++ + 8                                    +    E + + +|+ + + + + +++ + H
		                 +    +      c    |      +    +               O'                   +    +      c    |      +    +
		                 +    +      |    |      +    +                __ /|\              +    +      |    |      +    +
		                 +    +      |    |      +    +                |\  |               +    +      |    |      +    +
		                 +    +      |    |      +    +                  \ |               +    +      |    |      +    +
		                 +    +      |    |      +    +                   \|_______\       +    +      |    |      +    +
		                 +    +      |    p      +    +                    O       /       +    +      |    p      +    +
		                 2 + +++ + + + + +|+ + + 3    +                                    B + +++ + + + + +|+ + + C    +
		                  \   +           |       \   +                                     \   +           |       \   +
		                   \  +           |        \  +                                      \  +           |        \  +
		                    \ +           |         \ +                                       \ +           |         \ +
			                 \+           |          \+                                        \+           |          \+
			                  1 + + + + + + + + + + + 4                                         A + + + + + + + + + + + D

		*/
		glm::vec3 p1 = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2  * ori_prev.Right) - (float(railGap)/2  * ori_prev.Right) + (float(uGap) / 1.15f * ori_prev.Up) - (0.4f*ori_prev.Up);
		glm::vec3 p2 = ori_cur.origin - (float(uGap) / 1.5f  * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(railGap)/2 * ori_cur.Right) + (float(uGap) / 1.15f * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p3 = ori_cur.origin - (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) - (float(railGap)/2 * ori_cur.Right) + (float(uGap) / 1.15f * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p4 = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2  * ori_prev.Right) - (float(railGap)/2  * ori_prev.Right) + (float(uGap) / 1.15f * ori_prev.Up) - (0.4f*ori_prev.Up);

		glm::vec3 p5 = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) - (float(railGap) / 2 * ori_prev.Right) + (float(uGap) / 1.15f * ori_prev.Up) - (0.4f*ori_prev.Up);
		glm::vec3 p6 = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(railGap) / 2 * ori_cur.Right) + (float(uGap) / 1.15f * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p7 = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) - (float(railGap) / 2 * ori_cur.Right) + (float(uGap) / 1.15f * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 p8 = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) - (float(railGap) / 2 * ori_prev.Right) + (float(uGap) / 1.15f * ori_prev.Up) - (0.4f*ori_prev.Up);

		glm::vec3 pA = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap)/2  * ori_prev.Right) + (float(railGap)/2  * ori_prev.Right) + (float(uGap) / 1.15f * ori_prev.Up) - (0.4f*ori_prev.Up);
		glm::vec3 pB = ori_cur.origin - (float(uGap) / 1.5f  * ori_cur.Up) - (float(uGap)/2 * ori_cur.Right) + (float(railGap)/2 * ori_cur.Right) + (float(uGap) / 1.15f * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 pC = ori_cur.origin - (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap)/2 * ori_cur.Right) + (float(railGap)/2 * ori_cur.Right) + (float(uGap) / 1.15f * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 pD = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap)/2  * ori_prev.Right) + (float(railGap)/2  * ori_prev.Right) + (float(uGap) / 1.15f * ori_prev.Up) - (0.4f*ori_prev.Up);

		glm::vec3 pE = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) + (float(railGap) / 2 * ori_prev.Right) + (float(uGap) / 1.15f * ori_prev.Up) - (0.4f*ori_prev.Up);
		glm::vec3 pF = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) + (float(railGap) / 2 * ori_cur.Right) + (float(uGap) / 1.15f * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 pG = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) + (float(railGap) / 2 * ori_cur.Right) + (float(uGap) / 1.15f * ori_cur.Up) - (0.4f*ori_cur.Up);
		glm::vec3 pH = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) + (float(railGap) / 2 * ori_prev.Right) + (float(uGap) / 1.15f * ori_prev.Up) - (0.4f*ori_prev.Up);
		
		//rails that are above: 

		glm::vec3 p1a = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) - (float(railGap) / 2 * ori_prev.Right)+ (float(uGap) * 4.0f * ori_prev.Up) - (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);
		glm::vec3 p2a = ori_cur.origin - (float(uGap) / 1.5f  * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) - (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
		glm::vec3 p3a = ori_cur.origin - (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) - (float(railGap) / 2 * ori_cur.Right) + (float(uGap)* 4.0f * ori_cur.Up) - (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
		glm::vec3 p4a = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) - (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) - (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);

		glm::vec3 p5a = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) - (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) - (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);
		glm::vec3 p6a = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) - (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) - (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
		glm::vec3 p7a = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) - (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) - (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
		glm::vec3 p8a = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) - (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) - (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);

		glm::vec3 pAa = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) + (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) + (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);
		glm::vec3 pBa = ori_cur.origin - (float(uGap) / 1.5f  * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) + (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) + (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
		glm::vec3 pCa = ori_cur.origin - (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) + (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) + (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
		glm::vec3 pDa = ori_prev.origin - (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) + (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) + (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);

		glm::vec3 pEa = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) - (float(uGap) / 2 * ori_prev.Right) + (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) + (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);
		glm::vec3 pFa = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) - (float(uGap) / 2 * ori_cur.Right) + (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) + (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
		glm::vec3 pGa = ori_cur.origin + (float(uGap) / 1.5f * ori_cur.Up) + (float(uGap) / 2 * ori_cur.Right) + (float(railGap) / 2 * ori_cur.Right) + (float(uGap) * 4.0f * ori_cur.Up) + (float(railGap) / 3.0f * ori_cur.Right) - (0.4f*ori_cur.Up);
		glm::vec3 pHa = ori_prev.origin + (float(uGap) / 1.5f * ori_prev.Up) + (float(uGap) / 2 * ori_prev.Right) + (float(railGap) / 2 * ori_prev.Right) + (float(uGap) * 4.0f * ori_prev.Up) + (float(railGap) / 3.0f * ori_prev.Right) - (0.4f*ori_prev.Up);

		
		//----------LEFT RAIL-----------

		//bottom
		make_triangle(p1, p2, p4, true);
		make_triangle(p4, p3, p2, false);
		//top
		make_triangle(p5, p6, p8, false);
		make_triangle(p8, p7, p6, true);
		//left
		make_triangle(p1, p2, p5, false);
		make_triangle(p5, p6, p2, true);
		//right
		make_triangle(p3, p4, p8, false);
		make_triangle(p8, p7, p3, false);

		//----------RIGHT RAIL-----------

		//bottom
		make_triangle(pA, pB, pD, true);
		make_triangle(pD, pC, pB, false);
		//top
		make_triangle(pE, pF, pH, false);
		make_triangle(pH, pG, pF, true);
		//left
		make_triangle(pA, pB, pE, false);
		make_triangle(pE, pF, pB, true);
		//right
		make_triangle(pC, pD, pH, false);
		make_triangle(pH, pG, pC, false);

		//----------LEFT RAIL-----------

		//bottom
		make_triangle(p1a, p2a, p4a, true);
		make_triangle(p4a, p3a, p2a, false);
		//top
		make_triangle(p5a, p6a, p8a, false);
		make_triangle(p8a, p7a, p6a, true);
		//left
		make_triangle(p1a, p2a, p5a, false);
		make_triangle(p5a, p6a, p2a, true);
		//right
		make_triangle(p3a, p4a, p8a, false);
		make_triangle(p8a, p7a, p3a, false);

		//----------RIGHT RAIL-----------

		//bottom
		make_triangle(pAa, pBa, pDa, true);
		make_triangle(pDa, pCa, pBa, false);
		//top
		make_triangle(pEa, pFa, pHa, false);
		make_triangle(pHa, pGa, pFa, true);
		//left
		make_triangle(pAa, pBa, pEa, false);
		make_triangle(pEa, pFa, pBa, true);
		//right
		make_triangle(pCa, pDa, pHa, false);
		make_triangle(pHa, pGa, pCa, false);

		
	}

	// Find the normal for each triangle uisng the cross product and then add it to all three vertices of the triangle.  
	//   The normalization of all the triangles happens in the shader which averages all norms of adjacent triangles.   
	//   Order of the triangles matters here since you want to normal facing out of the object.  
	void set_normals(Vertex &p1, Vertex &p2, Vertex &p3)
	{
		glm::vec3 normal = glm::cross(p2.Position - p1.Position, p3.Position - p1.Position);
		p1.Normal += normal;
		p2.Normal += normal;
		p3.Normal += normal;
	}

	void setup_track()
	{
		// Like the heightmap project, this will create the buffers and send the information to OpenGL
		 //  1.  Create ID / Generate Buffers and for Vertex Buffer Object (VBO),
		 //      Vertex Array Buffer (VAO), and the Element Buffer Objects (EBO)

		 // 2. Bind Vertex Array Object
		 glGenVertexArrays(1, &VAO);

		 //  Bind the Vertex Buffer
		 glGenBuffers(1, &VBO);
		 //glGenBuffers(1, &EBO);

		 glBindVertexArray(VAO);

		 // 3. Copy our vertices array in a vertex buffer for OpenGL to use
		 glBindBuffer(GL_ARRAY_BUFFER, VBO);
		 glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		 // 4. Copy our indices array in a vertex buffer for OpenGL to use
		 //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		 //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

		 // 5.  Position attribute for the 3D Position Coordinates and link to position 0
		 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		 glEnableVertexAttribArray(0);
		 // 6.  TexCoord attribute for the 2d Texture Coordinates and link to position 2
		 glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		 glEnableVertexAttribArray(1);
		 // 6.  TexCoord attribute for the 2d Texture Coordinates and link to position 2
		 glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
		 glEnableVertexAttribArray(2);
	}
	void setup_track_plank()
	{
		// Like the heightmap project, this will create the buffers and send the information to OpenGL
		 //  1.  Create ID / Generate Buffers and for Vertex Buffer Object (VBO),
		 //      Vertex Array Buffer (VAO), and the Element Buffer Objects (EBO)

		 // 2. Bind Vertex Array Object
		glGenVertexArrays(1, &VAOPlank);

		//  Bind the Vertex Buffer
		glGenBuffers(1, &VBOplank);
		//glGenBuffers(1, &EBO);

		glBindVertexArray(VAOPlank);

		// 3. Copy our vertices array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBOplank);
		glBufferData(GL_ARRAY_BUFFER, vertices_plank.size() * sizeof(Vertex), &vertices_plank[0], GL_STATIC_DRAW);

		// 4. Copy our indices array in a vertex buffer for OpenGL to use
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices[0], GL_STATIC_DRAW);

		// 5.  Position attribute for the 3D Position Coordinates and link to position 0
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		// 6.  TexCoord attribute for the 2d Texture Coordinates and link to position 2
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		// 6.  TexCoord attribute for the 2d Texture Coordinates and link to position 2
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}
};
