// Author: Amay Kataria
// Date: 02/23/2018

#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
  
    // Helpers
    void setupMeshPlane();
    void setupBox2dSprings();
  
    // Constants
    const int radius = 150;
  
    // Mesh and texture.
    ofImage texture;
    ofMesh mesh;
    ofPath path;
  
  
    // Box2D
    ofxBox2d box2d;   // the box2d world
    std::vector<std::shared_ptr<ofxBox2dCircle>> circles; // default box2d circles
    std::vector<std::shared_ptr<ofxBox2dJoint>> joints;  // joints
};
