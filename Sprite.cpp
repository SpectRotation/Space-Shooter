#include "Sprite.h"




//
	// inside - check if point is inside player (can be an image or TriangleShape if no image)
	//
bool Sprite::inside(const glm::vec3 p) {

	// if there is no sprite image attached, then just use triangle case.
	//
	if (!bShowImage) {
		return insideTriangle (p);
	}

	// if sprite image attached, then first check if point is inside bounds of image
	// in object space.  If point is inside bounds, then make sure the point is in
	// opaque part of image.
	//
	glm::vec3 s = glm::inverse(getTransform()) * glm::vec4(p, 1);
	int w = spriteImage.getWidth();
	int h = spriteImage.getHeight();
	if (s.x > -w / 2 && s.x < w / 2 && s.y > -h / 2 && s.y < h / 2) {
		int x = s.x + w / 2;
		int y = s.y + h / 2;
		ofColor color = spriteImage.getColor(x, y);
	
		return (color.a != 0);   // check if color is opaque (not the transparent background)
	}
	else return false;
}


bool Sprite::insideTriangle(const glm::vec3 p) {

	// transform the screen space point p to the triangle's local 
	// oordinate system  (object space);  this will take into account any
	// rotation, translation or scale on the object.
	//
	glm::vec4 p2 = glm::inverse(getTransform()) * glm::vec4(p, 1);

	glm::vec3 v1 = glm::normalize(verts[0] - p2);
	glm::vec3 v2 = glm::normalize(verts[1] - p2);
	glm::vec3 v3 = glm::normalize(verts[2] - p2);
	float a1 = glm::orientedAngle(v1, v2, glm::vec3(0, 0, 1));
	float a2 = glm::orientedAngle(v2, v3, glm::vec3(0, 0, 1));
	float a3 = glm::orientedAngle(v3, v1, glm::vec3(0, 0, 1));
	if (a1 < 0 && a2 < 0 && a3 < 0) return true;
	else return false;

}

std::vector<glm::vec3> Sprite::getTargetTrianglePoints() {
	std::vector<glm::vec3> points;

	// Get the transformation matrix of the target
	glm::mat4 transform = getTransform();

	// Add the transformed vertices to the points array
	for (int i = 0; i < 3; i++) {
		glm::vec3 transformedVertex = transform * glm::vec4(verts[i], 1.0f);
		points.push_back(transformedVertex);
	}

	// Compute and add the transformed midpoints to the points array
	for (int i = 0; i < 3; i++) {
		// First midpoint (1/3 along the edge)
		glm::vec3 localMidpoint1 = glm::mix(verts[i], verts[(i + 1) % 3], 1.0f / 3.0f);
		glm::vec3 transformedMidpoint1 = transform * glm::vec4(localMidpoint1, 1.0f);
		points.push_back(transformedMidpoint1);

		// Second midpoint (2/3 along the edge)
		glm::vec3 localMidpoint2 = glm::mix(verts[i], verts[(i + 1) % 3], 2.0f / 3.0f);
		glm::vec3 transformedMidpoint2 = transform * glm::vec4(localMidpoint2, 1.0f);
		points.push_back(transformedMidpoint2);
	}

	return points;
}


