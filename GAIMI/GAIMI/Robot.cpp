#include "Robot.h"


// constructor
Robot::Robot(SDL_Renderer* rend, float x, float y, float w, float h) : 
	renderer(rend), posX(x), posY(y), lastX(x), lastY(y), width(w), height(h), flip(false), 
	direction(Directions::EAST), frameTime(0.0f)
{
	loadMedia();
}

// destructor
Robot::~Robot()
{
	mySprite->free();
}

// what ever media we need is loaded in here
bool Robot::loadMedia()
{
	mySprite = new Texture;

	if (!mySprite->loadFromFile(ROBOT_TEX, renderer))
		return false;

	return true;
}

TileAndDirection* Robot::move(Directions dir, Function* function, std::vector<Tile*>& tileSet)
{
	direction = dir;

	switch (dir)
	{
	case Directions::NORTH:
		if (posY > lastY - ROBOT_MOVEMENT_DISTANCE)
			posY -= ROBOT_MOVEMENT_DISTANCE * frameTime;
		else
		{
			// set a new current tile for the robot
			setCurrentTile(tileSet[currentTile->getTileNumber() - (LEVEL_WIDTH / TILE_WIDTH)]);
			// update its position
			lastY = posY;
			// flag as complete
			function->complete = true;

			TileAndDirection* info = new TileAndDirection;
			info->tile = currentTile;
			info->dir = Directions::NORTH;
			return info;
		}	
		break;
	case Directions::SOUTH:
		if (posY < lastY + ROBOT_MOVEMENT_DISTANCE)
			posY += ROBOT_MOVEMENT_DISTANCE * frameTime;
		else
		{
			// set a new current tile for the robot
			setCurrentTile(tileSet[currentTile->getTileNumber() + (LEVEL_WIDTH / TILE_WIDTH)]);
			lastY = posY;
			function->complete = true;

			TileAndDirection* info = new TileAndDirection;
			info->tile = currentTile;
			info->dir = Directions::SOUTH;
			return info;
		}
		break;
	case Directions::EAST:
		if (posX < lastX + ROBOT_MOVEMENT_DISTANCE)
			posX += ROBOT_MOVEMENT_DISTANCE * frameTime;
		else
		{
			// set a new current tile for the robot
			setCurrentTile(tileSet[currentTile->getTileNumber() + 1]);
			lastX = posX;
			function->complete = true;

			TileAndDirection* info = new TileAndDirection;
			info->tile = currentTile;
			info->dir = Directions::EAST;
			return info;
		}
		break;
	case Directions::WEST:
		if (posX > lastX - ROBOT_MOVEMENT_DISTANCE)
			posX -= ROBOT_MOVEMENT_DISTANCE * frameTime;
		else
		{
			// set a new current tile for the robot
			setCurrentTile(tileSet[currentTile->getTileNumber() - 1]);
			lastX = posX;
			function->complete = true;

			TileAndDirection* info = new TileAndDirection;
			info->tile = currentTile;
			info->dir = Directions::WEST;
			return info;
		}
		break;
	default:
		break;
	}

	return nullptr;
}

// render the robot to the screen
void Robot::render(SDL_Rect* viewport, SDL_Rect &camera)
{
	// set viewport
	SDL_RenderSetViewport(renderer, viewport);

	//Render texture to screen
	if (direction == Directions::WEST)
	{
		flip = false;
		mySprite->renderMedia(static_cast<int>(posX - ROBOT_WIDTH / 2.0f) - camera.x, 
			static_cast<int>(posY - ROBOT_HEIGHT / 2.0f) - camera.y, renderer);
	}
	else if (direction == Directions::EAST)
	{
		flip = true;
		mySprite->renderMedia(static_cast<int>(posX - ROBOT_WIDTH / 2.0f) - camera.x, 
			static_cast<int>(posY - ROBOT_HEIGHT / 2.0f) - camera.y, renderer, 0, 0.0, 0, SDL_FLIP_HORIZONTAL);
	}
	else if (direction == Directions::NORTH)
	{
		if (flip)
			mySprite->renderMedia(static_cast<int>(posX - ROBOT_WIDTH / 2.0f) - camera.x, 
				static_cast<int>(posY - ROBOT_HEIGHT / 2.0f) - camera.y, renderer, 0, -90.0, 0, SDL_FLIP_HORIZONTAL);
		else
			mySprite->renderMedia(static_cast<int>(posX - ROBOT_WIDTH / 2.0f) - camera.x, 
				static_cast<int>(posY - ROBOT_HEIGHT / 2.0f) - camera.y, renderer, 0, 90.0);
	}
	else if (direction == Directions::SOUTH)
	{
		if (flip)
			mySprite->renderMedia(static_cast<int>(posX - ROBOT_WIDTH / 2.0f) - camera.x, 
				static_cast<int>(posY - ROBOT_HEIGHT / 2.0f) - camera.y, renderer, 0, 90.0, 0, SDL_FLIP_HORIZONTAL);
		else
			mySprite->renderMedia(static_cast<int>(posX - ROBOT_WIDTH / 2.0f) - camera.x, 
				static_cast<int>(posY - ROBOT_HEIGHT / 2.0f) - camera.y, renderer, 0, -90.0);
	}
}

//Center the camera over the player
void Robot::centreCamera(SDL_Rect &camera)
{

	camera.x = (static_cast<int>(posX + (width / 2.0f))) - camera.w / 2;
	camera.y = (static_cast<int>(posY + (height / 2.0f))) - camera.h / 2;


	//Keep the camera in bounds
	if (camera.x < 0)
	{
		camera.x = 0;
	}
	if (camera.y < 0)
	{
		camera.y = 0;
	}
	if (camera.x > LEVEL_WIDTH - camera.w)
	{
		camera.x = LEVEL_WIDTH - camera.w;
	}
	if (camera.y > LEVEL_HEIGHT - camera.h)
	{
		camera.y = LEVEL_HEIGHT - camera.h;
	}
}