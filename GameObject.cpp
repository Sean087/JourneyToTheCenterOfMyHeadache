#include "GameObject.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Constructor
//GameObject::GameObject(int damage) :	// Constructor has default value for damage of 3
//	m_Damage(damage)
//{}
GameObject::GameObject() {
	m_x = 0;
	m_y = 0;
	m_xVel = 0;
	m_yVel = 0;
}
// Deconstructors
GameObject::~GameObject() {
	//std::cout << "GameObject deconstructor" << std::endl;
}

void GameObject::spawn() {
	m_x = 0;
	m_y = 0;
	m_xVel = 0;
	m_yVel = 0;
}
void GameObject::spawn(int x, int y) {
	m_x = x;
	m_y = y;
	m_xVel = 0;
	m_yVel = 0;
}
void GameObject::spawn(int x, int y, int vx) {
	m_x = x;
	m_y = y;
	m_xVel = vx;
	m_yVel = vx;	// 2017-01-10 JOE: use same velocity for x and y
}

void GameObject::spawn(int x, int y, int vx, int vy) {
	m_x = x;
	m_y = y;
	m_xVel = vx;
	m_yVel = vy;	// 2017-01-10 JOE: use same velocity for x and y
}

void GameObject::spawn(int x, int y, int vx, SDL_Rect collider) {
	m_x = x;
	m_y = y;
	m_xVel = vx;
	m_yVel = vx;	// 2017-01-10 JOE: use same velocity for x and y
	mCollider = collider;
}

void GameObject::spawn(int x, int y, int vx, int vy, SDL_Rect collider) {
	m_x = x;
	m_y = y;
	m_xVel = vx;
	m_yVel = vy;	// 2017-01-10 JOE: use same velocity for x and y
	mCollider = collider;

}

void GameObject::movement() {
	m_x += m_xVel;
}

//void GameObject::render() {

//}

// Getter and Setter methods
int GameObject::getX() {
	return m_x;
}

int GameObject::getY() {
	return m_y;
}

int GameObject::getVelX() {
	return m_xVel;
}

int GameObject::getVelY() {
	return m_yVel;
}
bool GameObject::getAlive() {
	return m_Alive;
}

void GameObject::setX(int x) {
	m_x = x;
}

void GameObject::setY(int y) {
	m_y = y;
}

void GameObject::setVelX(int x) {
	m_xVel = x;
}
void GameObject::setVelY(int y) {
	m_yVel = y;
}
void GameObject::setAlive(bool alive) {
	m_Alive = alive;
}

SDL_Rect GameObject::getCollider()
{
	return mCollider;
}

void GameObject::setColliderW(int w)
{
	mCollider.w = w;
}

void GameObject::setColliderH(int h)
{
	mCollider.h = h;
}

void GameObject::setColliderX(int x)
{
	mCollider.x = x;
}

void GameObject::setColliderY(int y)
{
	mCollider.y = y;
}
