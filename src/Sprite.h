#pragma once

#include "Shape.h"


// Base class for a Sprite. Can be instantiated on it's own (deafult)
// or subclassed to create a custom sprite.  Includes Shape transformations.
// If the sprite has no image set on it, then a simple triangle will be drawn.
//

class Sprite : public Shape {
public:
	

	enum SpriteType {
		FUGU_ENEMY,
		TARGET
	};

	Sprite() {
		// default geometry (triangle) if no image attached.
		//
		verts.push_back(glm::vec3(-20, 30, 0));
		verts.push_back(glm::vec3(20, 30, 0));
		verts.push_back(glm::vec3(0, -30, 0));
	}
	
	// some functions for highlighting when selected
	//
	void draw() {
		if (bShowImage) {
			ofPushMatrix();
			ofSetColor(ofColor::white);
			ofMultMatrix(getTransform());
			//ofDrawRectangle(0, 0, spriteImage.getWidth(), spriteImage.getHeight());
			//float xOffset = 0;
			//float yOffset = 0;
			float xOffset = -spriteImage.getWidth() / 2;
			float yOffset = -spriteImage.getHeight() / 2;
			spriteImage.draw(xOffset, yOffset);
			if (bShowVerts) {
				int dotRadius = 5;
				if (spriteType == TARGET) dotRadius = 15;
				for (const auto& vert : getVerts()) {
					ofSetColor(ofColor::red);
					
					ofDrawCircle(vert.x + xOffset, vert.y + yOffset, dotRadius); // Draw small circles at each vertex
				}
			}
			
			
			ofPopMatrix();
		}
		else
		{
			if (spriteType == TARGET) ofSetColor(ofColor::white);
			else ofSetColor(ofColor::yellow);
			ofPushMatrix();
			ofMultMatrix(getTransform());
			ofDrawTriangle(verts[0], verts[1], verts[2]);
			
			ofPopMatrix();
			
		}
	}

	float age() {
		return (ofGetElapsedTimeMillis() - birthtime);
	}

	void setImage(ofImage img) {
		spriteImage = img;
		bShowImage = true;
		width = img.getWidth();
		height = img.getHeight();
	
	}


	void setShowImage(bool value) {
		bShowImage = value;
	}

	void setShowVerts(bool value) {
		bShowVerts = value;
	}



	virtual bool inside(const glm::vec3 p);
	virtual void update() {}
	void integrate();



	bool willMoveOutsideScreen(const glm::vec3 & point);

	glm::vec3 centralForceDirection();

	bool insideTriangle(const glm::vec3 p);

	vector<glm::vec3> getTargetTrianglePoints();
	

	void setSelected(bool state) { bSelected = state; }
	void setHighlight(bool state) { bHighlight = state; }
	bool isSelected() { return bSelected; }
	bool isHighlight() { return bHighlight; }


	bool bHighlight = false;
	bool bSelected = false;
	bool bShowImage = false;
	bool bShowVerts = false;

	glm::vec3 velocity = glm::vec3(0, 0, 0);
	glm::vec3 acceleration = glm::vec3(0, 0, 0);
	glm::vec3 force = glm::vec3(0, 0, 0);
	float angularVelocity = 0;
	float angularAcceleration = 0;

	float mass = 1.0;
	float damping = .99;
	SpriteType spriteType;
	float thrust = 0;
	bool bThrust = false;
	float speed = 0.0;
	float rotationSpeed = 0.0;
	float birthtime = 0; // elapsed time in ms
	float lifespan = -1;  //  time in ms
	string name =  "UnammedSprite";
	float width = 40;
	float height = 40;

	ofImage spriteImage;

	// default verts for polyline shape if no image on sprite
	//
	vector<glm::vec3> verts;
	



	//this method stores all the coordinates of the fugu sprite verts (spikes, sides etc)
	//so we can check them for collision with other objects
	vector<glm::vec3> getVerts() {
		if (bShowImage and spriteType == TARGET){
			return getTargetVerts();
		}
		if (bShowImage and spriteType == FUGU_ENEMY) {
			return getFuguVerts();
		}
		return verts;
	}
	static vector<glm::vec3> getTargetVerts() {
		vector<glm::vec3> targetVerts;
		targetVerts.emplace_back(533, 0, 0);
		targetVerts.emplace_back(820, 168, 0);
		targetVerts.emplace_back(200, 168, 0);
		targetVerts.emplace_back(0, 503, 0);
		targetVerts.emplace_back(82, 830, 0);
		targetVerts.emplace_back(950, 830, 0);
		targetVerts.emplace_back(750, 963, 0);
		targetVerts.emplace_back(300, 963, 0);
		targetVerts.emplace_back(95, 290, 0);
		targetVerts.emplace_back(932, 290, 0);
		targetVerts.emplace_back(531, 1024, 0);
		targetVerts.emplace_back(1024, 503, 0);

		return targetVerts;
	}
	static vector<glm::vec3> getFuguVerts() {
		vector<glm::vec3> fuguVerts;
		fuguVerts.push_back(glm::vec3(223, 48, 0));
		fuguVerts.emplace_back(336, 60, 0);
		fuguVerts.emplace_back(354, 137, 0);
		fuguVerts.emplace_back(337, 277, 0);
		fuguVerts.emplace_back(353, 225, 0);
		fuguVerts.emplace_back(61, 223, 0);
		fuguVerts.emplace_back(95, 301, 0);
		fuguVerts.emplace_back(187, 305, 0);
		fuguVerts.emplace_back(47, 161, 0);
		fuguVerts.emplace_back(73, 95, 0);
		fuguVerts.emplace_back(134, 63, 0);
		fuguVerts.emplace_back(252, 311, 0);

		/* additional points for more accuracy but slower calulations...
		fuguVerts.emplace_back(243, 65, 0);
		fuguVerts.emplace_back(349, 80, 0);
		fuguVerts.emplace_back(351, 111, 0);
		fuguVerts.emplace_back(365, 155, 0);
		fuguVerts.emplace_back(361, 191, 0);
		fuguVerts.emplace_back(339, 261, 0);
		fuguVerts.emplace_back(284, 295, 0);
		fuguVerts.emplace_back(238, 307, 0);
		fuguVerts.emplace_back(225, 314, 0);
		fuguVerts.emplace_back(204, 312, 0);
		fuguVerts.emplace_back(184, 313, 0);
		fuguVerts.emplace_back(171, 304, 0);
		fuguVerts.emplace_back(162, 293, 0);
		fuguVerts.emplace_back(77, 284, 0);
		fuguVerts.emplace_back(67, 245, 0);
		fuguVerts.emplace_back(51, 195, 0);
		fuguVerts.emplace_back(60, 115, 0);
		fuguVerts.emplace_back(106, 71, 0);
		fuguVerts.emplace_back(166, 61, 0);
		fuguVerts.emplace_back(197, 68, 0);
		*/
		return fuguVerts;
	}
};

