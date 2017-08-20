
/*
    Gamepad Supported

    2017-08-11:
        Joe: Change window title
        Joe: Add relative path for asset files in "Art" directory
        Joe: Changed getCollision() for ship and enemyship
        Joe: Add relative path for asset files in "Music" and "SoundFX" directories
        Joe: Tested and working with Xbox One wireless controller
*/

#include <SDL.h>
#include <SDL_image.h>

#include <SDL_ttf.h>	// Fonts
#include <stdio.h>
#include <string>
#include <sstream>		// Using string streams

#include "Game.h"
#include "LTexture.h"
#include "Ship.h"
#include "EnemyShip.h"
#include "Laser.h"
#include <list>

int playerAlpha = 255;					// Modulation component for flashing objects
int gameOverAlpha = 255;
int timerAlpha = 255;
bool playerFlash = false;
bool gameOverFlash = true;
bool timerFlash = false;
bool gameOver = false;

//bool init();					// Starts up SDL and creates window
bool loadMedia();				// Loads media//void close();

SDL_Window* gWindow = NULL;		// The window we'll be rendering to
SDL_Renderer* gRenderer = NULL;	// The window renderer

SDL_Event e;										// Event handler

SDL_Joystick* gGameController = NULL;	// Game Controller 1 handler - Data type for a game controller is SDL_Joystick

TTF_Font *gFont = NULL;			// Globally used font

//Scene textures
LTexture gTimeTextTexture;
LTexture gPromptTextTexture;
LTexture gLevelTextTexture;

#define TIMER 15				// Time to start counting down from in seconds
Uint32 startTime = 6000;			// Unsigned integer 32-bits

//Scene textures
LTexture gBGTexture;
LTexture gBGStartTexture;
LTexture gBGEndTexture;
LTexture gShipTexture;
LTexture gEnemyShipTexture;
LTexture gLaserTexture; // SEAN: Created Texture for Laser
LTexture gGameOverTextTexture;

// SEAN: Move ship object outside of main so spawnLaser funtion can use it
Ship ship;									// Declare a ship object that will be moving around on the screen
EnemyShip enemy;


int scrollingOffset = 0;					// Declare the background scrolling offset

bool checkCollision(SDL_Rect a, SDL_Rect b);

// SEAN : Created list and iterator for laser objects
std::list<Laser*> listOfLaserObjects;		// List to store laser objects
std::list<Laser*>::const_iterator iter;		// Create global iterators to cycle through laser objects - Make them read only

/*gRenderer*/
bool LTexture::loadFromFile(std::string path) {
	free();													// Get rid of preexisting texture

	SDL_Texture* newTexture = NULL;							// The final texture

	SDL_Surface* loadedSurface = IMG_Load(path.c_str());	// Load image at specified path
	if (loadedSurface == NULL) {
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	} else {
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));	// Color key image

		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);						// Create texture from surface pixels
		if (newTexture == NULL) {
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		} else {
			// Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		SDL_FreeSurface(loadedSurface);	// Get rid of old loaded surface
	}

	// Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

#ifdef _SDL_TTF_H
bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
	free();	//Get rid of preexisting texture

	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);	//Render text surface

	if (textSurface != NULL) {
		//Create texture from surface pixels
		mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
		if (mTexture == NULL) {
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else {
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		SDL_FreeSurface(textSurface);	//Get rid of old surface
	}
	else {
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}

	return mTexture != NULL;	// Return success
}
#endif

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };	// Set rendering space and render to screen

	// Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);	// Render to screen
}

bool Game::init() {
	enemy.spawn();

	bool success = true;					// Initialization flag

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Check for joysticks
		if (SDL_NumJoysticks() < 1)							// check if there is at least one joystick connected.
		{
			printf("Warning: No joysticks connected!\n");
		}
		else {
			//Load joystick
			gGameController = SDL_JoystickOpen(0);			// open the joystick at index 0
			printf("Joystick connected\n");						// DETECTS JOYSTICK
			if (gGameController == NULL)
			{
				printf("Warning: Unable to open game controller! SDL Error: %s\n", SDL_GetError());
			}
		}

		gWindow = SDL_CreateWindow("JOURNEY TO THE CENTER OF MY HEADACHE v1.14 by Joe O'Regan & Se\u00E1n Horgan - Gamepad Support", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);	/* Create Window with name */
		if (gWindow == NULL) {
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		} else {
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);	// Create vsynced renderer for window
			if (gRenderer == NULL) {
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			} else {
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);										// Initialize renderer color

				int imgFlags = IMG_INIT_PNG;																	// Initialize PNG loading
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				//Initialize SDL_ttf
				if (TTF_Init() == -1) {
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia() {
	bool success = true;			// Loading success flag

	//Open the font
	//gFont = TTF_OpenFont("22_timing/lazy.ttf", 28);
	gFont = TTF_OpenFont(".\\Font\\lazy.ttf", 28);
	if (gFont == NULL) {
		printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}
	else {
		//Set text color as black
		//SDL_Color textColor = { 100, 225, 225, 255 }; // too bright
		SDL_Color textColor = { 0, 100, 200, 255 };
		TTF_SetFontStyle(gFont, TTF_STYLE_BOLD);

		//Load prompt texture
		//if (!gPromptTextTexture.loadFromRenderedText("Press Enter to Reset Start Time.", textColor)) {
		if (!gPromptTextTexture.loadFromRenderedText("A Game By Sean Horgan And Joe O'Regan", textColor)) {
			printf("Unable to render prompt text texture!\n");
			success = false;
		}
		if (!gLevelTextTexture.loadFromRenderedText("Level 1", textColor)) {
			printf("Unable to render level text texture!\n");
			success = false;
		}
	}

	if (!gShipTexture.loadFromFile(".\\Art\\Player1Ship.png")) {	// Load Ship texture
		printf("Failed to load Player texture!\n");
		success = false;
	}

	//if (!gEnemyShipTexture.loadFromFile("Enemy.png")) {		// Load Enemy Ship texture
	if (!gEnemyShipTexture.loadFromFile(".\\Art\\EnemyVirus.png")) {		// Load Enemy Ship texture
		printf("Failed to load Enemy texture!\n");
		success = false;
	}

	if (!gBGTexture.loadFromFile(".\\Art\\Background800.png")) {	// Load background texture
		printf("Failed to load background texture!\n");
		success = false;
	}
	if (!gBGStartTexture.loadFromFile(".\\Art\\bgBegin.png")) {	// Load background texture
		printf("Failed to load start background texture!\n");
		success = false;
	}
	if (!gBGEndTexture.loadFromFile(".\\Art\\bgEnd.png")) {	// Load background texture
		printf("Failed to load end background texture!\n");
		success = false;
	}

	if (!gLaserTexture.loadFromFile(".\\Art\\LaserBeam.png")) {	// SEAN: Load Laser texture
		printf("Failed to load Laser texture!\n");
		success = false;
	}

	//if (!gGameOverTextTexture.loadFromFile("GameOver.png")) {
	if (!gGameOverTextTexture.loadFromFile(".\\Art\\GameOverTEST.png")) {
		printf("Failed to load Game Over texture!\n");
		success = false;
	}

	return success;
}

void Game::close() {
	// Free loaded images
	gTimeTextTexture.free();
	gPromptTextTexture.free();
	gLevelTextTexture.free();
	gShipTexture.free();
	gEnemyShipTexture.free();
	gBGTexture.free();
	gBGStartTexture.free();
	gBGEndTexture.free();
	gLaserTexture.free();
	gGameOverTextTexture.free();

	//Close game controller
	SDL_JoystickClose(gGameController); // After we're done with the joystick, we close it with SDL_JoystickClose.
	gGameController = NULL;

	// Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	// Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

//int main(int argc, char* args[]) {
void Game::update(){
	if (!init()) {									// Start up SDL and create window
		printf("Failed to initialize!\n");
	}
	else {
		if (!loadMedia()) {							// Load media
			printf("Failed to load media!\n");
		}
		else {
			bool quit = false;						// Main loop flag

			SDL_Color textColor = { 0, 100, 200, 255 };

			if (SDL_PollEvent(&e) != 0) {
				printf("Joystick connected %d\n", e.jaxis.which);						// DETECTS JOYSTICK
																						//printf("Number of buttons %d", SDL_JoystickNumButtons);
																						//std::cout << "Number of buttons: " << SDL_JoystickNumButtons;
				std::cout << "Number of buttons: " << SDL_JoystickNumButtons(gGameController) << std::endl;			// Number of useable buttons
				std::cout << "Number of axes: " << SDL_JoystickNumAxes(gGameController) << std::endl;				// Number of axes on the controller, includes sticks and triggers.
				std::cout << "Number of trackballs: " << SDL_JoystickNumBalls(gGameController) << std::endl;		// No trackballs on NVidia Shield Controller
				std::cout << "Number of hats: " << SDL_JoystickNumHats(gGameController) << std::endl;				// Hats = d-pad on NVidia Shield Controller
				std::cout << "Controller Name: " << SDL_JoystickName(gGameController) << std::endl;					// Name of joystick
			}

			unsigned int lastTime = 0, currentTime, countdownTimer = TIMER;	// TEST TIMING

			//In memory text stream
			// string streams - function like iostreams only instead of reading or writing to the console, they allow you to read and write to a string in memory
			std::stringstream timeText;		// string stream

			while (!quit) {											// While application is running
				//flashGameObject();
				flashGameObject(playerAlpha, playerFlash, 10, 4);	// Flash player ship when it has a collision, flash at faster rate, flash 4 times
				flashGameObject(gameOverAlpha, gameOverFlash, 5);	// Flash game over at end of game, flash at slower rate for 5 than 10
				flashGameObject(timerAlpha, timerFlash, 8);			// Flash timer when time is running out

				quit = playerInput(quit);							// 2017/01/09 JOE: Handle input from player

				timeText.str("");									// Set text to be rendered - string stream - print the time since timer last started - initialise empty

				currentTime = SDL_GetTicks();

				if (currentTime > lastTime + 1000) {
					lastTime = currentTime;

					countdownTimer -= 1;

					//std::cout << "Time: " << countdownTimer << " lastTime: " << lastTime << " currentTime: " << currentTime << std::endl;
				}

				// Countdown Timer
				if (countdownTimer > TIMER && countdownTimer < TIMER + 6) {
					timeText << "Game Over";
					gameOver = true;
				}
				else if (countdownTimer >= 0 && countdownTimer <= TIMER) {
					timeText << "Time: " << countdownTimer;
					gameOver = false;
				}
				if (countdownTimer <= 0 || countdownTimer > TIMER + 6) {
					timeText << "Game Over";
					gameOver = true;
					countdownTimer = TIMER + 5;	// flash game over for 5 seconds
				}

				// Time running out change colour to red
				if (countdownTimer >= 0 && countdownTimer <= 5) {
					textColor = { 255, 0, 0, 255 };
					timerFlash = true;
				}
				else {
					textColor = { 0, 100, 200, 255 };
					timerFlash = false;
				}

				//Render text - Get a string from it and use it to render the current time to a texture
				if (!gTimeTextTexture.loadFromRenderedText(timeText.str().c_str(), textColor)) {
					printf("Unable to render time texture!\n");
				}

				renderGameObjects();

				moveGameObjects();

				destroyGameObjects();				// 2017-01-09 JOE: Destroy the game objects when finished on the screen
			}
		}
	}
}

bool Game::playerInput(bool quit = false) {
	// Handle events on queue
	while (SDL_PollEvent(&e) != 0) {
		// User requests quit	EXIT - CLOSE WINDOW
		if (e.type == SDL_QUIT) {
			quit = true;
		}
		//Reset start time on return keypress
		else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
			startTime = SDL_GetTicks();	// time since the program started in milliseconds
		}

		ship.handleEvent(e);							// Handle input for the ship
	}

	return quit;
}

void Game::moveGameObjects() {// SEAN: Cycle through list of laser objects and move them
	ship.move();							// Update ship movement
	checkCollision(ship.getCollider(), enemy.getCollider());
	enemy.moveEnemy();

	for (iter = listOfLaserObjects.begin(); iter != listOfLaserObjects.end();) {
		(*iter++)->move();					// Move the laser
	}
}







int backgroundLoopCounter = 0;
#define BACKGROUND_TIMES 4

void Game::renderGameObjects() {// Scroll background


	//--scrollingOffset;

	if(backgroundLoopCounter <= BACKGROUND_TIMES) scrollingOffset -= 5;
	if (scrollingOffset < -gBGTexture.getWidth()) {
		scrollingOffset = 0;				// update the scrolling background

		backgroundLoopCounter++;					// count the number of times the background has looped
		//std::cout << "Background has looped " << backgroundLoopCounter << " times" << std::endl;
	}

	// Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	// Render background
	if(backgroundLoopCounter < 1)
		gBGStartTexture.render(scrollingOffset, 0);			// 1st
	else if(backgroundLoopCounter > BACKGROUND_TIMES)
		gBGEndTexture.render(scrollingOffset, 0);			// end background
	else
		gBGTexture.render(scrollingOffset, 0);				// 3rd and every odd number

	if(backgroundLoopCounter < BACKGROUND_TIMES)
		gBGTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);		// 2nd background (and every even number)
	else
		gBGEndTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);		// end background

	//gBGTexture.render(scrollingOffset, 0);								// 1st background
	//gBGTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);		// 2nd background




	if (gameOver == false) {
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

//		if(enemy.mEnAlive) SDL_RenderDrawRect(gRenderer, &enemy.getCollider());
//		SDL_RenderDrawRect(gRenderer, &ship.getCollider());

		gShipTexture.setAlpha(playerAlpha);		// Flash the player ship -  Set the Alpha value for Enemy
		ship.render();							// render the ship over the background
		if(enemy.mEnAlive == true) enemy.render();

		// SEAN: Cycle through list of laser objects and render them to screen
		for (iter = listOfLaserObjects.begin(); iter != listOfLaserObjects.end();) {
			(*iter++)->render();	// Render the laser
		}
	}
	else {
		gGameOverTextTexture.setAlpha(gameOverAlpha);
		//gGameOverTextTexture.render((SCREEN_WIDTH - gGameOverTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gGameOverTextTexture.getHeight()) / 2);
		gGameOverTextTexture.render((SCREEN_WIDTH - gGameOverTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gGameOverTextTexture.getHeight() + 250) / 2); // FOR TESTING
	}

	//Render textures
	// Render prompt texture and time texture to the screen
	gPromptTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, SCREEN_HEIGHT - gPromptTextTexture.getHeight() - 8);
	gLevelTextTexture.render(10, 8);
	gTimeTextTexture.setAlpha(timerAlpha);	// Flash the timer
	gTimeTextTexture.render(600, 8);

	SDL_RenderPresent(gRenderer);			// Update screen
}



void Game::destroyGameObjects() {
	for (iter = listOfLaserObjects.begin(); iter != listOfLaserObjects.end();) {
		if (!(*iter)->getLaserAlive()) {
			iter = listOfLaserObjects.erase(iter);
			std::cout << "Laser Destroyed." << std::endl;
		}
		else {
			iter++;
		}
	}
}

// SEAN: Function to spawn laser at ships location
void Game::spawnLaser() {
	Laser* p_Laser = new Laser();
	p_Laser->spawn(ship.getShipX() + 65, ship.getShipY() + 30, 20);
	listOfLaserObjects.push_back(p_Laser);
}// end spawnLaser

void Ship::render() {
	gShipTexture.render(mPosX, mPosY);					// Show the ship
}

// SEAN: Function to render the laser objects to the screen
void Laser::render() {
	gLaserTexture.render(mPosX, mPosY);					// Show the Laser
}

void EnemyShip::render() {
	if(enemy.mEnAlive) gEnemyShipTexture.render(mEnPosX, mEnPosY);
}

bool checkCollision(SDL_Rect a, SDL_Rect b){
	//The sides of the rectangles
	int leftA, leftB, rightA, rightB;
	int topA, topB, bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)	{
		return false;
	}

	if (topA >= bottomB)	{
		return false;
	}

	if (rightA <= leftB)	{
		return false;
	}

	if (leftA >= rightB)	{
		return false;
	}

	std::cout << "Collision!" << std::endl;
	playerFlash = true;

	enemy.mEnAlive = false;

	ship.setShipX(ship.getShipX() - ship.getShipVelX());
	ship.setShipColX(ship.getShipX());

	ship.setShipY(ship.getShipY() - ship.getShipVelY());
	ship.setShipColY(ship.getShipY());

	return true;	//If none of the sides from A are outside B
}

int counter = 0;

//void Game::flashGameObject() {
void Game::flashGameObject(int &alpha, bool &flash, int rate, int times) {
		if (flash) {
			if (alphaDown > 5) {
				alphaDown -= rate;
				if (alpha < 5) alpha = 5;
				else alpha = alphaDown;
				if (alphaDown <= 5) alphaUp = 5;
			}
			if (alphaUp < 255) {
				alphaUp += rate;
				if (alpha > 255) alpha = 255;
				else alpha = alphaUp;
				if (alphaUp >= 255) alphaDown = 255;
			}

			if (times != 0 && counter > times * 50) {	// takes 25 decrements of 10 to set alpha to 5, and 25 increments to set alpha back to 255, 50 = 1 flash approx.
				flash = false;
				counter = 0;
				alpha = 255;
			}
		}
		else alpha = 255;	// Set visibility back to maximum

		counter++;
}
