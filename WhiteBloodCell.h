#pragma once
#ifndef WHITE_BLOOD_CELL_H
#define WHITE_BLOOD_CELL_H

#include "Enemy.h"

class WhiteBloodCell : public Enemy {
public:
	WhiteBloodCell();							// Initializes the variables
	~WhiteBloodCell();

	// The dimensions of the Enemy (dimensions of sprite image)
	//static const int ENEMY_WIDTH = 100;
	//static const int ENEMY_HEIGHT = 47;

	static const int WBLOOD_VEL = 1;			// 2017/01/10 JOE: Maximum axis velocity of the Blood Cell obstacle

	virtual void movement();
	void render();							// Shows the Blood Cell on the screen
};

#endif