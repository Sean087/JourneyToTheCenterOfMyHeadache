#include "PowerUp.h"

PowerUp::PowerUp() {
	GameObject::setColliderW(PUP_WIDTH);
	GameObject::setColliderH(PUP_HEIGHT);
}

void PowerUp::movement() {
	GameObject::movement();

	GameObject::setColliderX(getX());
	GameObject::setColliderY(getY());

	// destroy blood cell once it is offscreen
	if (getX() < -100) setAlive(false);
	else setAlive(true);
}

PowerUp::~PowerUp() {

}