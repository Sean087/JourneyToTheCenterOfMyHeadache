
/*
    Countdown Timer

    2017-08-11:
        Joe: Change window title
        Joe: Add relative path for asset files in "Art" directory
        Joe: Changed getCollision() for ship and enemyship
        Joe: Add relative path for asset files in "Music" and "SoundFX" directories
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

Uint8 a = 255;					// Modulation component for flashing objects
bool flash = false;
bool gameOver = false;

//bool init();					// Starts up SDL and creates window
bool loadMedia();				// Loads media//void close();

SDL_Window* gWindow = NULL;		// The window we'll be rendering to
SDL_Renderer* gRenderer = NULL;	// The window renderer

TTF_Font *gFont = NULL;			// Globally used font

//Scene textures
LTexture gTimeTextTexture;
LTexture gPromptTextTexture;
LTexture gLevelTextTexture;

Uint32 startTime = 6000;			// Unsigned integer 32-bits

//Scene textures
LTexture gBGTexture;
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
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		gWindow = SDL_CreateWindow("JOURNEY TO THE CENTER OF MY HEADACHE v1.12 by Joe O'Regan & Se\u00E1n Horgan - Countdown Timer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);	/* Create Window with name */
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
		if (!gPromptTextTexture.loadFromRenderedText("Press Enter to Reset Start Time.", textColor)) {
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

	if (!gEnemyShipTexture.loadFromFile(".\\Art\\EnemyVirus.png")) {		// Load Enemy Ship texture
		printf("Failed to load Enemy texture!\n");
		success = false;
	}

	if (!gBGTexture.loadFromFile(".\\Art\\Background800.png")) {	// Load background texture
		printf("Failed to load background texture!\n");
		success = false;
	}

	if (!gLaserTexture.loadFromFile(".\\Art\\LaserBeam.png")) {	// SEAN: Load Laser texture
		printf("Failed to load Laser texture!\n");
		success = false;
	}

	if (!gGameOverTextTexture.loadFromFile(".\\Art\\GameOver.png")) {
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
	gLaserTexture.free();
	gGameOverTextTexture.free();

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

			//SDL_Color textColor = { 0, 0, 0, 255 };	//Set text color as black
			SDL_Color textColor = { 0, 100, 200, 255 };



			// TEST TIMING
			unsigned int lastTime = 0, currentTime, countdownTimer = 3;



			//Current time start time
			//Uint32 startTime = 0;			// Unsigned integer 32-bits

			//In memory text stream
			// string streams - function like iostreams only instead of reading or writing to the console,
			// they allow you to read and write to a string in memory
			std::stringstream timeText;		// string stream

			while (!quit) {							// While application is running
				playerFlashOnCollide();

				quit = playerInput(quit);			// 2017/01/09 JOE: Handle input from player


				//Set text to be rendered - string stream - print the time since timer last started
				timeText.str("");			// initialise empty

				currentTime = SDL_GetTicks();

				if (currentTime > lastTime + 1000) {
					lastTime = currentTime;

					countdownTimer -= 1;

					std::cout << "Time: " << countdownTimer << " lastTime: " << lastTime << " currentTime: " << currentTime << std::endl;
				}

				if (countdownTimer > 3 && countdownTimer < 10) {
					timeText << "Game Over";
					gameOver = true;
				}
				else if (countdownTimer >= 0 && countdownTimer <= 3) {
					timeText << "Time: " << countdownTimer;
					gameOver = false;
				}
				if (countdownTimer <= 0 || countdownTimer > 10) {
					timeText << "Game Over";
					gameOver = true;
					countdownTimer = 5;
				}



				/*
				if (countdownTimer > 5)
					timeText << "Game Over";
				if (countdownTimer > 0 && countdownTimer < 10)
					timeText << "Time: " << countdownTimer + 1;
				//else if (countdownTimer < 1 || countdownTimer > 5) {
				else if (countdownTimer < 1) {
					timeText << "Game Over";
					//countdownTimer = 8;
				}
				if (countdownTimer == 0)
					countdownTimer = 8;
				*/
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
	SDL_Event e;										// Event handler

	// Handle events on queue
	while (SDL_PollEvent(&e) != 0) {
		// User requests quit	EXIT - CLOSE WINDOW
		if (e.type == SDL_QUIT) {
			quit = true;
		}
		//Reset start time on return keypress
		else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN) {
			startTime = SDL_GetTicks();	// time since the program started in milliseconds
			//startTime -= SDL_GetTicks();	// time since the program started in milliseconds
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

void Game::renderGameObjects() {// Scroll background
	--scrollingOffset;
	if (scrollingOffset < -gBGTexture.getWidth()) {
		scrollingOffset = 0;				// update the scrolling background
	}

	// Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	// Render background
	gBGTexture.render(scrollingOffset, 0);								// 1st background
	gBGTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);		// 2nd background

	if (gameOver == false) {
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

		//SDL_RenderDrawRect(gRenderer, &enemy.getCollider());
		//SDL_RenderDrawRect(gRenderer, &ship.getCollider());

		gShipTexture.setAlpha(a);				/* Set the Alpha value for Enemy */
		ship.render();							// render the ship over the background
		enemy.render();

		// SEAN: Cycle through list of laser objects and render them to screen
		for (iter = listOfLaserObjects.begin(); iter != listOfLaserObjects.end();) {
			(*iter++)->render();	// Render the laser
		}
	}
	else {
	gGameOverTextTexture.render((SCREEN_WIDTH - gGameOverTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gGameOverTextTexture.getHeight()) / 2);


	//gGameOverTextTexture.render(100, 100);

	}


	//Render textures
	// Render prompt texture and time texture to the screen
	gPromptTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, SCREEN_HEIGHT - gPromptTextTexture.getHeight() - 8);
	//gPromptTextTexture.render(10, 8);
	gLevelTextTexture.render(10, 8);
	//gTimeTextTexture.render((SCREEN_WIDTH - gPromptTextTexture.getWidth()) / 2, (SCREEN_HEIGHT - gPromptTextTexture.getHeight()) / 2);
	//gTimeTextTexture.render((SCREEN_WIDTH - (gPromptTextTexture.getWidth()) + gTimeTextTexture.getWidth()) / 2, 0);
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
	}// end for
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
	gEnemyShipTexture.render(mEnPosX, mEnPosY);
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
	//playerFlashOnCollide();
	flash = true;

	ship.setShipX(ship.getShipX() - ship.getShipVelX());
	ship.setShipColX(ship.getShipX());

	ship.setShipY(ship.getShipY() - ship.getShipVelY());
	ship.setShipColY(ship.getShipY());

	return true;	//If none of the sides from A are outside B
}


int counter = 0;

void Game::playerFlashOnCollide() {
		if (flash) {
			if (alphaDown > 5) {
				alphaDown -= 10;
				if (a < 5) a = 5;
				else a = alphaDown;
				if (alphaDown <= 5) alphaUp = 5;
			}
			if (alphaUp < 255) {
				alphaUp += 10;
				if (a > 255) a = 255;
				else a = alphaUp;
				if (alphaUp >= 255) alphaDown = 255;
			}

			if (counter > 150) {	// takes 25 decrements of 10 to set alpha to 5, and 25 increments to set alpha back to 255, 50 = 1 flash approx.
				flash = false;
				counter = 0;
			}
		}
		//else a = 255;	// visible -> dont need changed default a value to 255

		counter++;
}
