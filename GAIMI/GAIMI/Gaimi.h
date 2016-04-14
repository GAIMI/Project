#pragma once

#include <SDL.h>
#include "Globals.h"
#include "Game.h"
#include "WelcomeScreen.h"
#include "Gaimi.h"
#include "Timer.h"

class Gaimi
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	WelcomeScreen* welcome = nullptr;
	Game* newGame = nullptr;
	bool createNewGame = false;
	bool runGame = false;

	Maps* map = nullptr;

	//Main loop flag
	bool quit = false;

	// setup our frame time
	Timer frameTimer;
	float frameTime = 0;

	//Event handler
	SDL_Event e;

public:
	// constructor
	Gaimi();

	// destructor
	~Gaimi();

	// initialise all SDL systems, create the window and renderer
	bool init(const SDL_Rect& screenSize);

	void run();

	// clean up all memory allocation and close down SDL systems
	void close();
};
