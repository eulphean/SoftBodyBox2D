// Author: Amay Kataria
// Date: 02/23/2018

#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofEnableDepthTest();
    ofBackground(0);
  
    // Setup Box2d
    box2d.init();
    box2d.setGravity(0.3f, 0.3f);
    box2d.createBounds(ofRectangle(0, 0, ofGetWidth(), ofGetHeight()));
    box2d.setFPS(60.0);
    box2d.registerGrabbing(); // Enable grabbing the circles.
  
    // Commands
    showMesh = true;
    showSoftBody = false;
    showTexture = false;
  
    // OSC handler
    oscHandler.setup();
  
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
      auto& instance = instances[0];
      auto rect = instance.getBoundingBox();
      // Push it in the vector.
      boundingBoxes.push_back(rect);
    }
    
    // Go through all the bounding boxes currently active and
    // create a new face. Turn off the flag to enable this creation
    // for now. We can enable that flag for the system to recreate
    // these faces by emptying the current set of faces.
    if (boundingBoxes.size() > 0) {
      if (createMeshAndSprings) {
        // Clear existing faces.
        faces.clear();
        for (auto &r : boundingBoxes) {
          Face face;
          face.createFaceMesh(r);
          face.createFaceBox2DSprings();
          faces.push_back(face);
        }
        createMeshAndSprings = false;
      }
    }
    
    // Go through each face and update it based on its movement in the
    // environment.
    for (auto &face : faces) {
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
  // showBoundingRect();
  
  if (showDebugFace) {
    tracker.drawDebug();
  }
  
  // Show just the mesh with
  if (showMesh) {
    if (boundingBoxes.size() > 0) {
        auto&rect = boundingBoxes[0];
        ofPushStyle();
          ofSetColor(ofColor::white);
          ofPushMatrix();
            //ofTranslate(ofGetWidth()/2 - rect.getWidth()/2, ofGetHeight()/2 - rect.getHeight()/2);
            grabber.getTexture().bind();
              if (showTexture) {
                faceMesh.draw();
              } else {
                faceMesh.drawWireframe();
              }
            grabber.getTexture().unbind();
          ofPopMatrix();
        ofPopStyle();
    }
  }
  
  // Grabber.
  if (showGrabber) {
    grabber.draw(0, 0);
  }

  // Show springs and joints.
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
}

void ofApp::createFaceMesh() {
  // Get the first bounding box, which is my face for now.
  // faceMeshes.clear();
  auto &r = boundingBoxes[0];

  // Clear the mesh.
  faceMesh.clear();

  // Store all the important face variables that I'll use
  // to give the right coordinates for it on the mesh.
  float faceX = r.getX();
  float faceY = r.getY();
  float faceWidth = r.getWidth();
  float faceHeight = r.getHeight();
  float faceArea = r.getArea();
  glm::vec3 faceCenter = r.getCenter();

  // Calculate faceMeshRadius from faceArea.
  faceMeshRadius = sqrt(faceArea/PI);

  faceMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

  // Center vertex for the triangle fan.
  glm::vec3 center = glm::vec3(ofGetWidth()/2, ofGetHeight()/2, 0);
  faceMesh.addVertex(center);

  // Center vertex of the texture (ie. the center of the bounding rectangle for the texture)
  faceMesh.addTexCoord(glm::vec2(faceCenter.x, faceCenter.y));

  // Ratio between the texture and the circular mesh.
  float sizeRatio = faceArea/(PI * faceMeshRadius * faceMeshRadius);

  // Add vertices around the center to form a circle.
  for(int i = 1; i < meshPoints; i++){
    float n = ofMap(i, 1, meshPoints - 1, 0.0, TWO_PI);
    float x = sin(n);
    float y = cos(n);
    
    // Mesh vertex.
    faceMesh.addVertex({center.x + (x * faceMeshRadius), center.y + y * faceMeshRadius, 0});
    
    // Mesh texture scaled coordinate.
    faceMesh.addTexCoord(glm::vec2(faceX + faceWidth/2 + x * faceMeshRadius * sizeRatio, faceY + faceWidth/2 + y * faceMeshRadius * sizeRatio));
  }
}

void ofApp::updateFaceMeshPlane() {
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
    auto vertex = faceMesh.getVertices()[i];
    vertex.x = pos.x;
    vertex.y = pos.y;
    faceMesh.setVertex(i, vertex);
  }
}

void ofApp::createFaceBox2DSprings() {
  auto vertices = faceMesh.getVertices();
  
  // Clear them to make them again.
  circles.clear();
  joints.clear();
  
  // Construct circles at all the vertices from the mesh.
  for (int i = 0; i < meshPoints - 1; i++) {
    auto circle = std::make_shared<ofxBox2dCircle>();
    circle -> setPhysics(0.5, 0.5, 0.5);
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
    joint->setLength(faceMeshRadius);
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
    
    joint -> setup(box2d.getWorld(), circles[fromIdx] -> body, circles[toIdx] -> body, 1.f, 1.f);
    joint->setLength(jointLength);
    joints.push_back(joint);
  }
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

void ofApp::trackMeshCentroidForWekinator() {
  glm::vec2 centroid = mesh.getCentroid();
  oscHandler.sendPositionToWekinator(centroid);
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

void ofApp::createRectFaceMesh() {
  // Get the first bounding box, which is my face for now.
  auto &r = boundingBoxes[0];

  // Clear the mesh.
  faceMesh.clear();
  
  faceMesh.setMode(OF_PRIMITIVE_TRIANGLES);
  
  int nCols = 5;
  int nRows = 5;
  float faceX = r.getX();
  float faceY = r.getY();
  float faceWidth = r.getWidth();
  float xOffset = faceWidth/nRows;
  float faceHeight = r.getHeight();
  float yOffset = faceHeight/nCols;
  float faceArea = r.getArea();
  glm::vec3 faceCenter = r.getCenter();

  // Setup a 5 x 5 mesh.
  for (int y = 0; y < nCols; y++) {
    for (int x = 0; x < nRows; x++) {
      float ix = faceWidth * x / (nCols - 1);
      float iy = faceHeight * y / (nRows - 1);
      faceMesh.addVertex({ix, iy, 0});
      // Texture coordinates will be of the texture bounded by
      // rectangle.. Offset the texCoordinate by faceX and faceY.
      faceMesh.addTexCoord(glm::vec2(ix + faceX, iy + faceY));
    }
  }
  
  // We don't draw the last row / col (nRows - 1 and nCols - 1) because it was
  // taken care of by the row above and column to the left.
  for (int y = 0; y < nRows - 1; y++)
  {
      for (int x = 0; x < nCols - 1; x++)
      {
          // Draw T0
          // P0
          faceMesh.addIndex((y + 0) * nCols + (x + 0));
          // P1
          faceMesh.addIndex((y + 0) * nCols + (x + 1));
          // P2
          faceMesh.addIndex((y + 1) * nCols + (x + 0));

          // Draw T1
          // P1
          faceMesh.addIndex((y + 0) * nCols + (x + 1));
          // P3
          faceMesh.addIndex((y + 1) * nCols + (x + 1));
          // P2
          faceMesh.addIndex((y + 1) * nCols + (x + 0));
      }
  }
}
