// Author: Amay Kataria
// Date: 02/23/2018

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableDepthTest();
    ofBackground(0);
  
    // Setup Box2d
    box2d.init();
    box2d.setGravity(0, 10);
    box2d.createBounds(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
    box2d.setFPS(60.0);
    box2d.registerGrabbing(); // Enable grabbing the circles.
  
    setupMeshPlane();
    setupBox2dSprings();
}

//--------------------------------------------------------------
void ofApp::update(){
  box2d.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
  for(auto c: circles) {
    ofNoFill();
    ofSetColor(ofColor::red);
    c->draw();
  }
  
  for(auto j: joints) {
    ofSetColor(ofColor::green);
    j->draw();
  }
}

void ofApp::setupMeshPlane() {
    int nPts = 10;
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    // Center vertex for the triangle fan.
    glm::vec3 center = glm::vec3(ofGetWidth()/2, ofGetHeight()/2, 0);
    mesh.addVertex(center);
  
    // Add vertices around the center to form a circle.
    for(int i = 0; i < nPts; i++){
        float n = ofMap(i, 0, nPts - 1, 0.0, TWO_PI);
        float x = sin(n);
        float y = cos(n);
        mesh.addVertex({center.x + (x * radius), center.y + y * radius, 0});
    }
}

void ofApp::setupBox2dSprings() {
  auto vertices = mesh.getVertices();
  
  // Construct circles at all the vertices from the mesh.
  for (auto v : vertices) {
    auto circle = std::make_shared<ofxBox2dCircle>();
    circle -> setPhysics(1.0, 0.2, 0);
    circle -> setup(box2d.getWorld(), v.x, v.y, 40);
    circles.push_back(circle);
  }
  
  // Connect center circle to all the circles.
  // We start from the 1st circle because the 0th circle is actually
  // the center circle to which all the other circles will get attached to.
  for(auto i=1; i<circles.size(); i++) {
    auto joint = std::make_shared<ofxBox2dJoint>();
    joint -> setup(box2d.getWorld(), circles[0] -> body, circles[i] -> body);
    joint->setLength(radius);
    joints.push_back(joint);
  }
  
  // Connect joints with each other.
  for(auto i=1; i<circles.size(); i++) {
    auto joint = std::make_shared<ofxBox2dJoint>();
    
    // At last index, make a spring back to 0.
    int fromIdx = i; int toIdx = i+1;
    if (i == circles.size() - 1) {
      toIdx = 1;
    }
    
    joint -> setup(box2d.getWorld(), circles[fromIdx] -> body, circles[toIdx] -> body);
    joint->setLength(30);
    joints.push_back(joint);
  }
}
