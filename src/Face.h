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
    int meshPoints; 
    const int circleRadius = 18; // These should be GUI sliders to adjust the shape.
    const int jointLength = 5; // There should be a single shape. We add more control points
    float faceMeshRadius; // as one comes in the shape. The size increases. We apply a GAN on that shape.
    float faceCircumference;
  
  public:
    void createFaceMesh(ofRectangle boundingRect, float circleRadius, float jointLength);
    void createFaceBox2DSprings(ofxBox2d &box2d, float circleRadius, float jointLength, ofPoint circlePhysics, ofPoint centerJointPhysics, ofPoint outerJointPhysics);
    void updateFaceMeshVertices();
    void updateFaceMeshTexture();
    void updateFaceMeshPlane();
    void draw(ofVideoGrabber &grabber, bool showMesh, bool showTexture, bool showSoftBody);
};

// We will need the grabber texture to be associated with every face.
// Somehow pass grabber texture to every face object.
