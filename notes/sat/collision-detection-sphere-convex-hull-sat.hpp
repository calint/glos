  static bool
  isCollisionWithSphere(const glm::vec3 &sphereCenter, float sphereRadius,
                        const std::vector<glm::vec3> &worldPoints,
                        const std::vector<glm::vec3> &worldNormals) {
    // Check for separation along each normal
    for (const auto &normal : worldNormals) {
      float minProjection = glm::dot(worldPoints[0], normal);
      float maxProjection = minProjection;

      // Project both the sphere and the convex volume onto the normal
      for (const auto &point : worldPoints) {
        float projection = glm::dot(point, normal);
        if (projection < minProjection) {
          minProjection = projection;
        } else if (projection > maxProjection) {
          maxProjection = projection;
        }
      }

      float sphereProjection = glm::dot(sphereCenter, normal);

      // Check for separation
      if (sphereProjection + sphereRadius < minProjection ||
          sphereProjection - sphereRadius > maxProjection) {
        return false; // Separating axis found, no collision
      }
    }

    return true; // No separating axis found, collision detected
  }