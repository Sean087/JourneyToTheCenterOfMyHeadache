#include <SDL.h>
#include <SDL_image.h>
#include "Ship.h"
#include "Game.h"
#include "EnemyShip.h"
#include "LTexture.h"
#include <cmath>

/*
2017-01-15:
Added game controller support
2017-01-04:
Added asdw keyboard movement
*/

#define DIAGONAL_VEL SHIP_VEL / (sqrt(2))

//Analog joystick dead zone
const int JOYSTICK_DEAD_ZONE = 8000;	// Create a deadzone where input from the controller is ignored

Game game1;
Ship::Ship() {
	// Initialize the offsets
	mPosX = 0;
	mPosY = 200;

	// Initialize the velocity
	mVelX = 0;
	mVelY = 0;

	// Set Collision Box Dimension
	mCollider.w = SHIP_WIDTH;
	mCollider.h = SHIP_HEIGHT;
}

int previous = 0;
int previousStick = 0;

void Ship::handleEvent(SDL_Event& e) {
	// If a key was pressed
	int storeX = 0, storeY = 0;
	//Normalized direction
	int xDir = 0;		// keep track of the x direction, x equals ‐1, the joystick's x position is pointing left
	int yDir = 0;		// keep track of the y direction, positive up, negative down

	if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
		// MOVEMENT
		// Adjust the velocity
		switch (e.key.keysym.sym) {
		case SDLK_UP:
		case SDLK_w: mVelY -= SHIP_VEL; break;
		case SDLK_DOWN:
		case SDLK_s: mVelY += SHIP_VEL; break;
		case SDLK_LEFT:
		case SDLK_a: mVelX -= SHIP_VEL; break;
		case SDLK_RIGHT:
		case SDLK_d: mVelX += SHIP_VEL; break;

		// FIRE WEAPON
		case SDLK_SPACE:
			game1.spawnLaser();
			break; // SEAN: Press space bar to spawn a new laser
		}
	}
	else if (e.type == SDL_JOYBUTTONDOWN) {
		if (e.jbutton.button == 0) {
			game1.spawnLaser();
			std::cout << (int)e.jbutton.button << std::endl;	// shows which button has been pressed
		}
	}
	// If a key was released
	else if (e.type == SDL_KEYUP && e.key.repeat == 0) {
		// Adjust the velocity
		switch (e.key.keysym.sym) {
		case SDLK_UP:
		case SDLK_w: mVelY += SHIP_VEL; break;
		case SDLK_DOWN:
		case SDLK_s: mVelY -= SHIP_VEL; break;
		case SDLK_LEFT:
		case SDLK_a: mVelX += SHIP_VEL; break;
		case SDLK_RIGHT:
		case SDLK_d: mVelX -= SHIP_VEL; break;
		}
	}
	// Set movement for D-Pad
	else if (e.type == SDL_JOYHATMOTION) {
		if (e.jhat.value == SDL_HAT_UP) {
			resetPreviousDirection();
			mVelY -= SHIP_VEL;
			previous = SDL_HAT_UP;
		}
		else if (e.jhat.value == SDL_HAT_DOWN) {
			resetPreviousDirection();
			mVelY += SHIP_VEL;
			previous = SDL_HAT_DOWN;
		}
		else if (e.jhat.value == SDL_HAT_LEFT) {
			resetPreviousDirection();
			mVelX -= SHIP_VEL;
			previous = SDL_HAT_LEFT;
		}
		else if (e.jhat.value == SDL_HAT_RIGHT) {
			resetPreviousDirection();
			mVelX += SHIP_VEL;
			previous = SDL_HAT_RIGHT;
		}
		else if (e.jhat.value == SDL_HAT_RIGHTUP) {
			resetPreviousDirection();
			mVelX += DIAGONAL_VEL;
			mVelY -= DIAGONAL_VEL;
			previous = SDL_HAT_RIGHTUP;
		}
		else if (e.jhat.value == SDL_HAT_RIGHTDOWN) {
			resetPreviousDirection();
			mVelX += DIAGONAL_VEL;
			mVelY += DIAGONAL_VEL;
			previous = SDL_HAT_RIGHTDOWN;
		}
		else if (e.jhat.value == SDL_HAT_LEFTUP) {
			resetPreviousDirection();
			mVelX -= DIAGONAL_VEL;
			mVelY -= DIAGONAL_VEL;
			previous = SDL_HAT_LEFTUP;
		}
		else if (e.jhat.value == SDL_HAT_LEFTDOWN) {
			resetPreviousDirection();
			mVelX -= DIAGONAL_VEL;
			mVelY += DIAGONAL_VEL;
			previous = SDL_HAT_LEFTDOWN;
		}

		if (e.jhat.value == SDL_HAT_CENTERED) {
			resetPreviousDirection();
			previous = SDL_HAT_CENTERED;
		}
	}
	else if (e.type == SDL_JOYAXISMOTION)	// Check if the joystick has moved
	{
		if (e.jaxis.which == 1)			// if (e.jaxis.which == 0) - which controller the axis motion came from
		{
			// Check if it was a motion in the x direction or y direction
			//X axis motion
			if (e.jaxis.axis == 0) {			// typically, axis 0 is the x axis
				// LEFT
				if (e.jaxis.axis == 0 && e.jaxis.value < -JOYSTICK_DEAD_ZONE) {	// Value: position the analog stick has on the axis
					resetPreviousStickDirection();
					printf("\nleft ");
					mVelX -= SHIP_VEL;
					previousStick = 1;
				}
				// RIGHT
				else if (e.jaxis.axis == 0 && e.jaxis.value > JOYSTICK_DEAD_ZONE) {
					resetPreviousStickDirection();
					printf("\nright ");
					mVelX += SHIP_VEL;		// test
					previousStick = 2;
				}
				else if (e.jaxis.value < JOYSTICK_DEAD_ZONE && e.jaxis.value > -JOYSTICK_DEAD_ZONE) {
					//if (e.jaxis.axis == 1 && e.jaxis.value < JOYSTICK_DEAD_ZONE && e.jaxis.value > -JOYSTICK_DEAD_ZONE)
					if(!(e.jaxis.axis == 1 && e.jaxis.value < JOYSTICK_DEAD_ZONE && e.jaxis.value > -JOYSTICK_DEAD_ZONE)) // If x axes not in dead zone
					resetPreviousStickDirection();
					//if (e.jaxis.axis == 1) resetPreviousStickDirection();
				}
			}
			/*
				// BELOW
				else if (e.jaxis.axis == 1 && e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
					resetPreviousStickDirection();
					printf("\nup");
					mVelY -= SHIP_VEL;
					previousStick = 3;
				}
				// ABOVE
				else if (e.jaxis.axis == 1 && e.jaxis.value > JOYSTICK_DEAD_ZONE) {
					resetPreviousStickDirection();
					printf("\ndown");
					mVelY += SHIP_VEL;
					previousStick = 4;
				}
				//else if (e.jaxis.axis == 1 && e.jaxis.value < JOYSTICK_DEAD_ZONE && e.jaxis.value > -JOYSTICK_DEAD_ZONE) {
				//	resetPreviousStickDirection();
				//}


				if (e.jaxis.value > -JOYSTICK_DEAD_ZONE && e.jaxis.value < JOYSTICK_DEAD_ZONE) {
					resetPreviousStickDirection();
				}

					*/
			//Y axis motion
			//else
		/*	if (e.jaxis.axis == 1){	// axis id 1 for y axis
				// BELOW of dead zone
				if (e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
					resetPreviousStickDirection();
					printf("\nup");
					mVelY -= SHIP_VEL;
					previousStick = 3;
				}
				// ABOVE of dead zone
				else if (e.jaxis.value > JOYSTICK_DEAD_ZONE) {
					//yDir = 1;
					resetPreviousStickDirection();
					printf("\ndown");
					mVelY += SHIP_VEL;
					previousStick = 4;
				}

				else if (e.jaxis.value > -JOYSTICK_DEAD_ZONE && e.jaxis.value < JOYSTICK_DEAD_ZONE) {
					if (!(e.jaxis.axis == 0 && e.jaxis.value < JOYSTICK_DEAD_ZONE && e.jaxis.value > -JOYSTICK_DEAD_ZONE))
					resetPreviousStickDirection();
				}
			}

/*
			//else
				if (e.jaxis.axis == 2)	// axis id 1 for y axis
			{
				printf("left trigger");
			}
			else if (e.jaxis.axis == 3)	// axis id 1 for y axis
			{
				printf("right stick y");
			}
			else if (e.jaxis.axis == 4)	// axis id 1 for y axis
			{
				printf("right stick x");
			}
			else if (e.jaxis.axis == 5)	// axis id 1 for y axis
			{
				printf("right trigger");
			}
	*/
		}
	}

	//Calculate angle
	// Before we render the arrow which will point in the direction the analog stick is pushed, we need to calculate the angle
	// Gives angle in radians, so convert to degrees
	//double joystickAngle = atan2((double)yDir, (double)xDir) * (180.0 / M_PI);	// atan2: arc tangent 2 (inverse tangenet 2)

																				//Correct angle
	//if (xDir == 0 && yDir == 0)	// if both the x and y position are 0, we could get a garbage angle, so we correct the angle to equal 0.
	//{
	//	joystickAngle = 0;
	//}
}
void Ship::resetPreviousStickDirection() {
	//std::cout << "\nstick position: " << previousStick << std::endl;
	//if (previousStick != 0)
	//{
		if (previousStick == 1) {
			mVelX += SHIP_VEL;
		}
		else if (previousStick == 2) {
			mVelX -= SHIP_VEL;
		}

		if (previousStick == 3) {
			mVelY += SHIP_VEL;
		}
		else if (previousStick == 4) {
			mVelY -= SHIP_VEL;
		}

	//}
	previousStick = 0;
}

void Ship::resetPreviousDirection() {
	if (previous == SDL_HAT_UP) {
		mVelY += SHIP_VEL;
	}
	else if (previous == SDL_HAT_DOWN) {
		mVelY -= SHIP_VEL;
	}
	else if (previous == SDL_HAT_LEFT) {
		mVelX += SHIP_VEL;
	}
	else if (previous == SDL_HAT_RIGHT) {
		mVelX -= SHIP_VEL;
	}

	else if (previous == SDL_HAT_RIGHTUP) {
		mVelX -= DIAGONAL_VEL;
		mVelY += DIAGONAL_VEL;
	}
	else if (previous == SDL_HAT_RIGHTDOWN) {
		mVelX -= DIAGONAL_VEL;
		mVelY -= DIAGONAL_VEL;
	}
	else if (previous == SDL_HAT_LEFTDOWN) {
		mVelX += DIAGONAL_VEL;
		mVelY -= DIAGONAL_VEL;
	}
	else if (previous == SDL_HAT_LEFTUP) {
		mVelX += DIAGONAL_VEL;
		mVelY += DIAGONAL_VEL;
		//	std::cout << " previous " << previous << std::endl;
	}
}

void Ship::move() {
	mPosX += mVelX;												// Move the ship left or right
	mCollider.x = mPosX;

	// If the ship went too far to the left or right
	if ((mPosX < 0) || (mPosX + SHIP_WIDTH > SCREEN_WIDTH)) {
		mPosX -= mVelX;											// Move back
		mCollider.x = mPosX;
	}

	mPosY += mVelY;												// Move the ship up or down
	mCollider.y = mPosY;

	// If the ship went too far up or down
	if ((mPosY < 32) || (mPosY + SHIP_HEIGHT > SCREEN_HEIGHT - 32)) {	// Changed to 40 to stay in boundaries
		mPosY -= mVelY;
		mCollider.y = mPosY;// Move back
	}
}

// SEAN: Added get x and y function for ship to allow laser to spawn at ships location
int Ship::getShipX(){
	return mPosX;
}// end getX

int Ship::getShipY(){
	return mPosY;
}// end getX

int Ship::getShipVelX(){
	return mVelX;
}// end getX

int Ship::getShipVelY(){
	return mVelY;
}// end getX

SDL_Rect Ship::getCollider(){
	return mCollider;
}

void Ship::setShipX(int x){
	mPosX = x;
}// end setX

 // set Y
void Ship::setShipY(int y){
	mPosY = y;
}// end setY

void Ship::setShipColX(int x){
	mCollider.x = x;
}// end setX

 // set Y
void Ship::setShipColY(int y){
	mCollider.y = y;
}// end setY
