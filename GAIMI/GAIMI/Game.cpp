#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include "Tile.h"
#include "Game.h"
#include "Globals.h"


// constructor
Game::Game(SDL_Window* wind, SDL_Renderer* rend, std::string& map) :
	window(wind), renderer(rend), mapName(map)
{
	// set default touch location to the centre of the screen
	touchLocation.x = SCREEN_SIZE.w / 2;
	touchLocation.y = SCREEN_SIZE.h / 2;

	// create map viewport
	viewportMain = new SDL_Rect(UI_MAIN);	// simulation area
	miniMapViewport = new SDL_Rect{ UI_MAIN.w - 200, 0, 200, 200 };
	cameraFullMap = { 0, 0, LEVEL_WIDTH, LEVEL_HEIGHT };

	//create camera to show only part of the map within viewportMain
	cameraMain = UI_MAIN;

	if (!loadMap(mapName))
		throw;

	// the level tiles	
	if (!setTiles())
		throw;

	// setup our simulation robot
	if (!createGameObjects())
		throw;

	//Load texture target
	sceneTexture = new Texture;
	if (!sceneTexture->createBlank(rend, LEVEL_WIDTH, LEVEL_HEIGHT, SDL_TEXTUREACCESS_TARGET))
		throw;
}

// destructor
Game::~Game()
{
	delete digger;
	delete controls;

	if (mapTexture != nullptr)
		mapTexture->free();
}

// load the map into memory
bool Game::loadMap(std::string image)
{
	mapTexture = new Texture;

	//Load sprite sheet texture
	if (!mapTexture->loadFromFile(image.c_str(), renderer))
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		renderer = nullptr;
		window = nullptr;
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	return true;
}

// create bounding volumes for each tile and allocate a type type
bool Game::setTiles()
{
	//The tile offsets
	int x = 0;
	int y = 0;

	//Open the map
	std::ifstream map(MAP_1_MAP);

	//If the map couldn't be loaded
	if (!map)
	{
		std::cout << "Unable to load map file!" << std::endl;
		return false;
	}
	else
	{
		//Initialize the tiles
		for (int tileNum = 0; tileNum < TOTAL_TILES; ++tileNum)
		{
			//Determines what kind of tile will be made
			int tileType = -1;

			//Read tile from map file
			map >> tileType;

			//If the was a problem in reading the map
			if (map.fail())
			{
				//Stop loading map
				std::cout << "Error loading map: Unexpected end of file!" << std::endl;
				return false;
			}

			//If the number is a valid tile number
			if ((tileType >= 0) && (tileType < TOTAL_TILE_TYPES))
			{
				tileSet.push_back(new Tile(x, y, tileType, tileNum, mapTexture));
			}
			//If we don't recognize the tile type
			else
			{
				//Stop loading map
				std::cout << "Error loading map: Invalid tile type at: " << tileNum << std::endl;
				return false;
			}
			//Move to next tile spot
			x += TILE_WIDTH;

			//If we've gone too far
			if (x >= LEVEL_WIDTH)
			{
				//Move back
				x = 0;

				//Move to the next row
				y += TILE_HEIGHT;
			}
		}
		// clip the sprite sheet //
		{
			tileClips[PASSABLE].x = 0;
			tileClips[PASSABLE].y = 0;
			tileClips[PASSABLE].w = TILE_WIDTH;
			tileClips[PASSABLE].h = TILE_HEIGHT;

			tileClips[CRATE].x = 200;
			tileClips[CRATE].y = 0;
			tileClips[CRATE].w = TILE_WIDTH;
			tileClips[CRATE].h = TILE_HEIGHT;

			tileClips[BRIDGE_CRATE].x = 400;
			tileClips[BRIDGE_CRATE].y = 0;
			tileClips[BRIDGE_CRATE].w = TILE_WIDTH;
			tileClips[BRIDGE_CRATE].h = TILE_HEIGHT;

			tileClips[SURVIVOR].x = 600;
			tileClips[SURVIVOR].y = 0;
			tileClips[SURVIVOR].w = TILE_WIDTH;
			tileClips[SURVIVOR].h = TILE_HEIGHT;

			tileClips[DIG_SITE].x = 800;
			tileClips[DIG_SITE].y = 0;
			tileClips[DIG_SITE].w = TILE_WIDTH;
			tileClips[DIG_SITE].h = TILE_HEIGHT;

			tileClips[BRIDGE_VERT].x = 1000;
			tileClips[BRIDGE_VERT].y = 0;
			tileClips[BRIDGE_VERT].w = TILE_WIDTH;
			tileClips[BRIDGE_VERT].h = TILE_HEIGHT;

			tileClips[BRIDGE_HORIZ].x = 1200;
			tileClips[BRIDGE_HORIZ].y = 0;
			tileClips[BRIDGE_HORIZ].w = TILE_WIDTH;
			tileClips[BRIDGE_HORIZ].h = TILE_HEIGHT;

			tileClips[BRIDGE_CAP_LEFT].x = 1400;
			tileClips[BRIDGE_CAP_LEFT].y = 0;
			tileClips[BRIDGE_CAP_LEFT].w = TILE_WIDTH;
			tileClips[BRIDGE_CAP_LEFT].h = TILE_HEIGHT;

			tileClips[BRIDGE_CAP_TOP].x = 1600;
			tileClips[BRIDGE_CAP_TOP].y = 0;
			tileClips[BRIDGE_CAP_TOP].w = TILE_WIDTH;
			tileClips[BRIDGE_CAP_TOP].h = TILE_HEIGHT;

			tileClips[BRIDGE_CAP_RIGHT].x = 1800;
			tileClips[BRIDGE_CAP_RIGHT].y = 0;
			tileClips[BRIDGE_CAP_RIGHT].w = TILE_WIDTH;
			tileClips[BRIDGE_CAP_RIGHT].h = TILE_HEIGHT;

			tileClips[BRIDGE_CAP_BOTTOM].x = 2000;
			tileClips[BRIDGE_CAP_BOTTOM].y = 0;
			tileClips[BRIDGE_CAP_BOTTOM].w = TILE_WIDTH;
			tileClips[BRIDGE_CAP_BOTTOM].h = TILE_HEIGHT;

			tileClips[CANYON_CAP_LEFT].x = 2200;
			tileClips[CANYON_CAP_LEFT].y = 0;
			tileClips[CANYON_CAP_LEFT].w = TILE_WIDTH;
			tileClips[CANYON_CAP_LEFT].h = TILE_HEIGHT;

			tileClips[CANYON_CAP_TOP].x = 2400;
			tileClips[CANYON_CAP_TOP].y = 0;
			tileClips[CANYON_CAP_TOP].w = TILE_WIDTH;
			tileClips[CANYON_CAP_TOP].h = TILE_HEIGHT;

			tileClips[CANYON_CAP_RIGHT].x = 2600;
			tileClips[CANYON_CAP_RIGHT].y = 0;
			tileClips[CANYON_CAP_RIGHT].w = TILE_WIDTH;
			tileClips[CANYON_CAP_RIGHT].h = TILE_HEIGHT;

			tileClips[CANYON_CAP_BOTTOM].x = 2800;
			tileClips[CANYON_CAP_BOTTOM].y = 0;
			tileClips[CANYON_CAP_BOTTOM].w = TILE_WIDTH;
			tileClips[CANYON_CAP_BOTTOM].h = TILE_HEIGHT;

			tileClips[CANYON_CORNER_BOT_LEFT].x = 3000;
			tileClips[CANYON_CORNER_BOT_LEFT].y = 0;
			tileClips[CANYON_CORNER_BOT_LEFT].w = TILE_WIDTH;
			tileClips[CANYON_CORNER_BOT_LEFT].h = TILE_HEIGHT;

			tileClips[CANYON_CORNER_TOP_LEFT].x = 3200;
			tileClips[CANYON_CORNER_TOP_LEFT].y = 0;
			tileClips[CANYON_CORNER_TOP_LEFT].w = TILE_WIDTH;
			tileClips[CANYON_CORNER_TOP_LEFT].h = TILE_HEIGHT;

			tileClips[CANYON_CORNER_TOP_RIGHT].x = 3400;
			tileClips[CANYON_CORNER_TOP_RIGHT].y = 0;
			tileClips[CANYON_CORNER_TOP_RIGHT].w = TILE_WIDTH;
			tileClips[CANYON_CORNER_TOP_RIGHT].h = TILE_HEIGHT;

			tileClips[CANYON_CORNER_BOT_RIGHT].x = 3600;
			tileClips[CANYON_CORNER_BOT_RIGHT].y = 0;
			tileClips[CANYON_CORNER_BOT_RIGHT].w = TILE_WIDTH;
			tileClips[CANYON_CORNER_BOT_RIGHT].h = TILE_HEIGHT;

			tileClips[CANYON_VERT].x = 3800;
			tileClips[CANYON_VERT].y = 0;
			tileClips[CANYON_VERT].w = TILE_WIDTH;
			tileClips[CANYON_VERT].h = TILE_HEIGHT;

			tileClips[CANYON_HORIZ].x = 4000;
			tileClips[CANYON_HORIZ].y = 0;
			tileClips[CANYON_HORIZ].w = TILE_WIDTH;
			tileClips[CANYON_HORIZ].h = TILE_HEIGHT;
		}
	}

	//Close the file
	map.close();

	//If the map was loaded fine
	return true;
}

// setup our simulation robot
bool Game::createGameObjects()
{
	digger = new Robot(renderer);
	controls = new UI(renderer, viewportMain, tileSet);

	// error control
	if (digger == nullptr || controls == nullptr)
		return false;

	// work out which tile the robot is on by its x and y coordinates
	int tile = (static_cast<int>(digger->getPosX()) / TILE_WIDTH) +
		(LEVEL_WIDTH / TILE_WIDTH) * (static_cast<int>(digger->getPosY()) / TILE_WIDTH);

	digger->centreCamera(cameraMain);	// reposition the camera so it looks at the robot
	digger->setCurrentTile(tileSet[tile]);	// where the robot is at all times								

	controls->setCurrentTile(tileSet[tile]);	// whre the robot will end up after the current string is executed
	controls->setRobotTile(tileSet[tile]);	// our backup if we need to back track

	return true;
}

// run the game until a call to exit
void Game::run(SDL_Event& e, float& frameTime, bool& quit)
{
	// set the screen colour
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	//Clear screen
	SDL_RenderClear(renderer);

	//Render level
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		SDL_RenderSetViewport(renderer, viewportMain);
		tileSet[i]->renderTile(cameraMain, renderer, tileClips);
	}

	// handle all inputs (touch, mouse, keyboard etc)
	processInputs(e, frameTime, SCREEN_SIZE, touchLocation, quit);

	digger->updateFrameTime(frameTime);

	// check for the goButton press
	processFunctions();

	// centre the camera on the robot only when operating the function string
	if (controls->getGoButtonPressed())
	{
		// reposition the camera so it always looks at the robot
		digger->centreCamera(cameraMain);
	}

	// render objects
	controls->render(touchLocation, cameraMain);
	digger->render(viewportMain, cameraMain);
	controls->renderText();
	controls->renderScoreScreen();

	// render mini map
	miniMap();

	//Update screen
	SDL_RenderPresent(renderer);
}

// handle all inputs (touch, mouse, keyboard etc)
void Game::processInputs(SDL_Event& event, float& frameTime, const SDL_Rect& screenSize, SDL_Point& touchLocation, bool& quit)
{
	//Handle events on queue
	while (SDL_PollEvent(&event) != 0)
	{
		//User requests quit
		if (event.type == SDL_QUIT)
		{
			quit = true;
		}

		// only allow control of the UI when preparing a function string, not during its operation
		if (!controls->getGoButtonPressed() && !controls->getEndOfMission() && !controls->getOkActive())
		{
			controls->setCurrentStage(stage);
			controls->mouseInputHandler(event, frameTime, touchLocation, cameraMain);
			controls->touchInputHandler(event, frameTime, touchLocation, cameraMain, SCREEN_SIZE);
		}
		else if (controls->getOkActive())
		{
			controls->okButtonActiveOnly(event, frameTime, touchLocation, cameraMain, SCREEN_SIZE);
		}
	}
}

// checks if the go button is pressed and goes through the function list
void Game::processFunctions()
{
	if (controls->getGoButtonPressed()) // == true
	{
		std::list<Function*> functions = controls->getFunctionsList();

		int functionCount = 0;	// completed functions

		for (auto function = functions.begin(); function != functions.end(); ++function)
		{
			std::string fileName = (*function)->tex->getFileName();

			// we iterate through the list until we find the next uncompleted function
			if ((*function)->complete)
			{
				++functionCount;
				if (functionCount == functions.size())
				{
					controls->setGoButtonPressed(false);
					controls->resetRouteMap();
					controls->resetFunctions();
				}

				continue;
			}

			TileAndDirection* info = nullptr;

			// we do one frame of the current function then break out of the loop
			if (fileName == FUNCTION_1)
			{
				info = digger->move(Directions::NORTH, *function, tileSet);
			}
			else if (fileName == FUNCTION_2)
			{
				info = digger->move(Directions::SOUTH, *function, tileSet);
			}
			else if (fileName == FUNCTION_3)
			{
				info = digger->move(Directions::WEST, *function, tileSet);
			}
			else if (fileName == FUNCTION_4)
			{
				info = digger->move(Directions::EAST, *function, tileSet);
			}
			else if (fileName == FUNCTION_5)
			{
				// dig sample
				(*function)->complete = true;
			}

			if (info != nullptr)
				controls->processInventory(info);

			break;
		}
		// set the start position for the next function string
		controls->setRobotTile(controls->getCurrentTile());
	}
}

void Game::miniMap()
{
	// set the render target to a texture
	sceneTexture->setAsRenderTarget(renderer);

	// render the tiles to the texture
	for (int i = 0; i < TOTAL_TILES; ++i)
	{
		tileSet[i]->renderTile(cameraFullMap, renderer, tileClips);
	}

	// render the robots location on the mini map as a green square
	SDL_Rect icon = digger->getCurrentTile()->getCollisionBox();
	icon.x -= 15;
	icon.y -= 15;
	icon.w += 30;
	icon.h += 30;

	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	SDL_RenderFillRect(renderer, &icon);

	// reset the render target back to the back buffer
	SDL_SetRenderTarget(renderer, NULL);

	// apply transparency
	sceneTexture->setBlendMode(SDL_BLENDMODE_BLEND);
	int success = sceneTexture->setAplha(150);

	// set the viewport to the main area
	SDL_RenderSetViewport(renderer, viewportMain);

	// render the texture to the viewport
	SDL_RenderCopy(renderer, sceneTexture->getTexture(), 0, miniMapViewport);

	// draw a border around the mini map
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawRect(renderer, miniMapViewport);
}