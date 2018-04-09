// Author: Amay Kataria
// Date: 02/23/2018

#pragma once

#include "ofMain.h"
#include "ofxBox2d.h"
#include "Osc.h"
#include "ofxFaceTracker2.h"
#include "Face.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    void keyPressed(int key);
  
    // Helpers
    void setupMeshPlane();
    void setupBox2dSprings();
    void updateMeshPlane();
    void trackMeshCentroidForWekinator(); 
  
    // Constants
    const int meshRadius = 100;
    const int meshPoints = 20;
    const int circleRadius = 18;
    const int textureLength = 400;
    const int jointLength = 5;
  
    // Mesh and texture.
    ofImage image;
    ofMesh mesh;
  
    // Commands. 
    bool showMesh;
    bool showSoftBody;
    bool updateMesh;
    bool showTexture;
  
    // Box2D
    ofxBox2d box2d;   // the box2d world
    std::vector<std::shared_ptr<ofxBox2dCircle>> circles; // default box2d circles
    std::vector<std::shared_ptr<ofxBox2dJoint>> joints;  // joints
  
    // OSC
    Osc oscHandler;
  
    // Facetracker
    ofxFaceTracker2 tracker;
    ofVideoGrabber grabber;
    std::vector<ofRectangle> boundingBoxes;
    std::vector<ofMesh> faceMeshes;
    ofMesh faceMesh; 

    void showBoundingRect();
    void createFaceMesh();
    void createRectFaceMesh();
    void updateFaceMeshPlane();
    void createFaceBox2DSprings();
  
    bool showGrabber = false;
    bool showDebugFace = false;
    bool createMeshAndSprings = true;
    int faceMeshRadius = -1;
  
    // Face properties
    std::vector<Face> faces; 
};
