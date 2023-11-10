#include "ofApp.h"
//----------------------------------------------------------------------------------
//
// This example code demonstrates the use of an "Emitter" class to emit Sprites
// and set them in motion. The concept of an "Emitter" is taken from particle
// systems (which we will cover next week).
//
// The Sprite class has also been upgraded to include lifespan, velocity and age
// members.   The emitter can control rate of emission and the current velocity
// of the particles. In this example, there is no acceleration or physics, the
// sprites just move simple frame-based animation.
//
// The code shows a way to attach images to the sprites and optional the
// emitter (which is a point source) can also have an image.  If there are
// no images attached, a placeholder rectangle is drawn.
// Emitters  can be placed anywhere in the window. In this example, you can drag
// it around with the mouse.
//
// OF has an add-in called ofxGUI which is a very simple UI that is useful for
// creating sliders, buttons and fields. It is not recommended for commercial 
// game development, but it is useful for testing.  The "h" key will hide the GUI
// 
// If you want to run this example, you need to use the ofxGUI add-in in your
// setup.
//
//
//  Kevin M. Smith - CS 134 SJSU




//  Add a Sprite to the Sprite System
//
void SpriteList::add(Sprite* sprite) {
	
	sprites.push_back(sprite);
}

// Remove a sprite from the sprite system. Note that this function is not currently
// used. The typical case is that sprites automatically get removed when the reach
// their lifespan.
//
void SpriteList::remove(int i) {
	sprites.erase(sprites.begin() + i);
}


//  Update the SpriteSystem by checking which sprites have exceeded their
//  lifespan (and deleting).  Also the sprite is moved to it's next
//  location based on velocity and direction.
//
void SpriteList::update() {
	cout << "hello!"; 
	if (sprites.size() == 0) return;
	vector<Sprite*>::iterator s = sprites.begin();
	vector<Sprite*>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sprites.end()) {
		if ((*s)->lifespan != -1 && (*s)->age() > (*s)->lifespan) {
			//			cout << "deleting sprite: " << s->name << endl;
			tmp = sprites.erase(s);
			s = tmp;
		}
		else s++;
	}
	// Check collisions
	
	//  Move sprite
	//
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i]->pos += sprites[i]->velocity / ofGetFrameRate();
	}
	//dealWithCollisions();

}

//  Render all the sprites
//
void SpriteList::draw() {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i]->draw();
	}
}

void SpriteList::setShowImage(bool value) {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i]->setShowImage(value);
	}
}

void SpriteList::setShowEnemyVerts(bool value) {
	for (int i = 0; i < sprites.size(); i++) {
		sprites[i]->setShowVerts(value);
	}
}



Emitter::Emitter() {
	sys = new SpriteList();
	init();
}


void Emitter::init() {
	lifespan = 3000;    // default milliseconds
	started = false;

	lastSpawned = 0;
	rate = 1;    // sprites/sec
	haveChildImage = true;
	showChildImage = true;
	haveImage = false;
	velocity = ofVec3f(100, 100, 0);
	drawable = true;
	width = 50;
	height = 50;
	agentMultiplier = 1; 
}



//  Draw the Emitter if it is drawable. In many cases you would want a hidden emitter
//
//
void Emitter::draw() {

	// draw the Emitter itself 
	// note: set drawable=false if you want the emitter to be invisible
	//
	if (drawable) {

		ofPushMatrix();
		ofMultMatrix(getTransform());

		if (haveImage) {
			image.draw(-image.getWidth() / 2.0, -image.getHeight() / 2.0);
		}
		else {
			// draw some default shape (square) if drawable but no image attached.
			//
			ofSetColor(0, 0, 200);
			ofDrawRectangle(-width / 2, -height / 2, width, height);
		}

		ofPopMatrix();
	}

	// draw sprite system
	//
	sys->draw();
	
}

//  Update the Emitter. If it has been started, spawn new sprites with
//  initial velocity, lifespan, birthtime.
//
void Emitter::update() {
	if (!started) return;

	float time = ofGetElapsedTimeMillis();

	if ((time - lastSpawned) > (1000.0 / rate)) {

		// call virtual to spawn a new sprite
		//
		for (int i = 0; i < agentMultiplier; i++) {
			spawnSprite();
		}
		lastSpawned = time;
	}



	// update sprite list
	//
	if (sys->sprites.size() == 0) return;
	vector<Sprite*>::iterator s = sys->sprites.begin();
	vector<Sprite*>::iterator tmp;

	// check which sprites have exceed their lifespan and delete
	// from list.  When deleting multiple objects from a vector while
	// traversing at the same time, use an iterator.
	//
	while (s != sys->sprites.end()) {
		if ((*s)->lifespan != -1 && (*s)->age() > (*s)->lifespan) {
			//			cout << "deleting sprite: " << s->name << endl;
			tmp = sys->sprites.erase(s);
			s = tmp;
		}
		else s++;
	}
	//sys->dealWithCollisions();
	
	for (int i = 0; i < sys->sprites.size(); i++) {
		moveSprite(sys->sprites[i]);
	}
	
}
void Emitter::setShowImage(bool value) {
	sys->setShowImage(value);
	showChildImage = value;

}
void Emitter::setShowEnemyVerts(bool value) {
	sys->setShowEnemyVerts(value);
	showChildEnemyVerts = value;

}
// virtual function to move sprite (can be overloaded)
//
void Emitter::moveSprite(Sprite *sprite) {
	float dt = 1.0 / ofGetFrameRate();
	sprite->pos += sprite->velocity * dt;	
}

// virtual function to spawn sprite (can be overloaded)
//
void Emitter::spawnSprite() {
	Sprite* sprite = new Sprite();
	if (haveChildImage) sprite->setImage(childImage);
	sprite->spriteType = Sprite::SpriteType::FUGU_ENEMY;
	sprite->setShowImage(showChildImage);
	sprite->setShowVerts(showChildEnemyVerts);
	sprite->velocity = velocity;
	sprite->lifespan = lifespan;
	sprite->rotationSpeed;
	sprite->pos = pos;
	sprite->birthtime = ofGetElapsedTimeMillis();
	sys->add(sprite);

	ofApp* theApp = (ofApp*)ofGetAppPtr();
	theApp->sprites.push_back(sprite);
	cout << "\nNew Sprite Pushed";
}

// Start/Stop the emitter.
//
void Emitter::start() {
	started = true;
	lastSpawned = ofGetElapsedTimeMillis();
}

void Emitter::stop() {
	started = false;
}


void Emitter::setLifespan(float life) {
	lifespan = life;
}

void Emitter::setVelocity(const glm::vec3 v) {
	velocity = v;
}

void Emitter::setChildImage(ofImage img) {
	childImage = img;
	haveChildImage = true;
}

void Emitter::setImage(ofImage img) {
	image = img;
	haveImage = true;
}

void Emitter::setRate(float r) {
	rate = r;
}

void Emitter::setMultiplier(int i) {
	agentMultiplier = i;
}
