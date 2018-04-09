// Author: Amay Kataria
// Date: 02/23/2018

#include "ofMain.h"
#include "ofxBox2d.h"

#pragma once

class Face {
	private:
    std::vector<std::shared_ptr<ofxBox2dCircle>> circles; // default box2d circles
    std::vector<std::shared_ptr<ofxBox2dJoint>> joints;  // joints
    ofMesh faceMesh;
  
    // Constants
    const int meshPoints = 20;
    const int circleRadius = 18;
    const int jointLength = 5;
    float faceMeshRadius;
  
  public:
    void createFaceMesh(ofRectangle boundingRect);
    void updateFaceMeshPlane();
    void createFaceBox2DSprings(ofxBox2d &box2d);
    void draw(ofVideoGrabber &grabber, bool showMesh, bool showTexture, bool showSoftBody);
};

// We will need the grabber texture to be associated with every face.
// Somehow pass grabber texture to every face object.
