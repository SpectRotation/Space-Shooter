#include "ofApp.h"

// spawnSprite - we override this function in the Emitter class to spawn our 
// "custom" Agent sprite.
//
void AgentEmitter::spawnSprite() {
	
	Agent *sprite = new Agent();
	
	if (haveChildImage) sprite->setImage(childImage);
	sprite->setShowImage(showChildImage);
	sprite->setShowVerts(showChildEnemyVerts);
	sprite->velocity = velocity;
	sprite->lifespan = lifespan;
	sprite->spriteType = Sprite::SpriteType::FUGU_ENEMY;

	ofApp* theApp = (ofApp*)ofGetAppPtr();  
	glm::vec3 targetPos = theApp->target->pos;

	// Set random position away from the target

	glm::vec3 randomPos;
	float distanceToTarget;
	do {
		float randomX = ofRandom(0, ofGetWidth());
		float randomY = ofRandom(0, ofGetHeight());
		randomPos = glm::vec3(randomX, randomY, 0);
		distanceToTarget = glm::length(targetPos - randomPos);
	} while (distanceToTarget < 200);

	sprite->pos = randomPos;
	


	// Set sprites rotation to be turned on the target on spawn
	glm::vec3 v = glm::normalize(targetPos - sprite->pos);
	glm::vec3 u = glm::vec3(0, -1, 0);
	float a = glm::orientedAngle(u, v, glm::vec3(0, 0, 1));
	sprite->rot = glm::degrees(a);


	sprite->birthtime = ofGetElapsedTimeMillis();
	sys->add(sprite);
	
	theApp->sprites.push_back(sprite);
	
}

//  moveSprite - we override this function in the Emitter class to implment
//  "following" motion towards the target
//
void AgentEmitter::moveSprite(Sprite* sprite) {
	
	// Get a pointer to the app so we can access application
	// data such as the target sprite.
	//
	ofApp* theApp = (ofApp*)ofGetAppPtr();

	// rotate sprite to point towards target
	//  - find vector "v" from sprite to target
	//  - set rotation of sprite to align with v
	//
	glm::vec3 v = glm::normalize(theApp->target->pos - sprite->pos);
	glm::vec3 u = glm::vec3(0, -1, 0);
	float a = glm::orientedAngle(u, v, glm::vec3(0, 0, 1));
	
		
	float deltaRotation = glm::degrees(a) - sprite->rot;

	// Adjust for the -180/180 boundary crossing
	if (deltaRotation > 180) {
		deltaRotation -= 360;
	}
	else if (deltaRotation < -180) {
		deltaRotation += 360;
	}

	float limitedRotation;
	if (deltaRotation > sprite->rotationSpeed) {
		limitedRotation = sprite->rotationSpeed;
	}
	else if (deltaRotation < -sprite->rotationSpeed) {
		limitedRotation = -sprite->rotationSpeed;
	}
	else {
		limitedRotation = deltaRotation;
		//add acceleration if the enemy faces the target:
		
		//sprite->acceleration += cos(glm::radians(deltaRotation));

		//printf("\nCharge: angle = %f, cos = %f, acceleration = %f", deltaRotation, cos(deltaRotation), sprite->acceleration);
	}
	
	sprite->rot += limitedRotation;
	
	
	


	// Calculate new velocity vector

	glm::vec3 heading = glm::normalize(glm::rotate(glm::vec3(0, -1, 0), glm::radians(sprite->rot), glm::vec3(0, 0, 1)));

	sprite->velocity = glm::length(sprite->velocity) * heading;
	
	
	
	
	sprite->velocity += theApp->targetForce(sprite->pos);
	// Now move the sprite in the normal way (along velocity vector)
	//
	Emitter::moveSprite(sprite);
	// check if the sprite intersects with the target.
	
	if ((theApp->energy <=  0) && (theApp->currentState == theApp->GAME)) {
		theApp->currentState = theApp->POST_GAME;
		theApp->playedTime = ofToString(ofGetElapsedTimef(), 2);
		theApp->target->bThrust = false;
		theApp->engineSound.stop();
		theApp->collisionEmitter.stop();
		theApp->sprites.clear();
	}
		 
	


}

void ofApp::makeExplosion(ofVec3f position) {
	collisionEmitter.setPosition(position);
	collisionEmitter.start();
}


void ofApp::startGame() {
	
	
	
	currentState = GAME;
	ofResetElapsedTimeCounter();
	setupGame();


}
void ofApp::restartGame() {
	startGame();
}
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetVerticalSync(true);
	targetImage.load("images/target-ship.png");
	titleFont.load("fonts/Watchful.otf", 100);
	backgroundImg.load("images/dark_sky.png");
	post_game_background.load("images/game_over.png");
	guiDifficulty = 0;
	currentDifficulty = EASY;
	
	
	// create an image for sprites being spawned by emitter
	//
	if (defaultImage.load("images/monster.png")) {
		imageLoaded = true;
		showImage = true;
	}
	else {
		cout << "Can't open image file" << endl;
		//		ofExit();
	}
	
	ofSetBackgroundColor(ofColor::black);

	setupGUI();

	bHide = false;

	engineSound.load("sounds/thrusters-loop.wav");
	engineSound.setLoop(true);
	shooting.load("sounds/shot.wav");
	targetHit.load("sounds/targetHit.wav");
	collision.load("sounds/wetCollision.mp3");
	collision.setSpeed(2.0);
	collision.setVolume(0.5);
}


void ofApp::setupGUI() {
	gui.clear();
	gui.setup();
	float guiSpawnRate = 0.5; 
	float guiLifespan = 8;
	float guiEnemyVelocity = -140;
	float guiEnemyScale = 0.4;
	float guiEnemyRotation = 0.5;
	float guiTargetThrust = 400;
	float guiTargetScale = 0.17;
	float guiTargetForceCoefficient = 0.25;
	int guiEnemyMultiplier = 2;
	int guiEnergy = 100;
	currentDifficulty = EASY;
	switch (difficulty)
	{
	case 0:
		guiDifficulty = 0;
		break;
	case 1:
		currentDifficulty = DIFFICULT;
		guiSpawnRate = 0.5;
		guiLifespan = 8;
		guiEnemyVelocity = -160;
		guiEnemyScale = 0.3;
		guiEnemyRotation = 1.5;
		guiTargetThrust = 400;
		guiTargetScale = 0.15;
		guiTargetForceCoefficient = 0.4;
		guiEnemyMultiplier = 3;
		guiDifficulty = 1;
		guiEnergy = 60;
		break;
	case 2:
		currentDifficulty = IMPOSSIBLE;
		guiSpawnRate = 1;
		guiLifespan = 2;
		guiEnemyVelocity = -300;
		guiEnemyScale = 0.2;
		guiEnemyRotation = 2.0;
		guiTargetThrust = 700;
		guiTargetScale = 0.13;
		guiEnemyMultiplier = 3;
		guiTargetForceCoefficient = 0.8;
		guiDifficulty = 2;
		guiEnergy = 80;
		break;
	default:
		break;
	}
	maxEnergy = guiEnergy;
	gui.add(rate.setup("Spawn Rate", guiSpawnRate, 0, 8));
	gui.add(life.setup("Lifespan", guiLifespan, .1, 10));
	gui.add(enemyVelocity.setup("Enemy Velocity", ofVec3f(0, guiEnemyVelocity, 0), ofVec3f(-1000, -1000, -1000), ofVec3f(1000, 1000, 1000)));
	gui.add(enemyScale.setup("Enemy Scale", guiEnemyScale, .1, 2));
	gui.add(enemyRotationSpeed.setup("Enemy Rotation Speed (deg/Frame)", guiEnemyRotation, 0, 10));
	gui.add(spriteImage.setup("Show Sprite Image", imageLoaded));
	gui.add(enemyVerts.setup("Show Sprite Verts", showEnemyVerts));
	gui.add(targetThrust.setup("Target Speed", guiTargetThrust, 100, 1000));
	gui.add(targetScale.setup("Target Scale", guiTargetScale, .1, .5));
	gui.add(targetRotationSpeed.setup("Target Rotation Speed (deg/Frame)", 3, 1, 5));
	gui.add(targetForceCoefficient.setup("Target Force Coefficient", guiTargetForceCoefficient, 0, 1));
	gui.add(nEnergy.setup("Max Energy", guiEnergy, 1, 50));
	gui.add(nAgents.setup("Number of Agents at a time", guiEnemyMultiplier, 1, 10));
	gui.add(difficulty.setup("Difficulty", guiDifficulty, 0, 2));
	gui.add(turbMin.setup("Turbulence Min", ofVec3f(-200, -200, -200), ofVec3f(-500, -500, -500), ofVec3f(500,500, 500)));
	gui.add(turbMax.setup("Turbulence Max", ofVec3f( 200,  200,  200), ofVec3f(-500, -500, -500), ofVec3f(500, 500, 500)));
	gui.add(radialForceVal.setup("Radial Force", 5, 1, 100));
}
//--------------------------------------------------------------
void ofApp::setupGame(){
	
	//apply selected Difficulty
	difficulty = static_cast<int>(currentDifficulty);
	setupGUI();
	// create an emitter
	//
	bHide = true;
	
	emitter = new AgentEmitter();  // C++ polymorphism

	emitter->pos = glm::vec3(ofGetWindowWidth() / 2.0, ofGetWindowHeight() / 2.0, 0);
	emitter->drawable = false;
	if (imageLoaded) emitter->setChildImage(defaultImage);
	emitter->start();

	// create a target sprite to chase
	//
	target = new Sprite();
	target->pos = glm::vec3(400, 100, 0);
	target->setImage(targetImage);
	target->spriteType = Sprite::SpriteType::TARGET;
	target->scale = glm::vec3(0.17,0.17,0.17);
	maxEnergy = nEnergy;
	energy = maxEnergy;

	//emitter logic
	radialForce = new ImpulseRadialForce(500);
	turbForce = new TurbulenceForce(ofVec3f(turbMin->x, turbMin->y, turbMin->z), ofVec3f(turbMax->x, turbMax->y, turbMax->z));


	collisionEmitter.setPosition(ofVec3f(500, 500, 0));
	collisionEmitter.sys->addForce(radialForce);
	collisionEmitter.sys->addForce(turbForce);
	collisionEmitter.setVelocity(ofVec3f(0,100,0));
	collisionEmitter.setOneShot(true);
	collisionEmitter.setEmitterType(RadialEmitter);
	collisionEmitter.setGroupSize(200);
	if (currentDifficulty == IMPOSSIBLE) {
		collisionEmitter.setGroupSize(50);
	}

	leftGunEmitter.setPosition(ofVec3f(500, 500, 0));
	leftGunEmitter.sys->addForce(turbForce);
	leftGunEmitter.sys->addForce(radialForce);
	leftGunEmitter.setOneShot(true);
	//leftGunEmitter.setVelocity(ofVec3f(0, -500, 0));
	leftGunEmitter.setParticleRadius(3.0);
	leftGunEmitter.setGroupSize(1);


	rightGunEmitter.setPosition(ofVec3f(500, 500, 0));
	rightGunEmitter.sys->addForce(turbForce);
	rightGunEmitter.sys->addForce(radialForce);
	rightGunEmitter.setOneShot(true);
	//rightGunEmitter.setVelocity(ofVec3f(0, -500, 0));
	
	rightGunEmitter.setParticleRadius(3.0);
	rightGunEmitter.setGroupSize(1);
	//collisionEmitter.setEmitterType(DirectionalEmitter);
}

//--------------------------------------------------------------
void ofApp::update() {
	
	if (currentState != GAME) return; 
	if (static_cast<int>(currentDifficulty)!= difficulty) {
		cout << static_cast<int>(currentDifficulty) << " : " << difficulty;
		setupGUI();
		
	}
	//sounds
	if (target->bThrust && !engineSound.isPlaying()) {
		engineSound.play();
	}
	else if (!target->bThrust && engineSound.isPlaying()) {
		engineSound.stop();
	}
	


	//Target updates
	target->mass = 3; //useful for collisions
	if (!showImage) {
		target->scale = glm::vec3(2, 2, 2);
	} else { 
		target->scale = glm::vec3((float)targetScale, (float)targetScale, (float)targetScale);
	}
	target->thrust = targetThrust;
	target->rotationSpeed = targetRotationSpeed;
	maxEnergy = nEnergy;

	target->integrate();

	

	//Emitter updates
	emitter->setMultiplier(nAgents);
	emitter->setRate(rate);
	emitter->setLifespan(life * 1000);    // convert to milliseconds 
	emitter->setVelocity(ofVec3f(enemyVelocity->x, enemyVelocity->y, enemyVelocity->z));

	if (showImage != spriteImage) {
		showImage = spriteImage;
		emitter->setShowImage(showImage);
		target->setShowImage(showImage);
		
	}
	if (showEnemyVerts != enemyVerts) {
		showEnemyVerts = enemyVerts;
		emitter->setShowEnemyVerts(enemyVerts);
		target->bShowVerts = showEnemyVerts;
	}
	emitter->update();


	for (int i = 0; i < emitter->sys->sprites.size(); i++) {

		// Get values from sliders and update sprites dynamically
		//
		float sc = enemyScale;
		float rs = enemyRotationSpeed;
		emitter->sys->sprites[i]->scale = glm::vec3(sc, sc, sc);
		emitter->sys->sprites[i]->rotationSpeed = rs;
		
	}

	// update the pointer list to remove dead targets
	updatePointerList();
	

	//Movement logic with keymaps
	updateMovement();

	//emitter updates
	turbForce = new TurbulenceForce(ofVec3f(turbMin->x, turbMin->y, turbMin->z), ofVec3f(turbMax->x, turbMax->y, turbMax->z));
	radialForce = new ImpulseRadialForce(radialForceVal);
	collisionEmitter.update();
	leftGunEmitter.update();
	rightGunEmitter.update();

	//handle collisions
	handleCollisions();
}

//Since computation speed becomes an issue - we prioritize target collisions first
void ofApp::handleCollisions() {
	handleTargetCollisions();
	handleEnemyCollisions();

	// bullets are handleded in the Particle.cpp
	//handleBulletCollisions();
}
void ofApp::handleTargetCollisions() {
	vector<glm::vec3> verts = target->getVerts();
	for (size_t i = 0; i < sprites.size(); ++i) {
		if (isColliding(target, sprites[i])){	
			resolveCollision(target, sprites[i]);

			float currentTime = ofGetElapsedTimef();
			float protectionTime = 0.3f;
			if (currentDifficulty == IMPOSSIBLE) protectionTime = 0.8f;
			if (currentTime - lastCollisionTime >= protectionTime) {
				energy -= 1;
			}
			
		}
	}
}
void ofApp::handleEnemyCollisions() {
	for (size_t i = 0; i < sprites.size(); ++i) {
		for (size_t j = i + 1; j < sprites.size(); ++j) {
			// Check for collision between sprites[i] and sprites[j]
			if (isColliding(sprites[i], sprites[j])) {
				// If there's a collision, store the pair of indices
				resolveCollision(sprites[i], sprites[j]);
			}
		}
	}
}


void ofApp::resolveCollision(Sprite* sprite1, Sprite* sprite2) {
	collision.play();
	glm::vec3 collisionNormal = glm::normalize(sprite1->pos - sprite2->pos);
	float e = 1; // Coefficient of restitution.

	// Calculate relative velocity
	glm::vec3 relativeVelocity = sprite1->velocity - sprite2->velocity;
	float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

	// Calculate impulse scalar
	float impulseScalar = -(1 + e) * velocityAlongNormal / (1 / sprite1->mass + 1 / sprite2->mass);

	// Calculate impulse vector
	glm::vec3 impulse = impulseScalar * collisionNormal;

	// Apply impulse to each sprite
	sprite1->velocity += impulse / sprite1->mass;
	sprite2->velocity -= impulse / sprite2->mass;

	// Positional correction
	float percent = 0.2;
	float slop = 0.01;
	float overlap = glm::distance(sprite1->pos, sprite2->pos);
	glm::vec3 correction = max(overlap - slop, 0.0f) / (1 / sprite1->mass + 1 / sprite2->mass) * percent * collisionNormal;

	sprite1->pos += correction / sprite1->mass;
	sprite2->pos -= correction / sprite2->mass;
}

bool ofApp::isColliding(Sprite* sprite1, Sprite* sprite2) {
	std::vector<glm::vec3> verts1 = sprite1->getVerts();
	std::vector<glm::vec3> verts2 = sprite2->getVerts();
	
	glm::mat4 transform1 = sprite1->getTransform();
	glm::mat4 transform2 = sprite2->getTransform();

	for (const auto& vert : verts1) {
		glm::vec4 worldVert = transform1 * glm::vec4(vert, 1.0f);
		if (sprite2->inside(glm::vec3(worldVert))) {
			makeExplosion(glm::vec3(worldVert));
			return true;
		}
	}

	for (const auto& vert : verts2) {
		glm::vec4 worldVert = transform2 * glm::vec4(vert, 1.0f);
		if (sprite1->inside(glm::vec3(worldVert))) {
			makeExplosion(glm::vec3(worldVert));
			return true;
		}
	}

	return false;
}

void ofApp::draw() {
	cout << "\n Size = " << sprites.size();
	if (!bHide) gui.draw();
	switch (currentState) {
	case INITIAL:
		drawInitialScreen();
		break;
	case GAME:
		drawGame();
		break;
	case POST_GAME:
		drawPostGameScreen();
		break;
	}
}
void Sprite::integrate() {
	float dt = 1.0 / ofGetFrameRate();

	glm::vec3 nextPos = pos + velocity * dt;

	if (willMoveOutsideScreen(nextPos)) {
		// Determine the edge and reflect the velocity
		if (nextPos.x < 0 || nextPos.x > ofGetWidth()) { // Left or right edge
			velocity.x = -velocity.x;
		}
		if (nextPos.y < 0 || nextPos.y > ofGetHeight()) { // Top or bottom edge
			velocity.y = -velocity.y;
		}

		// Adjust position slightly off the wall to prevent sticking
		pos += velocity * dt;

		// Apply an additional force towards the center
		velocity += centralForceDirection() * 50;
	}
	else {
		// Normal motion
		pos = nextPos;
	}
	
	velocity += acceleration * dt;
	velocity *= damping;

	rot += angularVelocity * dt;
	angularVelocity += angularAcceleration * dt;
	angularVelocity *= damping; 

	
}

void ofApp::targetHitSound() {
	targetHit.play();
}
//This force acts like a charge - when enemy looks at the target - he gets additional force.
glm::vec3 ofApp::targetForce(glm::vec3 pos) {
	float dot = glm::dot(glm::normalize(pos), glm::normalize(target->pos));
	//dot = glm::clamp(dot, -1.0f, 1.0f);
	
	glm::vec3 direction = glm::normalize(target->pos - pos);
	//cout << "\n pos = "<<pos << "; target->pos = " << target->pos <<"; dot = " << dot << "; direction = " << direction;

	return direction*dot*(float)targetForceCoefficient*10;
}
void ofApp::drawInitialScreen() {
	ofSetColor(255, 255, 255);
	difficultyOptions.clear();
	int guiDifficulty = static_cast<int>(currentDifficulty);
	
	string title = "Project 1: The Sky Pursuit";
	string instructions = "Use arrow keys or WASD to move. Avoid the enemies!";
	string prompt = "Press SPACE to start";
	string instructions2 = "Use mouse or arrows to select the difficulty level.";

	titleFont.drawString(title, ofGetWidth() / 2 - titleFont.stringWidth(title)/2, ofGetHeight() / 2 - 40);
	ofDrawBitmapString(instructions, ofGetWidth() / 2 - instructions.length() * 4, ofGetHeight() / 2);
	ofDrawBitmapString(prompt, ofGetWidth() / 2 - prompt.length() * 4, ofGetHeight() / 2 + 20);
	ofDrawBitmapString(instructions2, ofGetWidth() / 2 - instructions2.length() * 4, ofGetHeight() / 2 + 80);

	difficultyOptions.push_back({ "  Easy", ofRectangle(ofGetWidth() / 2 - 180, ofGetHeight() / 2 + 100, 90, 50) });
	difficultyOptions.push_back({ "Difficult", ofRectangle(ofGetWidth() / 2 - 55, ofGetHeight() / 2 + 100, 100, 50) });
	difficultyOptions.push_back({ " Impossible", ofRectangle(ofGetWidth() / 2 + 80, ofGetHeight() / 2 + 100, 115, 50) });

	


	for (const auto& option : difficultyOptions) {
		if (option.name == difficultyOptions[currentDifficulty].name) {
			ofSetColor(255, 255, 255);
			ofDrawRectangle(option.bounds);
			ofSetColor(0, 0, 0);
		}
		else {
			ofSetColor(255, 255, 255);
		}
		ofDrawBitmapString(option.name, option.bounds.x + 10, option.bounds.y + 30);
	}
	
}

void ofApp::drawPostGameScreen() {
	ofSetColor(255, 0, 0);  // Set color to red
	difficultyOptions.clear();
	post_game_background.draw(0, 0, ofGetWidth(), ofGetHeight());
	
	string elapsedTime = "Time: " + playedTime + " seconds";
	string gameOver = "GAME OVER";
	string prompt = "Press SPACE to restart";

	titleFont.drawString(gameOver, ofGetWidth() / 2 - titleFont.stringWidth(gameOver) / 2, ofGetHeight() / 2 - 40);
	ofDrawBitmapString(elapsedTime, ofGetWidth() / 2 - elapsedTime.length() * 4, ofGetHeight() / 2);
	ofDrawBitmapString(prompt, ofGetWidth() / 2 - prompt.length() * 4, ofGetHeight() / 2 + 20);


	difficultyOptions.push_back({ "  Easy", ofRectangle(ofGetWidth() / 2 - 180, ofGetHeight() / 2 + 100, 90, 50) });
	difficultyOptions.push_back({ "Difficult", ofRectangle(ofGetWidth() / 2 - 55, ofGetHeight() / 2 + 100, 100, 50) });
	difficultyOptions.push_back({ " Impossible", ofRectangle(ofGetWidth() / 2 + 80, ofGetHeight() / 2 + 100, 115, 50) });




	for (const auto& option : difficultyOptions) {
		if (option.name == difficultyOptions[currentDifficulty].name) {
			ofSetColor(255, 0, 0);
			ofDrawRectangle(option.bounds);
			ofSetColor(0, 0, 0);
		}
		else {
			ofSetColor(255, 0, 0);
		}
		ofDrawBitmapString(option.name, option.bounds.x + 10, option.bounds.y + 30);
	}
}


//--------------------------------------------------------------
void ofApp::drawGame(){
	ofSetColor(255, 255, 255);
	backgroundImg.draw(0, 0, ofGetWidth(), ofGetHeight());
	emitter->draw();
	target->draw();
	collisionEmitter.draw();
	leftGunEmitter.draw();
	rightGunEmitter.draw();
	//

	// Draw energy, elapsed time, and framerate
	std::string energyText = "Energy: " + std::to_string((int)energy) + "/" + std::to_string((int)maxEnergy);
	std::string elapsedTimeText = "Elapsed Time: " + std::to_string((int)ofGetElapsedTimef()) + " seconds";
	std::string framerateText = "Framerate: " + std::to_string(ofGetFrameRate()) + " FPS";

	ofDrawBitmapString(energyText, ofGetWidth() - 200, 20);
	ofDrawBitmapString(elapsedTimeText, ofGetWidth() - 200, 40);
	ofDrawBitmapString(framerateText, ofGetWidth() - 200, 60);

	if (!bHide) {
		gui.draw();
	}


}

bool ofApp::intersectsWithTarget(Sprite* sprite) {
	vector<glm::vec3> points = sprite->getVerts();
	


	for (int i = 0; i < points.size(); i++) {
		//cout << "Triangle Point " << i + 1 << ": (" << points[i].x << ", " << points[i].y << ", " << points[i].z << ")";
		if (target->inside(points[i])) {
			return true;  // Found a point inside the sprite
		}
	}

	return false;  // No points were inside the sprite
}


//--------------------------------------------------------------
void ofApp::updateMovement() {
	bool engineWorks = false;

	target->acceleration = glm::vec3(0, 0, 0);
	target->angularAcceleration = 0;
	if (currentState == GAME and keymap[' ']) {
		updateGunPosition();

		leftGunEmitter.start();
		rightGunEmitter.start();

		shooting.play();
	}
	if (keymap[OF_KEY_LEFT] or keymap['A'] or keymap['a']) {
		turnLeft();
		engineWorks = true;
	}
	if (keymap[OF_KEY_RIGHT] or keymap['D'] or keymap['d']) {
		turnRight();
		engineWorks = true;
	}
	if (keymap[OF_KEY_UP] or keymap['W'] or keymap['w']) {
		moveForward();
		engineWorks = true;
	}
	if (keymap[OF_KEY_DOWN] or keymap['S'] or keymap['s']) {
		moveBackward();
		engineWorks = true;
	}
	
	target->bThrust = engineWorks;
}

// Movement logic
void ofApp::moveForward() {
	target->bThrust = true;
	
	glm::vec3 heading = glm::normalize(glm::rotate(glm::vec3(0, -1, 0), glm::radians(target->rot), glm::vec3(0, 0, 1)));
	target->acceleration = heading * target->thrust;
	
}

void ofApp::moveBackward() {
	target->bThrust = true;
	glm::vec3 heading = glm::normalize(glm::rotate(glm::vec3(0, -1, 0), glm::radians(target->rot), glm::vec3(0, 0, 1)));
	
	target->acceleration = -heading * target->thrust;
}

void ofApp::turnRight() {
	target->bThrust = true;
	target->angularAcceleration += 0.2*(float)target->thrust;
}

void ofApp::turnLeft() {
	target->bThrust = true;
	target->angularAcceleration -= 0.2*(float)target->thrust;
}

bool Sprite::willMoveOutsideScreen(const glm::vec3& point) {
	return point.x < 0 || point.x > ofGetWidth() || point.y < 0 || point.y > ofGetHeight();
}

glm::vec3 Sprite::centralForceDirection() {
	glm::vec3 screenCenter(ofGetWidth() / 2, ofGetHeight() / 2, 0);
	glm::vec3 direction = screenCenter - pos;
	direction = glm::normalize(direction);
	return direction;
}

void ofApp::updatePointerList() {
	vector<Sprite*> aliveSprites;
	float now = ofGetElapsedTimeMillis();
	// Check each sprite to see if it is dead
	for (Sprite* sprite : sprites) {
		if (now - sprite->birthtime > sprite->lifespan) {
			// If the sprite is dead, delete the sprite and set the pointer to nullptr
			delete sprite;
			sprite = nullptr;
		}
		else {
			// If the sprite is alive, add it to the temporary vector
			aliveSprites.push_back(sprite);
		}
	}

	// Replace the old list with the new list of alive sprites
	sprites = aliveSprites;
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
//	cout << "mouse( " << x << "," << y << ")" << endl;


}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	if (bDrag) {
		glm::vec3 p = glm::vec3(x, y, 0);
		glm::vec3 delta = p - lastMousePos;
		
		target->pos += delta;
		
	
		lastMousePos = p;
	}

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
	if (currentState == GAME) {

		glm::vec3 pos = glm::vec3(x, y, 0);
		if (target->inside(pos)) {
			bDrag = true;
			lastMousePos = pos;
		}
	}
	else {
		for (int i = 0; i < difficultyOptions.size(); i++) {
			if (difficultyOptions[i].bounds.inside(x, y)) {
				currentDifficulty = static_cast<Difficulty>(i);
				break;
			}
		}
	}
		
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	bDrag = false;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

void ofApp::keyPressed(int key) {
	if (currentState != GAME) {
		if (key == OF_KEY_RIGHT) {
			currentDifficulty = static_cast<Difficulty>((currentDifficulty + 1) % 3);
		}
		else if (key == OF_KEY_LEFT) {
			currentDifficulty = static_cast<Difficulty>((currentDifficulty - 1 + 3) % 3);
		}
	}
	switch (key) {
	case ' ':
		if (currentState != GAME) {
			startGame();
		}
		break;
	
		
	case 'c':
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		bHide = !bHide;
		break;
	case 'r':
		break;
	case 'u':
		break;
	case OF_KEY_ALT:
		break;
	case OF_KEY_CONTROL:
		break;
	case OF_KEY_SHIFT:
		break;
	case OF_KEY_DEL:
		break;
	}
	if (currentState != GAME) return;
	keymap[key] = true;
}
void ofApp::updateGunPosition() {
	
	glm::mat4 shipTransform = target->getTransform();


	// Apply the ship's transform to the gun offsets to get the world position
	glm::vec3 leftGunWorldPos = glm::vec3(shipTransform * glm::vec4(-20, 30, 0, 1.0));
	glm::vec3 rightGunWorldPos = glm::vec3(shipTransform * glm::vec4(20, 30, 0, 1.0));
	if (target->bShowImage) {
		leftGunWorldPos = glm::vec3(shipTransform * glm::vec4(-300, -300, 0, 1.0));
		rightGunWorldPos = glm::vec3(shipTransform * glm::vec4(330, -300, 0, 1.0));
	}
	
		
	
	

	// Set the gun emitters' positions
	leftGunEmitter.setPosition(leftGunWorldPos);
	rightGunEmitter.setPosition(rightGunWorldPos);

	//update bullet direction
	glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(target->rot), glm::vec3(0, 0, 1));
	glm::vec3 originalVelocity(0, -1000, 0);

	glm::vec3 rotatedVelocity = rotationMatrix * originalVelocity;
	leftGunEmitter.setVelocity(rotatedVelocity);
	rightGunEmitter.setVelocity(rotatedVelocity);
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	keymap[key] = false;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}



//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

