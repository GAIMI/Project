#pragma once

#include <SDL.h>
#include "Texture.h"
#include "Globals.h"

class Tile
{
private:
	int posX, posY;

	SDL_Rect collisionBox;

	Texture* texture;

	int type;	// the tile code
	int tile;	// the tile number in sequence


public:
	Tile(int x, int y, int tileType, int tile, Texture* tex);

	int getPosX() const { return posX; }
	int getPosY() const { return posY; }
	SDL_Rect getCollisionBox() const { return collisionBox; }
	Texture* getTexture() const { return texture; }
	int getType() const { return type; }
	int getTileNumber() const { return tile; }

	void setType(int tileType) { type = tileType; }

	void renderTile(SDL_Rect &camera, SDL_Renderer *rend, SDL_Rect *clip, double dAngle = 0.0, SDL_Point *centre = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
	bool checkCollision(SDL_Rect &camera);
};