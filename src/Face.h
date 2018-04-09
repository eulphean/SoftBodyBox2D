// Author: Amay Kataria
// Date: 02/23/2018

#include "ofMain.h"

#pragma once

class Face {
	private:
    std::vector<std::shared_ptr<ofxBox2dCircle>> circles; // default box2d circles
    std::vector<std::shared_ptr<ofxBox2dJoint>> joints;  // joints
    ofMesh faceMesh;
  
  public:
    void createFaceMesh();
    void updateFaceMeshPlane(ofRectangle boundingRect);
    void createFaceBox2DSprings();
    void showMesh();
    void showMeshWireframe();
    void showSoftBody();
    void showTexture();
};

// We will need the grabber texture to be associated with every face.
// Somehow pass grabber texture to every face object.
