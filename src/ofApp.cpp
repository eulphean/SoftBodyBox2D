// Author: Amay Kataria
// Date: 02/23/2018

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableDepthTest();
    ofBackground(0);
  
    // Setup Box2d
    box2d.init();
    box2d.setGravity(0, 0);
    box2d.createBounds(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
    box2d.setFPS(60.0);
    box2d.registerGrabbing(); // Enable grabbing the circles.
  
    // This reduces the texture dimensions to (0, 0) to (1, 1).
    // Very helpful here. 
    //ofDisableArbTex();
  
    // Load image and resize.
    ofLoadImage(image, "nebula.png");
    image.resize(textureLength, textureLength);
    image.getTexture().setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
    image.getTexture().setTextureWrap(GL_REPEAT, GL_REPEAT);
  
    // Setup mesh and box2d springs and circles.
    setupMeshPlane();
    setupBox2dSprings();
  
    // Commands
    showMesh = true;
    showSoftBody = false;
    showTexture = false;
}

//--------------------------------------------------------------
void ofApp::update(){
  box2d.update();
  updateMeshPlane();
}

//--------------------------------------------------------------
void ofApp::draw(){
  if (showSoftBody) {
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
  
  if (showMesh) {
    ofSetColor(ofColor::white);
    // Check if we need to bind the texture.
    if (showTexture) {
      image.getTexture().bind();
      mesh.draw();
      image.getTexture().unbind();
    } else {
      mesh.drawWireframe();
    }
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
  
    case 't': {
      showTexture = !showTexture;
      break;
    }
    
    default:
      break;
  }
}

void ofApp::setupMeshPlane() {
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

    // Center vertex for the triangle fan.
    glm::vec3 center = glm::vec3(ofGetWidth()/2, ofGetHeight()/2, 0);
    mesh.addVertex(center);
    // Center vertex.
    mesh.addTexCoord(glm::vec2(textureLength/2, textureLength/2));
  
    // Ratio between the texture and the circular mesh.
    float sizeRatio = textureLength / (meshRadius * 2);
  
    // Add vertices around the center to form a circle.
    for(int i = 1; i < meshPoints; i++){
      float n = ofMap(i, 1, meshPoints - 1, 0.0, TWO_PI);
      float x = sin(n);
      float y = cos(n);
      
      mesh.addVertex({center.x + (x * meshRadius), center.y + y * meshRadius, 0});
      mesh.addTexCoord(glm::vec2(textureLength/2 + x * meshRadius * sizeRatio, textureLength/2 + y * meshRadius * sizeRatio));
    }
}

void ofApp::updateMeshPlane() {

  // 0th vertex is the center of the mesh.
  // Vertices 1 -> circles.(size-1) are other vertices.
  for (int i = 0; i < meshPoints; i++) {
    // Get ith circle's position.
    glm::vec2 pos;
    
    if (i == meshPoints - 1) {
      pos = circles[1] -> getPosition();
    } else {
      pos = circles[i] -> getPosition();
    }
  
    // Update ith mesh vertex's position.
    auto vertex = mesh.getVertices()[i];
    vertex.x = pos.x;
    vertex.y = pos.y;
    mesh.setVertex(i, vertex);
  }
}

void ofApp::setupBox2dSprings() {
  auto vertices = mesh.getVertices();
  
  // Construct circles at all the vertices from the mesh.
  for (int i = 0; i < meshPoints - 1; i++) {
    auto circle = std::make_shared<ofxBox2dCircle>();
    circle -> setPhysics(0.1, 0.1, 0.1);
    circle -> setup(box2d.getWorld(), vertices[i].x, vertices[i].y, circleRadius);
    circles.push_back(circle);
  }
  
  // Connect center circle to all the circles.
  // Start from 1st circle because the 0th circle is connected other vertices.
  // We go 1 less than the mesh points because last point in the mesh is the
  // same as the second point (after center).
  for(auto i=1; i<circles.size(); i++) {
    auto joint = std::make_shared<ofxBox2dJoint>();
    joint -> setup(box2d.getWorld(), circles[0] -> body, circles[i] -> body, 4.f, 1.f);
    joint->setLength(meshRadius);
    joints.push_back(joint);
  }
  
  // Connect joints with each other.
  // We go 1 less than the mesh points because last point in the mesh is the
  // same as the second point (after center).
  for(auto i=1; i<circles.size(); i++) {
    auto joint = std::make_shared<ofxBox2dJoint>();
    
    // At last index, make a spring back to 0.
    int fromIdx = i; int toIdx = i+1;
    if (i == circles.size() - 1) {
      toIdx = 1;
    }
    
    joint -> setup(box2d.getWorld(), circles[fromIdx] -> body, circles[toIdx] -> body, 4.f, 1.f);
    joint->setLength(jointLength);
    joints.push_back(joint);
  }
}
