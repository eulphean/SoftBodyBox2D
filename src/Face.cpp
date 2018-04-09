// Author: Amay Kataria
// Date: 02/23/2018

#include "Face.h"

void Face::createFaceMesh(ofRectangle r) {
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

  // Start creating a face randomly on the screen. 
  //glm::vec3 center = glm::vec3(ofGetWidth()/2, ofGetHeight()/2, 0);
  glm::vec3 center = glm::vec3(ofRandom(faceMeshRadius, ofGetWidth() - faceMeshRadius), ofRandom(faceMeshRadius, ofGetHeight() - faceMeshRadius), 0);
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

void Face::createFaceBox2DSprings(ofxBox2d &box2d) {
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

void Face::updateFaceMeshPlane() {
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

void Face::draw(ofVideoGrabber &grabber, bool showMesh, bool showTexture, bool showSoftBody) {
  // Show just the mesh with
  if (showMesh) {
    if (faceMesh.getVertices().size() > 0) {
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

