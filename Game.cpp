// 09/01 Edited background to be 800 x 600 instead of 600 * 480

/*
    Nanobot replaces Enemy Ship, White Blood Cell, Power Up (Health Box)
    Laser Collides with Enemies. New music, and explosion sound effects

    2017-08-11:
        Joe: Change window title
        Joe: Add relative path for asset files in "Art" directory
        Joe: Changed getCollision() for ship and enemyship
        Joe: Add relative path for asset files in "Music" and "SoundFX" directories
*/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>			// 2017/01/09 JOE: SOUND - library we use to make audio playing easier
#include "Game.h"
#include "LTexture.h"
#include "Ship.h"
#include "EnemyShip.h"
#include "EnemyVirus.h"			// 2017/01/10 JOE: Added Seans virus enemy
#include "Laser.h"
#include "NinjaStar.h"
#include "BloodCell.h"			// 2017/01/10 JOE: Added Blood Cell
#include "BloodCellSmall.h"		// 2017/01/10 JOE: Added Small Blood Cell
#include "WhiteBloodCell.h"
#include "PowerUp.h"			// 2017/01/10 SEAN: Added Power Up

//The music that will be played
Mix_Music *gMusic = NULL;		// Mix_Music: Data type for music
//Mix_Music *gEngineFX = NULL;	// 2017/01/10 JOE: Change Engine effect to music, turn off when no enemy ship on screen

//The sound effects that will be used (pointers)
Mix_Chunk *gNinjaFX = NULL;		// 2017/01/09 JOE: SOUND - Mix_Chunk: Data type for short sounds
Mix_Chunk *gLaserFX = NULL;		// 2017/01/09 JOE: SOUND - Mix_Chunk: Data type for short sounds
Mix_Chunk *gEngineFX = NULL;	// 2017/01/09 JOE: SOUND - Mix_Chunk: Data type for short sounds
Mix_Chunk *gExplosionFX = NULL;

bool init();					// Starts up SDL and creates window
bool loadMedia();				// Loads media//void close();
bool checkCollision(SDL_Rect a, SDL_Rect b);

SDL_Window* gWindow = NULL;		// The window we'll be rendering to
SDL_Renderer* gRenderer = NULL;	// The window renderer
Uint8 a = 0;					// Modulation component

int scrollingOffset = 0;		// 2017/01/10 JOE: Declare the background scrolling offset (Moved as stops background scrolling when in the render function)

int degrees = 0;
int degreesBC = 0;				// 2017/01/10 JOE: Added degrees for blood cell rotation
int degreesBCS = 0;
int alphaUp = 5, alphaDown = 5;

//Scene textures
LTexture gBGTexture;			// Background
LTexture gShipTexture;			// Player ship
LTexture gEnemyShipTexture;		// Enemy ship
LTexture gEnemyVirusTexture;	// Enemy Virus
LTexture gLaserTexture;			// Texture for Laser weapon
LTexture gNinjaStar;			// Texture for Ninja Star weapon
LTexture gBloodCell;			// Texture for Blood Cell obstacle (classed as enemy as it causes damage on impact)
LTexture gBloodCellSmall;		// Texture for Smaller Blood Cell
LTexture gWhiteBloodCell;		// Texture for White Blood Cell
LTexture gPowerUp;				// Texture for Health Box Power Up
//LTexture gGO;

// SEAN: Move ship object outside of main so spawnLaser funtion can use it
Ship player;					// Declare a ship object that will be moving around on the screen
EnemyShip* enemy1 = new EnemyShip();
EnemyShip* enemy2 = new EnemyShip();
EnemyVirus* virus1 = new EnemyVirus();

std::vector<EnemyShip*> listOfEnemyShips;			// 2017/01/09 JOE: List to store laser objects
//std::list<EnemyShip*>::const_iterator iterES;	// 2017/01/09 JOE: Make them read only
std::vector<EnemyVirus*> listOfEnemyVirus;		// 2017/01/09 JOE: List to store laser objects
//std::list<EnemyVirus*>::const_iterator iterEV;	// 2017/01/09 JOE: Make them read only
std::list<BloodCell*> listOfBloodCells;			// 2017/01/10 JOE: List to store laser objects
std::list<BloodCell*>::const_iterator iterBC;	// 2017/01/10 JOE: Make them read only
std::list<BloodCellSmall*> listOfSmallBloodCells;			// 2017/01/10 JOE: List to store laser objects
std::list<BloodCellSmall*>::const_iterator iterSBC;	// 2017/01/10 JOE: Make them read only
std::list<WhiteBloodCell*> listOfWhiteBloodCells;
std::list<WhiteBloodCell*>::const_iterator iterWBC;
std::vector<PowerUp*> listOfPowerUps;
//std::list<PowerUp*>::const_iterator iterPU;
// SEAN : Created list and iterator for laser objects
std::vector<Laser*> listOfLaserObjects;			// List to store laser objects
//std::list<Laser*>::const_iterator iter;			// Make them read only

std::list<NinjaStar*> listOfNinjaStarObjects;	// 2017/01/09 JOE: List to store Ninja Star objects
std::list<NinjaStar*>::const_iterator iterNS;	// 2017/01/09 JOE: Create global iterators to cycle through laser objects - Make them read only


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

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };	// Set rendering space and render to screen

	// Set clip rendering dimensions
	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);	// Render to screen
}

bool init() {
	bool success = true;						// Initialization flag

	srand(static_cast<unsigned int>(time(0)));	// Seed the random number

	//player.spawn(10, SCREEN_HEIGHT / 2);
	//(*enemy1).spawn(800,200, -5);
	//(*enemy2).spawn(850, 400, -5);				// Spawn x, y, and x velocity
	//(*virus1).spawn(900, 300, -3);
	//listOfEnemyShips.push_back(enemy1);
	//listOfEnemyShips.push_back(enemy2);
	//listOfEnemyVirus.push_back(virus1);

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {	// 2017/01/09 JOE: SOUND - Neet do initialise audio with video
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	} else {
		// Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			printf("Warning: Linear texture filtering not enabled!");
		}

		// Create window
		gWindow = SDL_CreateWindow("JOURNEY TO THE CENTER OF MY HEADACHE v1.15 by Joe O'Regan & Se\u00E1n Horgan - Laser Collisions Destroy Enemies", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);	/* Window name */
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

				//call Mix_oopenAudio to Initialize SDL_mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)	// sound frequencey, sample format, hardware channels, sample size
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());	// report errors with SDL_mixer
					success = false;
				}

				Mix_Volume(-1, MIX_MAX_VOLUME);	// sets the volume for all channels MIX_MAX_VOLUME = 128 BALANCES OUT THE VOLUMES A BIT BETTER - LEAVE IN

				//If there is no music playing
				//if (Mix_PlayingMusic() == 0)
				//{
				//Play the music
				//	Mix_PlayMusic(gMusic, -1);	// start if not playing
				//}
			}
		}
	}

	return success;
}

bool loadMedia() {
	bool success = true;			// Loading success flag

	// Load Ship texture
	if (!gShipTexture.loadFromFile(".\\Art\\Player1Ship.png")) {
		printf("Failed to load Player texture!\n");
		success = false;
	}
	// Load Enemy Ship texture
	if (!gEnemyShipTexture.loadFromFile(".\\Art\\NanobotOld.png")) {
		printf("Failed to load Enemy Ship texture!\n");
		success = false;
	}
	// Load Enemy Virus texture
	if (!gEnemyVirusTexture.loadFromFile(".\\Art\\EnemyVirus.png")) {
		printf("Failed to load Enemy Virus texture!\n");
		success = false;
	}
	// Load Blood Cell texture
	if (!gBloodCell.loadFromFile(".\\Art\\bloodCell.png")) {			// 10/01 Added Blood Cell
		printf("Failed to load Blood Cell texture!\n");
		success = false;
	}
	// Load Blood Cell texture
	if (!gBloodCellSmall.loadFromFile(".\\Art\\bloodCell2b.png")) {			// 10/01 Added Blood Cell
		printf("Failed to load Small Blood Cell texture!\n");
		success = false;
	}

	if (!gBloodCellSmall.loadFromFile(".\\Art\\WhiteCell.png")) {			// 10/01 Added Blood Cell
		printf("Failed to load White Blood Cell texture!\n");
		success = false;
	}
	// Load Power Up texture
	if (!gPowerUp.loadFromFile(".\\Art\\HealthBox.png")) {			// 10/01 Added Blood Cell
		printf("Failed to load Health Box texture!\n");
		success = false;
	}
	// Load background texture
	if (!gBGTexture.loadFromFile(".\\Art\\bg3.png")) {				// 09/01 Edited background to be 800 x 600 instead of 600 * 480
		printf("Failed to load background texture!\n");
		success = false;
	}
	// Load player texture for testing
	//if (!gGO.loadFromFile("PlayerShip1.png")) {				// 09/01 Edited background to be 800 x 600 instead of 600 * 480
	//	printf("Failed to load background texture!\n");
	//	success = false;
	//}

	// Load Laser texture
	//if (!gLaserTexture.loadFromFile("LaserBeam4b.png")) {
	if (!gLaserTexture.loadFromFile(".\\Art\\LaserGreen.png")) {
		printf("Failed to load Laser texture!\n");
		success = false;
	}

	// Load Ninja Star texture
	if (!gNinjaStar.loadFromFile(".\\Art\\NinjaStar.png")) {
		printf("Failed to load Ninja Star texture!\n");
		success = false;
	}

	//Load music
	gMusic = Mix_LoadMUS(".\\Music\\GameSong2.mp3");	// Load music
	if (gMusic == NULL) {
		printf("Failed to load rage music! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}
	/*
	gEngineFX = Mix_LoadMUS("Engine1.wav");	// Load music
	if (gEngineFX == NULL) {
		printf("Failed to load engine effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	} */
	//Load sound effects
	gNinjaFX = Mix_LoadWAV(".\\SoundFx\\Swoosh1.wav");	// Load sound effects
	if (gNinjaFX == NULL) {
		printf("Failed to load ninja star sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	gLaserFX = Mix_LoadWAV(".\\SoundFx\\Laser1.wav");	// Load sound effects
	if (gLaserFX == NULL) {
		printf("Failed to load laser beam sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	gExplosionFX = Mix_LoadWAV(".\\SoundFx\\explosion.wav");	// Load sound effects
	if (gExplosionFX == NULL) {
		printf("Failed to load explosion sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}

	/*gEngineFX = Mix_LoadWAV("Engine1.wav");	// Load sound effects
	if (gNinjaFX == NULL) {
		printf("Failed to load engine sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		success = false;
	}*/

	Mix_PlayMusic(gMusic, -1);
	//Mix_PlayMusic(gEngineFX, -1);	// can only play one music track at a time

	return success;
}

void Game::close() {
	// Free loaded images
	gShipTexture.free();
	gEnemyShipTexture.free();
	gEnemyVirusTexture.free();
	gBGTexture.free();
	gLaserTexture.free();
	gNinjaStar.free();
	gBloodCell.free();
	gBloodCellSmall.free();
	gWhiteBloodCell.free();
	gPowerUp.free();
	//gGO.free();

	//Free the sound effects
	Mix_FreeChunk(gNinjaFX);	// Free a sound effect
	Mix_FreeChunk(gLaserFX);	// Free a sound effect
	Mix_FreeChunk(gEngineFX);	// Free a sound effect
	Mix_FreeChunk(gExplosionFX);	// Free a sound effect

	//Free the music
	Mix_FreeMusic(gMusic);	// Free music
	gMusic = NULL;
	//Mix_FreeMusic(gEngineFX);	// Free music
	//gEngineFX = NULL;

	// Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

//int main(int argc, char* args[]) {
void Game::update(){
	// Start up SDL and create window
	if (!init()) {
		printf("Failed to initialize!\n");
	} else {
		// Load media
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		} else {
			bool quit = false;							// Main loop flag

			// While application is running
			while (!quit) {
				// Handle events on queue
				//playerFlashOnCollide();					// 2017-01-09 JOE: The player texture flashes on collision with other Game Objects
				//player.render();

				quit = playerInput(quit);				// 2017/01/09 JOE: Handle input from player

				renderGameObjects();					// 2017-01-09 JOE: Render the game objects to the screen

				moveGameObjects();						// 2017-01-09 JOE: Move the game objects on the screen

  				SDL_RenderPresent(gRenderer);			// Update screen

				destroyGameObjects();					// 2017-01-09 JOE: Destroy the game objects when finished on the screen

				audio();								// 2017-01-10 JOE: Handle the audio for game objects

				spawnEnemies();							// 2017/01/10 JOE: Spawn enemies and obstacles at random coords and distances apart
			}
		}
	}
}

void Game::audio() {
	// Cycle through list of Enemy ships and play engine sound
	if (listOfEnemyShips.size() > 0){
	//for (iterES = listOfEnemyShips.begin(); iterES != listOfEnemyShips.end();) {
		//if ((*iterES++)->getX() < 800 && (*iterES++)->getX() > 0 - (*iterES)->getESwidth())
		//if ((*iterES++)->getX() == 800) {
			//Mix_PlayChannel(-1, gEngineFX, 0);
			Mix_FadeInChannel(-1, gEngineFX, 0, 300);
			//std::cout << "test audio" << std::endl;
		//}
	}
	if (listOfEnemyShips.size() == 1 && (*listOfEnemyShips.begin())->getX() < 0)
		Mix_FadeOutChannel(-1, 300);
}

bool Game::playerInput(bool quit = false) {
	SDL_Event e;								// Event handler

	while (SDL_PollEvent(&e) != 0) {
		// User requests quit	EXIT - CLOSE WINDOW
		if (e.type == SDL_QUIT) {
			quit = true;
		}

		player.handleEvent(e);				// Handle input for the ship
	}
	return quit;
}

void Game::renderGameObjects() {
	//int scrollingOffset = 0;					// Declare the background scrolling offset

	degrees += 5;								// Number of degrees to rotate spinning objects
	degrees %= 360;
	degreesBC += 1;								// Number of degrees to rotate spinning objects
	degreesBC %= 360;
	degreesBCS += 2;								// Number of degrees to rotate spinning objects
	degreesBCS %= 360;

	// Scroll background
	--scrollingOffset;
	if (scrollingOffset < -gBGTexture.getWidth()) {
		scrollingOffset = 0;				// update the scrolling background
	}
	// Clear screen
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(gRenderer);

	// Render background
	gBGTexture.render(scrollingOffset, 0);
	gBGTexture.render(scrollingOffset + gBGTexture.getWidth(), 0);

	// Cycle through list of small Blood Cells obstacles and render to screen
	for (iterSBC = listOfSmallBloodCells.begin(); iterSBC != listOfSmallBloodCells.end();) {
		(*iterSBC++)->render();	// Render the blood cell
	}

	// Cycle through list of small Blood Cells obstacles and render to screen
	for (iterWBC = listOfWhiteBloodCells.begin(); iterWBC != listOfWhiteBloodCells.end();) {
		(*iterWBC++)->render();	// Render the blood cell
	}
	// Cycle through list of Enemy ships and render to screen
	for (unsigned int index = 0; index != listOfEnemyShips.size(); ++index) {
		listOfEnemyShips[index]->render();
		//SDL_RenderDrawRect(gRenderer, &listOfEnemyShips[index]->getCollider());
	}
	// Cycle through list of Enemy Virus and render to screen
	for (unsigned int index = 0; index != listOfEnemyVirus.size(); ++index) {
		listOfEnemyVirus[index]->render();
		//SDL_RenderDrawRect(gRenderer, &listOfEnemyVirus[index]->getCollider());
	}
	// Cycle through list of Blood Cells obstacles and render to screen
	for (iterBC = listOfBloodCells.begin(); iterBC != listOfBloodCells.end();) {
		(*iterBC++)->render();	// Render the blood cell
	}

	// Cycle through list of power up objects and render them to screen
	for (unsigned int index = 0; index != listOfPowerUps.size(); ++index) {
		listOfPowerUps[index]->render();
//		SDL_RenderDrawRect(gRenderer, &listOfPowerUps[index]->getCollider());
	}

	// Cycle through list of laser objects and render them to screen
	for (unsigned int index = 0; index != listOfLaserObjects.size(); ++index) {
		listOfLaserObjects[index]->render();
		//SDL_RenderDrawRect(gRenderer, &listOfLaserObjects[index]->getLaserCollider());
	}
	for (iterNS = listOfNinjaStarObjects.begin(); iterNS != listOfNinjaStarObjects.end();) {
		(*iterNS++)->render();	// Render the ninja star
	}

	/* Set the Alpha value for Enemy */
	//gEnemyShipTexture.setAlpha(a);			// will flash for all enemies NO GOOD
	//gShipTexture.setAlpha(a);
	player.render();							// render the ship over the background
}

void Game::moveGameObjects() {
	player.movement();						// Update ship movement
	for (unsigned int index = 0; index != listOfPowerUps.size(); ++index) {
		if (checkCollision(player.getCollider(), listOfPowerUps[index]->getCollider()) == true) {
			listOfPowerUps[index]->setAlive(false);
			std::cout << "Power Up Picked Up by Player!\n";
		}
	}

	// Cycle through lists of Enemys and move them
	for (unsigned int index = 0; index != listOfEnemyShips.size(); ++index) {
		listOfEnemyShips[index]->movement();
		//SDL_RenderDrawRect(gRenderer, &listOfLaserObjects[index]->getLaserCollider());
	}
	// Cycle through list of Enemy virus and move them
	for (unsigned int index = 0; index != listOfEnemyVirus.size(); ++index) {
		listOfEnemyVirus[index]->movement(player.getX(), player.getY());
	}
	// Cycle through list of Blood Cells and move them
	for (iterBC = listOfBloodCells.begin(); iterBC != listOfBloodCells.end();) {
		(*iterBC++)->movement();	// 2017/01/10 JOE: Move the blood cells in a wavey line
	}
	// Cycle through list of Small Blood Cells and move them
	for (iterSBC = listOfSmallBloodCells.begin(); iterSBC != listOfSmallBloodCells.end();) {
		(*iterSBC++)->movement();	// 2017/01/10 JOE: Move the blood cells in a wavey line
	}

	for (iterWBC = listOfWhiteBloodCells.begin(); iterWBC != listOfWhiteBloodCells.end();) {
		(*iterWBC++)->movement();	// Render the blood cell
	}

	// Cycle through list of Power Ups and move them
	for (unsigned int index = 0; index != listOfPowerUps.size(); ++index) {
		listOfPowerUps[index]->movement();
	}

	// Cycle through lists of weapons and move them
	for (unsigned int index = 0; index != listOfLaserObjects.size(); ++index) {
		listOfLaserObjects[index]->movement();
		for (unsigned int index1 = 0; index1 != listOfEnemyVirus.size(); ++index1) {
			if (checkCollision(listOfLaserObjects[index]->getLaserCollider(), listOfEnemyVirus[index1]->getCollider()) == true) {
				listOfEnemyVirus[index1]->setAlive(false);
				listOfLaserObjects[index]->setAlive(false);
				std::cout << "Enemy Virus Killed by Player!\n";
				Mix_PlayChannel(-1, gExplosionFX, 0);
			}
		}
		for (unsigned int index2 = 0; index2 != listOfEnemyShips.size(); ++index2) {
			if (checkCollision(listOfLaserObjects[index]->getLaserCollider(), listOfEnemyShips[index2]->getCollider()) == true) {
				listOfEnemyShips[index2]->setAlive(false);
				listOfLaserObjects[index]->setAlive(false);
				std::cout << "Enemy Ship Killed by Player!\n";
				Mix_PlayChannel(-1, gExplosionFX, 0);
			}
		}
	}
	for (iterNS = listOfNinjaStarObjects.begin(); iterNS != listOfNinjaStarObjects.end();) {
		(*iterNS++)->movement();	// Move the ninja star
	}
}
// Destroy Game Objects
void Game::destroyGameObjects() {
	for (unsigned int index = 0; index != listOfEnemyShips.size(); ++index) {
		if (!listOfEnemyShips[index]->getAlive()) {
			listOfEnemyShips.erase(listOfEnemyShips.begin() + index);
			std::cout << "Enemy Ship Destroyed." << std::endl;
			index--;
		}
	}// end for
	for (unsigned int index = 0; index != listOfEnemyVirus.size(); ++index) {
		if (!listOfEnemyVirus[index]->getAlive()) {
			listOfEnemyVirus.erase(listOfEnemyVirus.begin() + index);
			std::cout << "Enemy Virus Destroyed." << std::endl;
			index--;
		}
	}// end for
	for (iterBC = listOfBloodCells.begin(); iterBC != listOfBloodCells.end();) {
		if (!(*iterBC)->getAlive()) {
			iterBC = listOfBloodCells.erase(iterBC);
			std::cout << "destroy blood cell" << std::endl;
		}
		else {
			iterBC++;
		}
	}
	for (iterSBC = listOfSmallBloodCells.begin(); iterSBC != listOfSmallBloodCells.end();) {
		if (!(*iterSBC)->getAlive()) {
			iterSBC = listOfSmallBloodCells.erase(iterSBC);
			std::cout << "destroy small blood cell" << std::endl;
		}
		else {
			iterSBC++;
		}
	}
	for (iterWBC = listOfWhiteBloodCells.begin(); iterWBC != listOfWhiteBloodCells.end();) {
		if (!(*iterWBC)->getAlive()) {
			iterWBC = listOfWhiteBloodCells.erase(iterWBC);
			std::cout << "destroy blood cell" << std::endl;
		}
		else {
			iterWBC++;
		}
	}
	for (unsigned int index = 0; index != listOfLaserObjects.size(); ++index) {
		if (!listOfLaserObjects[index]->getAlive()) {
			listOfLaserObjects.erase(listOfLaserObjects.begin() + index);
			std::cout << "Laser Destroyed." << std::endl;
			index--;
		}
	}// end for
	for (iterNS = listOfNinjaStarObjects.begin(); iterNS != listOfNinjaStarObjects.end();) {
		if (!(*iterNS)->getAlive()) {
			iterNS = listOfNinjaStarObjects.erase(iterNS);
			std::cout << "destroy ninja star" << std::endl;
		}
		else {
			iterNS++;
		}
	}
	for (unsigned int index = 0; index != listOfPowerUps.size(); ++index) {
		if (!listOfPowerUps[index]->getAlive()) {
			listOfPowerUps.erase(listOfPowerUps.begin() + index);
			std::cout << "Power Up Destroyed." << std::endl;
			index--;
		}
	}// end for
}

/*void Game::playerFlashOnCollide() {
	if (alphaUp < 255) {
		alphaUp += 10;

		if (a > 255) a = 255;
		else a = alphaUp;

		if (alphaUp >= 255) alphaDown = 255;
	}

	if (alphaDown > 5) {
		alphaDown -= 10;

		if (a < 5) a = 5;
		else a = alphaDown;

		if (alphaDown <= 5) alphaUp = 5;
	}
}*/


// Decide how many of each enemy / obstacle on screen at a given time
void Game::spawnEnemies() {
	if (listOfEnemyShips.size() <= 1) {
		spawnEnemyShip();
	}
	if (listOfEnemyVirus.size() <= 2) {
		spawnEnemyVirus();
	}
	if (listOfBloodCells.size() <= 2) {
		spawnBloodCell();
	}
	if (listOfSmallBloodCells.size() <= 2) {
		spawnSmallBloodCell();
	}
	if(listOfWhiteBloodCells.size() <= 2) {
		spawnWhiteBloodCell();
	}
	if (listOfPowerUps.size() == 0) {
		spawnPowerUp();
	}
}

// List of enemy ships to spawn at random times and positions
void Game::spawnEnemyShip() {
	int x, y, randomX, randomY, randomSpeed;
	randomX = rand() % 5 + 1;
	randomY = rand() % 5 + 1;
	randomSpeed = rand() % 3 + 3;

	x = SCREEN_WIDTH + (randomX * 50);
	y = (randomY * 120) - 80;

	//int y = SCREEN_HEIGHT / 2;
	EnemyShip* p_Enemy = new EnemyShip();
	//p_Enemy->spawn(800, y, -5);
	p_Enemy->spawn(x, y, -randomSpeed, p_Enemy->getCollider());
	listOfEnemyShips.push_back(p_Enemy);
}

void Game::spawnEnemyVirus() {
	int x, y, randomX, randomY;
	randomX = rand() % 5 + 1;
	randomY = rand() % 5 + 1;
	x = SCREEN_WIDTH + (randomX * 50);
	y = (randomY * 120) - 80;

	//int y = SCREEN_HEIGHT / 2;
	EnemyVirus* p_Virus = new EnemyVirus();
	//p_Virus->spawn(800, y, -2);
	p_Virus->spawn(x, y, -4, -2, p_Virus->getCollider());
	listOfEnemyVirus.push_back(p_Virus);
}

void Game::spawnBloodCell() {
	int x, y, randomX, randomY, randomSpeed;
	randomX = rand() % 5 + 1;
	randomY = rand() % 5 + 1;
	randomSpeed = rand() % 3 + 1;

	x = SCREEN_WIDTH + (randomX * 200);
	y = (randomY * 120) - 80;

	BloodCell* p_BloodCell = new BloodCell();
	p_BloodCell->spawn(x, y, -randomSpeed);
	listOfBloodCells.push_back(p_BloodCell);
}
void Game::spawnSmallBloodCell() {
	int x, y, randomX, randomY, randomSpeed;
	randomX = rand() % 5 + 1;
	randomY = rand() % 5 + 1;
	randomSpeed = rand() % 3 + 1;

	x = SCREEN_WIDTH + (randomX * 150);
	y = (randomY * 120) - 80;

	BloodCellSmall* p_SmallBloodCell = new BloodCellSmall();
	p_SmallBloodCell->spawn(x, y, -randomSpeed);
	listOfSmallBloodCells.push_back(p_SmallBloodCell);
}

void Game::spawnWhiteBloodCell() {
	int x, y, randomX, randomY, randomSpeed;
	randomX = rand() % 5 + 1;
	randomY = rand() % 5 + 1;
	randomSpeed = rand() % 3 + 1;

	x = SCREEN_WIDTH + (randomX * 150);
	y = (randomY * 120) - 80;

	WhiteBloodCell* p_WhiteBloodCell = new WhiteBloodCell();
	p_WhiteBloodCell->spawn(x, y, -randomSpeed);
	listOfWhiteBloodCells.push_back(p_WhiteBloodCell);
}

void Game::engineFX() {
//	Mix_PlayChannel(-1, gEngineFX, 0);
}

// Spawn Weapon at ships location
void Game::spawnLaser() {
	Laser* p_Laser = new Laser();
	p_Laser->spawn(player.getX(), player.getY(), p_Laser->getLaserCollider());
	listOfLaserObjects.push_back(p_Laser);
	Mix_PlayChannel(-1, gLaserFX, 0);
}
void Game::spawnNinjaStar() {
	NinjaStar* p_NinjaStar = new NinjaStar();
	p_NinjaStar->spawn(player.getX(), player.getY());
	listOfNinjaStarObjects.push_back(p_NinjaStar);
	Mix_PlayChannel(-1, gNinjaFX, 0);
}

void Game::spawnPowerUp() {

	int x, y, randomX, randomY, randomSpeed;
	randomX = rand() % 5 + 1;
	randomY = rand() % 5 + 1;
	randomSpeed = rand() % 3 + 1;

	x = SCREEN_WIDTH + (randomX * 150);
	y = (randomY * 120) - 80;
	PowerUp* p_PowerUp = new PowerUp();
	p_PowerUp->spawn(x, y, -randomSpeed);
	listOfPowerUps.push_back(p_PowerUp);
}

void Ship::render() {
	gShipTexture.render(player.getX(), player.getY());				// Show the ship
}

// Render the laser objects to the screen
void Laser::render() {
	gLaserTexture.render(getX(), getY());
}

/* Render the laser objects to the screen*/
void NinjaStar::render() {
	gNinjaStar.render(getX(), getY(), NULL, degrees, NULL, SDL_FLIP_NONE);
}
// Enemies and Obstacles
void EnemyShip::render() {
	gEnemyShipTexture.render(getX(), getY());
}
void EnemyVirus::render() {
	gEnemyVirusTexture.render(getX(), getY());
}
void BloodCell::render() {
	//gBloodCell.render(getX(), getY());
	gBloodCell.render(getX(), getY(), NULL, -degreesBC, NULL, SDL_FLIP_NONE);
}
void BloodCellSmall::render() {
	gBloodCellSmall.render(getX(), getY(), NULL, -degreesBCS, NULL, SDL_FLIP_NONE);
}
void WhiteBloodCell::render() {
	gWhiteBloodCell.render(getX(), getY(), NULL, -degreesBCS, NULL, SDL_FLIP_NONE);
}
void PowerUp::render() {
	gPowerUp.render(getX(), getY());
}
//void GameObject::render() {
//	gGO.render(getX(), getY());
//}
/*
void GameObject::renderTexture(LTexture x) {
	x.render(getX(), getY());
}
*/

bool checkCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

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
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	std::cout << "Collision!\n";

	//Handle ship collision with enemy (MOVE TO SEPERATE FUNCTION)
	/*ship.setShipX(ship.getShipX() - ship.getShipVelX());
	ship.setShipColX(ship.getShipX());

	ship.setShipY(ship.getShipY() - ship.getShipVelY());
	ship.setShipColY(ship.getShipY());*/

	//If none of the sides from A are outside B
	return true;
}
