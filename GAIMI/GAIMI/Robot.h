#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <list>
#include "Texture.h"
#include "Globals.h"
#include "Tile.h"


class Robot
{
private:
	SDL_Renderer* renderer;

	Texture* mySprite;

	float posX;	// current X position
	float posY;	// current Y position

	float lastX;	// X position at the end of the last move
	float lastY;	// Y position at the end of the last move

	Tile* currentTile;	// stores the tile the robot is currently on, even while moving

	float width;
	float height;

	bool flip;	// the direction the sprite is looking in (false for left)

	Directions direction;

	float frameTime;

public:
	// constructor
	Robot(SDL_Renderer* rend, float x = START_XPOS_DEFAULT, float y = START_YPOS_DEFAULT, float w = ROBOT_WIDTH, float h = ROBOT_HEIGHT);

	// destructor
	~Robot();

	// getters //

	float getPosX() const { return posX; }
	float getPosY() const { return posY; }
	Tile*& getCurrentTile() { return currentTile; }

	// setters //

	void setCurrentTile(Tile*& current) { currentTile = current; }

	void updateFrameTime(float &time) { frameTime = time; }

	// what ever media we need is loaded in here
	bool loadMedia();

	TileAndDirection* move(Directions direction, Function* function, std::vector<Tile*>& tileSet);

	// render the robot to the screen
	void render(SDL_Rect* viewport, SDL_Rect &camera);

	//Center the camera over the player
	void centreCamera(SDL_Rect &camera);
};
