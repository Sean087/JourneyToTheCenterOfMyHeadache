#pragma once
#ifndef POWERUP_H
#define POWERUP_H

#include "GameObject.h"

class PowerUp : public GameObject {
public:
	PowerUp();

	static const int PUP_WIDTH = 60;
	static const int PUP_HEIGHT = 60;

	static const int PowerUp_VEL = 1;			// 2017/01/10 JOE: Maximum axis velocity of the Blood Cell obstacle

	virtual void movement();
	void render();							// Shows the Power Up on the screen

	~PowerUp();
};

#endif