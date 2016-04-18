#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "WelcomeScreen.h"
#include "Gaimi.h"
#include "Timer.h"

// constructor
Gaimi::Gaimi()
{
	// intialise the game instance
	if (!init(SCREEN_SIZE))
		throw;

	// create a welcome screen
	welcome = new WelcomeScreen(window, renderer);
	if (welcome == nullptr)
		throw;
}

// destructor
Gaimi::~Gaimi()
{
}

// initialise all SDL systems, create the window and renderer
bool Gaimi::init(const SDL_Rect& screenSize)
{
	// initialise SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		return false;
	}

	// initialise SDL_image
	int initFlag = IMG_INIT_PNG | IMG_INIT_JPG;
	if ((IMG_Init(initFlag) & initFlag) != initFlag)
	{
		SDL_Quit();
		return false;
	}

	if (TTF_Init() == -1)
	{
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	window = SDL_CreateWindow("GAIMI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenSize.w, screenSize.h, SDL_WINDOW_FULLSCREEN);	// width and height not used when set to fullscreen
	if (window == nullptr)
	{
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr)
	{
		SDL_DestroyWindow(window);
		window = nullptr;
		TTF_Quit();
		IMG_Quit();
		SDL_Quit();
		return false;
	}
	return true;

	SDL_RenderSetLogicalSize(renderer, 1920, 1080);
}


void Gaimi::run()
{
	// while application is running
	while (state != GameStates::QUIT)
	{
		// calculate frame time
		frameTime = static_cast<float>(frameTimer.getTicks()) / 1000.0f;

		// restart frame timer
		frameTimer.start();

		if (state == GameStates::CREATE_GAME || state == GameStates::NEXT_MISSION || state == GameStates::RESTART_MISSION)
		{
			// if we're moving on to the next mission, a game already exists so we need to clean that up first
			if (newGame != nullptr)
			{
				// delete old game
				delete newGame;
				newGame = nullptr;

				// change the map to the next map
				bool nextMap = false;

				// only change the map if we're not restarting
				if (state != GameStates::RESTART_MISSION)
				{
					for (Maps map : MAPS)
					{
						if (nextMap || currentMap == nullptr)
						{
							nextMap = false;

							if (currentMap == nullptr)
							{
								currentMap = new Maps;
							}

							currentMap->first = map.first;
							currentMap->mapFile = map.mapFile;
							currentMap->mapMap = map.mapMap;
							currentMap->missionScript = map.missionScript;

							state = GameStates::CREATE_GAME;

							break;
						}

						if (currentMap->mapFile == map.mapFile)
						{
							nextMap = true;
						}
					}
				}
				else
					state = GameStates::CREATE_GAME;

				// if we just completed the last mission
				if (nextMap)
				{
					state = GameStates::CREATE_MENU;
				}
			}	// end if newGame exists

			if (state == GameStates::CREATE_GAME)
			{
				// create a game instance
				newGame = new Game(window, renderer, currentMap);
				if (newGame == nullptr)
					break;

				delete welcome;
				welcome = nullptr;

				state = GameStates::RUN_GAME;
			}
		}	// end create game
		else if (state == GameStates::BACK_TO_MENU)
		{
			state = GameStates::CREATE_MENU;
		}


		if (state == GameStates::CREATE_MENU)
		{

			// delete old game
			delete newGame;
			newGame = nullptr;

			// delete old map
			delete currentMap;
			currentMap = nullptr;


			welcome = new WelcomeScreen(window, renderer);
			state = GameStates::RUN_MENU;
		}

		if (state == GameStates::RUN_GAME)
		{
			newGame->run(e, frameTime, state);
		}
		// still on the welcome screen
		else if (state == GameStates::RUN_MENU)
		{
			welcome->run(e, frameTime, state, currentMap);
			if (currentMap != nullptr)
			{
				state = GameStates::CREATE_GAME;
			}			
		}
	}

	close();
}

// clean up all memory allocation and close down SDL systems
void Gaimi::close()
{
	delete welcome;
	delete newGame;

	// clean up renderer and window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	renderer = nullptr;
	window = nullptr;

	// unload SDL and subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}