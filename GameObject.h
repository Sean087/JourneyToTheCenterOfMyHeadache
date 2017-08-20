/* 2017/01/09:
JOE: Moved functionality common to game objects to GameObjects class reducing the code
*/
#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <SDL.h>
#include <SDL_image.h>
#include "LTexture.h"
#include <iostream>

class GameObject
{
public:
	GameObject();
	~GameObject();				// Deconstructor

	virtual void spawn();
	virtual void spawn(int x, int y);
	void spawn(int x, int y, int vx);
	void spawn(int x, int y, int vx, int vy);
	virtual void movement();
	//void render();
	//void renderTexture(LTexture renderThis);

	int getX();					// Get GameObject X coord
	int getY();					// Get GameObject Y coord
	int getVelX();
	int getVelY();
	bool getAlive();

	void setX(int x);			// Set GameObject X coord
	void setY(int y);			// Set GameObject Y coord
	void setVelX(int x);
	void setVelY(int y);
	void setAlive(bool alive);

private:
	// GameObject Variables
	int m_health;				// Value between 0 and 160
	int m_speed;				// Value between 1 and 4
	int m_x;					// GameObject x coord
	int m_y;					// GameObject y coord
	int m_xVel;
	int m_yVel;
	bool m_Alive;				// Is the weapon active on screen
};

#endif

// global functions friends of the GameObjects class
//friend std::ostream& operator<<(std::ostream& os, const GameObject& aGameObject);

//GameObject(int damage = 3);			// Constructor with default value of 3 for damage

//std::string m_typeID;				// GameObject name
//int m_Damage;						// Damage to reduce health after an object moves
//int m_direction;					// CA2 - set the objects direction

//void render();
//	void render();
//	void render(LTexture x);

//void spawn(std::string typeID, int health, int speed);
//void spawn(std::string typeID, int health, int speed, int x, int y);	// create an object
//void draw();						// print to the screen the typeID and its x and y coords
//virtual void update();				// virtual function
//void info();						// print all info relating to the object
//bool isAlive();						// return true if its health is greater than 0

// Getter Methods
//std::string getID() const;			// Get GameObject name / ID
//const int getHealth() const;		// Get GameObject health
//int getSpeed() const;				// Get GameObject speed

//int getDamage() const;				// Get GameObject Damage
// Setter Methods
//void setHealth(int health);			// set the health, Max health should be 160
//void setSpeed(int speed);			// Set speed between 1 and 4

// Indicate where the border has been impacted
//int reboundX = 0, reboundY = 0;		// Store impact point, to highlight
//int getImpactX() const;				// Get the x coord of impact object
//int getImpactY() const;				// Get the y coord of impact object
//void setImpactX(int x);				// Set and reset objects impact x coord
//void setImpactY(int y);				// Set and reset objects impact y coord

// Change GameObject direction
//int rebound;						// Amount to bounce back of perimeter
//enum directionName { SW = 1, South, SE, West, East = 6, NW, North, NE };	// Use readable values for directions
//int getDirection();					// CA2 - Get the objects direction
//void setDirection(int d);			// Sets the direction of the GameObject (game init())
//int direction;					// The direction GameObject is moving
//void setRandomDirection();			// CA2 - Set a random direction for game objects

//int getRandomCoord();				// Get Random Coordinate
//void setRandomCoords(int x, int y); // NEVER USED X OR Y
//void setRandomCoords();
//void checkCoords();
//void checkCoords(GameObject* objectToCheck);	// MOVED TO GAME as uniqueCoords	