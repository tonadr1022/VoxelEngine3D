//
// Created by Tony Adriansen on 12/30/2023.
//

#include "ViewFrustum.hpp"
void ViewFrustum::updatePlanes(const glm::mat4 &projectionMatrix, const glm::mat4 &viewMatrix) {
  const glm::mat4 clipMatrix = projectionMatrix * viewMatrix;
// source: https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf

// Left clipping plane
  m_planes[0].normal.x = clipMatrix[0][3] + clipMatrix[0][0];
  m_planes[0].normal.y = clipMatrix[1][3] + clipMatrix[1][0];
  m_planes[0].normal.z = clipMatrix[2][3] + clipMatrix[2][0];
  m_planes[0].distance = clipMatrix[3][3] + clipMatrix[3][0];

  // Right clipping plane
  m_planes[1].normal.x = clipMatrix[0][3] - clipMatrix[0][0];
  m_planes[1].normal.y = clipMatrix[1][3] - clipMatrix[1][0];
  m_planes[1].normal.z = clipMatrix[2][3] - clipMatrix[2][0];
  m_planes[1].distance = clipMatrix[3][3] - clipMatrix[3][0];

  // Top clipping plane
  m_planes[2].normal.x = clipMatrix[0][3] - clipMatrix[0][1];
  m_planes[2].normal.y = clipMatrix[1][3] - clipMatrix[1][1];
  m_planes[2].normal.z = clipMatrix[2][3] - clipMatrix[2][1];
  m_planes[2].distance = clipMatrix[3][3] - clipMatrix[3][1];

  // Bottom clipping plane
  m_planes[3].normal.x = clipMatrix[0][3] + clipMatrix[0][1];
  m_planes[3].normal.y = clipMatrix[1][3] + clipMatrix[1][1];
  m_planes[3].normal.z = clipMatrix[2][3] + clipMatrix[2][1];
  m_planes[3].distance = clipMatrix[3][3] + clipMatrix[3][1];

  // Near clipping plane
  m_planes[4].normal.x = clipMatrix[0][3] + clipMatrix[0][2];
  m_planes[4].normal.y = clipMatrix[1][3] + clipMatrix[1][2];
  m_planes[4].normal.z = clipMatrix[2][3] + clipMatrix[2][2];
  m_planes[4].distance = clipMatrix[3][3] + clipMatrix[3][2];

  // Far clipping plane
  m_planes[5].normal.x = clipMatrix[0][3] - clipMatrix[0][2];
  m_planes[5].normal.y = clipMatrix[1][3] - clipMatrix[1][2];
  m_planes[5].normal.z = clipMatrix[2][3] - clipMatrix[2][2];
  m_planes[5].distance = clipMatrix[3][3] - clipMatrix[3][2];

  // Normalize the plane equations
  for (auto &plane : m_planes) {
    float length = glm::length(plane.normal);
    plane.normal /= length;
    plane.distance /= length;
  }
}

bool ViewFrustum::isBoxInFrustum(const AABB &boundingBox) const {
  for (auto &plane : m_planes) {
    // if plane normal is positive, use upper corner of bounding box in that direction.
    // (if all three normals are positive, max vertex is used)
    // if plane normal is negative, use lower corner of bounding box in that direction.
    // (if all three normals are negative, min vertex is used)
    glm::vec3 positiveVertex = boundingBox.min;
    if (plane.normal.x >= 0) {
      positiveVertex.x = boundingBox.max.x;
    }
    if (plane.normal.y >= 0) {
      positiveVertex.y = boundingBox.max.y;
    }
    if (plane.normal.z >= 0) {
      positiveVertex.z = boundingBox.max.z;
    }
    // LEARNING NOTES:

    // dist point from origin is pV.
    // projection of pV onto plane normal is (pV dot plane.normal)/(|plane.normal|) * plane.normal
    // plane.normal is length 1, so projection of pV onto plane normal is pV dot plane.normal * plane.normal
    // But, only care about length of projection, so use pV dot plane.normal.
    // This length is the scalar projection of pV onto the plane normal (vector projection would be
    // pV dot plane.noraml * plane.normal / |plane.normal|, but |plane.normal| is 1)

    // call the above length e. if plane.distance > e, the vertex is behind the plane and not in frustum
    // so not in frustum if plane.distance - e > 0
    // also known as signed distance
    if (plane.distanceToPoint(positiveVertex) < 0) return false;
  }
  return true;
}


