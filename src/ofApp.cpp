// Author: Amay Kataria
// Date: 02/23/2018

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0);
  
    // Setup Box2d
    box2d.init();
    box2d.setGravity(-0.3f, 0.3f);
    box2d.createBounds(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
    box2d.setFPS(60.0);
    box2d.registerGrabbing(); // Enable grabbing the circles.
  
    // Commands
    showMesh = true;
    showSoftBody = false;
    showTexture = false;
  
    // OSC handler
    oscHandler.setup();
  
    // Setup GUI.
    gui.setup();
    gui.add(softCircleRadius.setup("Radius", 18, 2, 50));
    gui.add(softJointLength.setup("JointLength", 2, 0, 20));
    gui.add(density.setup("Density", 0.5, 0, 1));
    gui.add(bounce.setup("Bounce", 0.5, 0, 1));
    gui.add(friction.setup("Friction", 0.5, 0, 1));
    gui.add(centerJointFrequency.setup("CenterJointFrequency", 4.f, 0.f, 20.f ));
    gui.add(centerJointDamping.setup("CenterJointDamping", 1.f, 0.f, 5.f));
    gui.add(outerJointFrequency.setup("OuterJointFrequency", 1.f, 0.f, 20.f));
    gui.add(outerJointDamping.setup("OuterJointDamping", 1.f, 0.f, 5.f));
  
    gui.loadFromFile("SoftBodyBox2D.xml");
    
    // Face tracker
    grabber.setup(ofGetWidth(), ofGetHeight());
    tracker.setup();
}

//--------------------------------------------------------------
void ofApp::update(){
  // Box2D updates.
  box2d.update();
  
  // Update grabber and face tracker.
  grabber.update();
  
  if (grabber.isFrameNew()) {
    // Clear the vector.
    boundingBoxes.clear();
    tracker.update(grabber);

    // Get the bounding boxes.
    if (tracker.size() > 0){
      auto& instances = tracker.getInstances();
      for (auto &i : instances) {
        auto rect = i.getBoundingBox();
        boundingBoxes.push_back(rect);
      }
    }
    
    // Go through all the bounding boxes currently active and
    // create a new face. Turn off the flag to enable this creation
    // for now. We can enable that flag for the system to recreate
    // these faces by emptying the current set of faces.
    if (boundingBoxes.size() > 0) {
      if (createMeshAndSprings) {
        // Vector of density, bounce, and friction.
        ofPoint physics(density, bounce, friction);
        ofPoint centerJointPhysics(centerJointFrequency, centerJointDamping);
        ofPoint outerJointPhysics(outerJointFrequency, outerJointDamping);
        
        // Clear existing faces.
        faces.clear();
        for (auto &r : boundingBoxes) {
          Face face;
          face.createFaceMesh(r, softCircleRadius, softJointLength); // Create a face mesh.
          face.createFaceBox2DSprings(box2d, softCircleRadius, softJointLength, physics, centerJointPhysics, outerJointPhysics); // Create the soft body. Pass all GUI physics parameters.
          faces.push_back(face);
        }
        createMeshAndSprings = false;
      }
    }
    
    // Go through each face and update it based on its movement in the
    // environment.
    for (auto &face : faces) {
      if (boundingBoxes.size() > 0) {
        for (auto &r : boundingBoxes) {
          // Get the texture and update it. 
        }
      }
      face.updateFaceMeshPlane();
    }
  }
  
  // Update OSC Handler.
  //oscHandler.update();
  
  // Box2D updates.
  // box2d.update();
  
  // Update mesh plane.
  //updateMeshPlane();
  
  // Track centroid with Wekinator. 
  //trackMeshCentroidForWekinator();
}

//--------------------------------------------------------------
void ofApp::draw(){
  //showBoundingRect();
  
  if (showDebugFace) {
    tracker.drawDebug();
  }
  
  for (auto &f : faces) {
    f.draw(grabber, showMesh, showTexture, showSoftBody);
  }
  
  // Grabber.
  if (showGrabber) {
    grabber.draw(0, 0);
  }
  
  gui.draw(); // Draw the GUI.
}

void ofApp::showBoundingRect() {
  // Draw the bounding boxes from the instances only.
  for (auto& b : boundingBoxes) {
    ofPushStyle();
      ofFill();
      ofSetColor(ofColor::red);
      ofDrawRectangle(b.getX(), b.getY(), b.getWidth(), b.getHeight());
    ofPopStyle();
  }
}

void ofApp::keyPressed(int key) {
  switch (key) {
    case 'm': {
      showMesh = !showMesh;
      break;
    }
    
    case 's': {
      showSoftBody = !showSoftBody;
      break;
    }
    
    case 'r': {
      createMeshAndSprings = !createMeshAndSprings;
      break;
    }
  
    case 't': {
      showTexture = !showTexture;
      break;
    }
    
    case 'g': {
      showGrabber = !showGrabber;
      break;
    }
    
    case 'f': {
      showDebugFace = !showDebugFace;
      break;
    }
    
    default:
      break;
  }
}

void ofApp::trackMeshCentroidForWekinator() {
  //glm::vec2 centroid = mesh.getCentroid();
  //oscHandler.sendPositionToWekinator(centroid);
}

void ofApp::exit() {
  gui.saveToFile("SoftBodyBox2D.xml");
}

//void ofApp::setupMeshPlane() {
//    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
//
//    // Center vertex for the triangle fan.
//    glm::vec3 center = glm::vec3(ofGetWidth()/2, ofGetHeight()/2, 0);
//    mesh.addVertex(center);
//    // Center vertex.
//    mesh.addTexCoord(glm::vec2(textureLength/2, textureLength/2));
//
//    // Ratio between the texture and the circular mesh.
//    float sizeRatio = textureLength / (meshRadius * 2);
//
//    // Add vertices around the center to form a circle.
//    for(int i = 1; i < meshPoints; i++){
//      float n = ofMap(i, 1, meshPoints - 1, 0.0, TWO_PI);
//      float x = sin(n);
//      float y = cos(n);
//
//      mesh.addVertex({center.x + (x * meshRadius), center.y + y * meshRadius, 0});
//      mesh.addTexCoord(glm::vec2(textureLength/2 + x * meshRadius * sizeRatio, textureLength/2 + y * meshRadius * sizeRatio));
//    }
//}
//
//void ofApp::createRectFaceMesh() {
//  // Get the first bounding box, which is my face for now.
//  auto &r = boundingBoxes[0];
//
//  // Clear the mesh.
//  faceMesh.clear();
//
//  faceMesh.setMode(OF_PRIMITIVE_TRIANGLES);
//
//  int nCols = 5;
//  int nRows = 5;
//  float faceX = r.getX();
//  float faceY = r.getY();
//  float faceWidth = r.getWidth();
//  float xOffset = faceWidth/nRows;
//  float faceHeight = r.getHeight();
//  float yOffset = faceHeight/nCols;
//  float faceArea = r.getArea();
//  glm::vec3 faceCenter = r.getCenter();
//
//  // Setup a 5 x 5 mesh.
//  for (int y = 0; y < nCols; y++) {
//    for (int x = 0; x < nRows; x++) {
//      float ix = faceWidth * x / (nCols - 1);
//      float iy = faceHeight * y / (nRows - 1);
//      faceMesh.addVertex({ix, iy, 0});
//      // Texture coordinates will be of the texture bounded by
//      // rectangle.. Offset the texCoordinate by faceX and faceY.
//      faceMesh.addTexCoord(glm::vec2(ix + faceX, iy + faceY));
//    }
//  }
//
//  // We don't draw the last row / col (nRows - 1 and nCols - 1) because it was
//  // taken care of by the row above and column to the left.
//  for (int y = 0; y < nRows - 1; y++)
//  {
//      for (int x = 0; x < nCols - 1; x++)
//      {
//          // Draw T0
//          // P0
//          faceMesh.addIndex((y + 0) * nCols + (x + 0));
//          // P1
//          faceMesh.addIndex((y + 0) * nCols + (x + 1));
//          // P2
//          faceMesh.addIndex((y + 1) * nCols + (x + 0));
//
//          // Draw T1
//          // P1
//          faceMesh.addIndex((y + 0) * nCols + (x + 1));
//          // P3
//          faceMesh.addIndex((y + 1) * nCols + (x + 1));
//          // P2
//          faceMesh.addIndex((y + 1) * nCols + (x + 0));
//      }
//  }
//}
