#include "Particle.h"
#include "Emitter.h"
#include "ofApp.h"


Particle::Particle() {

	// initialize particle with some reasonable values first;
	//
	velocity.set(0, 0, 0);
	acceleration.set(0, 0, 0);
	position.set(0, 0, 0);
	forces.set(0, 0, 0);
	lifespan = 2;
	birthtime = 0;
	radius = .2;
	damping = .98;
	mass = 1;
	color = ofColor::whiteSmoke;
}

void Particle::draw() {
//	ofSetColor(color);
	ofSetColor(color);
	ofDrawSphere(position, radius);
}

// write your own integrator here.. (hint: it's only 3 lines of code)
//
void Particle::integrate() {

	
	// interval for this step
	//
	float dt = 1.0 / ofGetFrameRate();

	// update position based on velocity
	//
	position += (velocity * dt);
	
	if (radius == 3.0) {
		checkIntersectionWithEnemies();
	}
	
	// update acceleration with accumulated paritcles forces
	// remember :  (f = ma) OR (a = 1/m * f)
	//
	ofVec3f accel = acceleration;    // start with any acceleration already on the particle
	accel += (forces * (1.0 / mass));
	velocity += accel * dt;

	// add a little damping for good measure
	//
	velocity *= damping;

	// clear forces on particle (they get re-added each step)
	//
	forces.set(0, 0, 0);

	//check if the bullets intersect with enemies
	
}
void Particle::checkIntersectionWithEnemies() {
	ofApp* theApp = (ofApp*)ofGetAppPtr();
	for (size_t i = 0; i < theApp->sprites.size(); ++i) {
		if (theApp->sprites[i]->inside(position)) {
			lifespan = 0;
			theApp->sprites[i]->lifespan = 0;
			theApp->makeExplosion(position);
			theApp->targetHitSound();
			if (theApp->energy < theApp->maxEnergy) {
				theApp->energy += 1;
			}
			
		}
	
	}


}
//  return age in seconds
//
float Particle::age() {
	return (ofGetElapsedTimeMillis() - birthtime)/1000.0;
}


