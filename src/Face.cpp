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
  float faceMeshRadius = sqrt(faceArea/PI);

  faceMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);

  // Center vertex for the triangle fan. // random
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

void Face::createFaceBox2DSprings() {

}

void Face::updateFaceMeshPlane() {

}

void Face::showMesh() {

}

void Face::showMeshWireframe() {
  
}

void Face::showSoftBody() {

}

void Face::showTexture() {

}
