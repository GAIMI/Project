#pragma once

#include <SDL.h>
#include <string>
#include "Globals.h"
#include <vector>

class WelcomeScreen
{
private:
	SDL_Window* window;
	SDL_Renderer* renderer;

	SDL_Point touchLocation;	// also mouse pointer location

	SDL_Rect* viewportFull;

	Texture* background;
	Texture* startButton;
	Texture* title;
    Texture* exitButton;

	std::vector<MapTexture*> maps;

	int selectedX = 0;
	int selectedY = 0;

public:
	// constructor
	WelcomeScreen(SDL_Window* wind, SDL_Renderer* rend);

	// destructor
	~WelcomeScreen();

	bool loadMedia();

	// do welcome screen stuff
	void run(SDL_Event& e, float& frameTime, GameStates& state, Maps*& chosenMap);

	// handle all inputs (touch, mouse, keyboard etc)
	void processInputs(SDL_Event& e, float& frameTime, GameStates& state,
		const SDL_Rect& screenSize, SDL_Point& touchLocation, Maps*& chosenMap);
};
