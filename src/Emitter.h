#pragma once

#include "ofMain.h"
#include "Shape.h"
#include "Sprite.h"

//
//  Manages all Sprites in a system.  You can create multiple systems
//


class SpriteList {
public:
	void add(Sprite* sprite);
	void remove(int);
	void update();
	void draw();
	
	void setShowImage(bool value);
	void setShowEnemyVerts(bool value);

	vector<Sprite*> sprites;
};


//  General purpose Emitter class for emitting sprites
//  This works similar to a Particle emitter
//
class Emitter : public Shape {
public:
	
	Emitter();
	void init();
	void draw();
	void start();
	void stop();
	void setLifespan(float);
	void setVelocity(const glm::vec3 v);
	void setChildImage(ofImage);
	void setImage(ofImage);
	void setRate(float);
	void setMultiplier(int i);
	void update();

	
	void setShowImage(bool value);
	void setShowEnemyVerts(bool value);

		

	// virtuals - can overloaded
	virtual void moveSprite(Sprite *);
	virtual void spawnSprite();
	virtual bool inside(glm::vec3 p) {
		glm::vec3 s = glm::inverse(getTransform()) * glm::vec4(p, 1);
		return (s.x > -width / 2 && s.x < width / 2 && s.y > -height / 2 && s.y < height / 2);
	}

	SpriteList *sys;


	SpriteList* getSpriteList() {
		return sys;
	}
	float rate;
	glm::vec3 velocity;
	float lifespan;
	bool started;
	float lastSpawned;
	ofImage childImage;
	ofImage image;
	bool drawable;
	bool haveChildImage;
	bool showChildImage;
	bool showChildEnemyVerts;
	int agentMultiplier;
	bool haveImage;
	float width, height;
};

