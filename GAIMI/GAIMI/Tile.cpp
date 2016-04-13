
#include "Globals.h"
#include "Tile.h"


Tile::Tile(int x, int y, int tileType, int tileNum, Texture* tex) : 
	posX(x), posY(y), type(tileType), tile(tileNum), texture(tex)
{
	collisionBox.x = x;
	collisionBox.y = y;
	collisionBox.w = TILE_WIDTH;
	collisionBox.h = TILE_HEIGHT;
}

void Tile::renderTile(SDL_Rect &camera, SDL_Renderer *rend, SDL_Rect *clip, double dAngle, SDL_Point *centre, SDL_RendererFlip flip)
{
	// if inside viewing area
	if (checkCollision(camera))
	{
		texture->renderMedia(posX - camera.x, posY - camera.y, rend, &clip[type], dAngle, centre, flip);
	}
}

bool Tile::checkCollision(SDL_Rect &camera)
{
	int nLeftOfCollisionBox = collisionBox.x;
	int nRightOfCollisionBox = collisionBox.x + collisionBox.w;
	int nTopOfCollisionBox = collisionBox.y;
	int nBottomOfCollisionBox = collisionBox.y + collisionBox.h;

	int nLeftOfcamera = camera.x;
	int nRightOfcamera = camera.x + camera.w;
	int nTopOfcamera = camera.y;
	int nBottomOfcamera = camera.y + camera.h;

	if (((nBottomOfCollisionBox < nTopOfcamera) || (nTopOfCollisionBox > nBottomOfcamera) || 
		(nLeftOfCollisionBox > nRightOfcamera) || (nRightOfCollisionBox < nLeftOfcamera)) == false)
	{
		return true;
	}
	return false;
}