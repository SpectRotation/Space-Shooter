#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "Emitter.h"
#include "Shape.h"
#include "Sprite.h"
#include "Particle.h"
#include "ParticleEmitter.h"




class Agent : public Sprite {
public:
	Agent() {
		Sprite::Sprite();
//		cout << "in Agent Constuctor" << endl;
	}
};

class AgentEmitter : public Emitter {
public:
	void spawnSprite();
	void moveSprite(Sprite*);
};

class ofApp : public ofBaseApp{

	public:
		void makeExplosion(ofVec3f position);
		void startGame();
		void restartGame();
		void setup();
		void setupGame();
		void update();
		void handleCollisions();
		void handleTargetCollisions();
		void handleEnemyCollisions();

		void resolveCollision(Sprite * sprite1, Sprite * sprite2);
		bool isColliding(Sprite * sprite1, Sprite * sprite2);
		void draw();
		void setupGUI();
		void drawGame();
		void targetHitSound();
		glm::vec3 targetForce(glm::vec3 pos);
		void drawInitialScreen();
		void drawPostGameScreen();

		bool intersectsWithTarget(Sprite * sprite);

		void updateMovement();
		void moveForward();
		void moveBackward();
		void turnRight();
		void turnLeft();




		void keyPressed(int key);
		void updateGunPosition();
		void keyReleased(int key);
		void updatePointerList();
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		

		enum GameState {
			INITIAL,
			GAME,
			POST_GAME
		};


		enum Difficulty {
			EASY = 0,
			DIFFICULT = 1,
			IMPOSSIBLE = 2
		};

		

		struct DifficultyOption {
			std::string name;
			ofRectangle bounds;
		};

		Difficulty currentDifficulty;
		std::vector<DifficultyOption> difficultyOptions;

		GameState currentState = INITIAL;
		Emitter  *emitter = NULL;
		Sprite* target = NULL;
		float lastCollisionTime = 0.0f;
		

		ofImage backgroundImg;
		ofImage post_game_background;
		ofImage defaultImage;
		ofImage targetImage;
		ofVec3f mouse_last;
		bool imageLoaded;
		bool showImage;
		bool showEnemyVerts = false;
		bool bDrag = false;
		glm::vec3 lastMousePos;
		// Some basic UI
		//
		bool bHide;
		ofxFloatSlider rate;
		ofxFloatSlider life;
		ofxVec3Slider enemyVelocity;
		ofxLabel screenSize;
		ofxFloatSlider enemyScale;
		ofxFloatSlider enemyRotationSpeed;
		ofxIntSlider difficulty;
		int guiDifficulty = 1;

		ofxFloatSlider targetScale;
		ofxFloatSlider targetRotationSpeed;
		ofxFloatSlider targetThrust;
		ofxFloatSlider targetForceCoefficient;
		ofxIntSlider nAgents;

		ofxToggle spriteImage;
		ofxToggle enemyVerts;
		
		ofTrueTypeFont titleFont;


		float energy = 0;
		ofxIntSlider nEnergy;
		float maxEnergy = 20;
		

		string playedTime;
		ofxPanel gui;
		map<int, bool> keymap;

		//
		// Particle emitters
		//
		ParticleEmitter leftGunEmitter;
		ParticleEmitter rightGunEmitter;
		ParticleEmitter collisionEmitter;

		ofxVec3Slider turbMin;
		ofxVec3Slider turbMax;
		ofxFloatSlider radialForceVal;

		TurbulenceForce *turbForce;
		GravityForce *gravityForce;
		ImpulseRadialForce *radialForce;

		vector<Sprite*> sprites;

		ofSoundPlayer engineSound;
		ofSoundPlayer collision;
		ofSoundPlayer shooting;
		ofSoundPlayer targetHit;
};
