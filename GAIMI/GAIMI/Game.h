#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <list>
#include "Texture.h"
#include "Robot.h"
#include "UI.h"

class Tile;


class Game
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Point touchLocation;	// also mouse pointer location

	SDL_Rect* viewportMain;
	SDL_Rect* miniMapViewport;

	SDL_Rect cameraMain;
	SDL_Rect cameraFullMap;


	std::vector<Tile*> tileSet;
	SDL_Rect tileClips[TOTAL_TILE_TYPES];

	std::string mapName;
	Texture* mapTexture;

	Texture* sceneTexture;	// for the mini map

	Robot* digger;
	UI* controls;

public:
	// constructor
	Game(SDL_Window* wind, SDL_Renderer* rend, std::string& mapName);

	// destructor
	~Game();

	// getters //

	Texture* getTexture() { return mapTexture; }

	// setters //

	bool loadMap(std::string image);

	// create bounding volumes for each tile and allocate a type type
	bool setTiles();

	// setup our scene
	bool createGameObjects();

	// run the game until a call to exit
	void run(SDL_Event& e, float& frameTime, bool& quit);

	// handle all inputs (touch, mouse, keyboard etc)
	void processInputs(SDL_Event& e, float& frameTime, const SDL_Rect& screenSize, SDL_Point& touchLocation, bool& quit);

	// checks if the go button is pressed and goes through the function list
	void processFunctions();

	void miniMap();
};
