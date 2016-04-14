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

	window = SDL_CreateWindow("GAIMI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenSize.w, screenSize.h, SDL_WINDOW_FOREIGN); //SDL_WINDOW_FULLSCREEN_DESKTOP
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
}


void Gaimi::run()
{
	// while application is running
	while (!quit)
	{
		// calculate frame time
		frameTime = static_cast<float>(frameTimer.getTicks()) / 1000.0f;

		// restart frame timer
		frameTimer.start();

		if (createNewGame)
		{
			// create a game instance
			newGame = new Game(window, renderer, map);
			if (newGame == nullptr)
				break;

			createNewGame = false;
			runGame = true;

			delete welcome;
			welcome = nullptr;
		}

		if (runGame)
		{
			newGame->run(e, frameTime, quit);
		}
		// still on the welcome screen
		else
		{
			welcome->run(e, frameTime, quit, map);
			if (map != nullptr)
			{
				createNewGame = true;
			}			
		}
	}

	close();
}

// clean up all memory allocation and close down SDL systems
void Gaimi::close()
{
	if (welcome != nullptr)
		delete welcome;

	if (newGame != nullptr)
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