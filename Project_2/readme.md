# Project Title

STAR WARS WORLD (ROLLER COASTER SIMULATION)

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 

### Prerequisites

* Visual Studio 2015, Visual Studio 2017 other VS distros
* Windows, Ubuntu 14.04, Mac OSX Sierra(should work on linux, apple)
* You should know OpenGL
* cMake
* Easy to use

### Running the Project

Click the Project2.exe to run the project

### Instructions

* Press the U,I,O to increase Darth Vader's powers
* Press the J,K,L to decrease Darth Vader's powers
* Key alone will alter rotation rate
* Shift+Key will alter scale
* Control+Key will alter Darth Vader's powers
* Pressing G will reset Darth Vader's powers
* Use mouse to look around
* Press W,A,S,D to move around 
* Press N to look at the normal lines of each object
* Press B to make the boxes and the track to reflect the skybox
* Press C and T roughly at the same time to keep the cart and the track running
* Press C again to remove the spaceship while moving on the track

## Built With

* OpenGL
* Visual Studio 2017
* cMake
* Windows 10 Pro

## Project Structure

These will give you an idea what parts I have done and the basic file structure for this project

### File Structures

The files are located in ‘path/to/files/Project_2/
* Project2.hpp source file:  ‘path/to/files/Project_2/Sources/
* heightmap.hpp header file:  ‘path/to/files/Project_2/Headers/
* shader.vert and shader.frag shader files:  ‘path/to/files/Project_2/Shaders/
* Image files:  ‘path/to/files/CMPSC458Projects/Project_2/Media/
* Spline files: ‘path/to/files/CMPSC458Projects/Project_2/Media/spline_parts
* Track files: 

### Parts of Projects

* Part 1: Roller Coaster Track: Track is complete
* Part 2: Camera motion: Camera motion is complete with gravity
* Bonus :
 
	Story: 
This is a star wars world. Welcome to the world of Darth Vader. There is a giant Darth Vader that is using the dark force to make the boxes float. You can control Darth Vader's powers to move the boxes around. 

	Track: 
The track is complex containing two 3D rails with two 3D railings and a support beam. Planks extend to the railings and there is a slanted support from the end of the planks to the support beam. This was time consuming because I had to figure out the points for vertex of each of the things listed above. The most challenging part was to make the slanted support parrallel. The track has two different textures drawn on it. 
	
	Pillars: 
I have made pillars at every control points. This was tricky because many pillars were going through the track. I did fix that issue. Now the track is seamless. 

	Scenery: 
There is a giant Darth Vader's fort in the background. The skybox is space. This is a planet a Darth Vader conquered. 

	Cart Movement: 
There is a spaceship that goes around the track. Just one spaceship. This was tough because of the rotation of the cart. I had to multiply the model matrix of the spaceship with the right, up, and front 4x4 matrix inorder to rotate in the correct manner. (using basis vectors learned in class)
	
### List of Everything in the Submission

* Shaders
* Media
* Sources
* Headers
* Project2.exe
* Project2_Final_Chandra_Prateek.mp4
* readme.md

## Author(s)

Prateek Chandra

## Acknowledgments

* I thank Christopher Funk and the other LAs for the OpenGL sessions and basic setup
* I thank Professor Yanxi Liu for her lectures every Tuesday and Thursday

## External Resources

* I got Darth Vader's fort from:
https://free3d.com/3d-model/organodron-city-22304.html
* I got Darth Vader from:
https://clara.io/view/09ba3bc4-d618-4f6d-b521-c9fa91873492
* I got the spaceship from: 
https://free3d.com/3d-model/falcon-t45-rescue-ship-23161.html

