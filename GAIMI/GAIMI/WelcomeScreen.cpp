#include "WelcomeScreen.h"


// constructor
WelcomeScreen::WelcomeScreen(SDL_Window* wind, SDL_Renderer* rend) :
	renderer(rend)
{
	// //create viewports
	viewportFull = new SDL_Rect({ 0, 0, SCREEN_SIZE.w, SCREEN_SIZE.h });

	//// load up the textures
	if (!loadMedia())
		throw;
}

// destructor
WelcomeScreen::~WelcomeScreen()
{
	background->free();
	startButton->free();
	title->free();
	selectedMap->free();

	for (auto it = maps.begin(); it != maps.end(); ++it)
	{
		(*it)->tex->free();
	}
}

bool WelcomeScreen::loadMedia()
{
	background = new Texture;
	startButton = new Texture;
	title = new Texture;
	selectedMap = new Texture;

	int startX = (SCREEN_SIZE.w / 2) - 200;
	int startY = (SCREEN_SIZE.h / 3) * 2;

	for (int i = 0; i < NUMBER_OF_MAPS; ++i)
	{
		maps.push_back(new MapTexture({
			new Texture,
			startX,
			startY
		}));

		startX += MAP_WIDTH + MAP_SPACING;

		if (!maps.at(i)->tex->loadFromFile(MAP_FILES[i], renderer))
			return false;
	}

	if (!background->loadFromFile(BACKGROUND_FILE, renderer))
		return false;

	if (!startButton->loadFromFile(STARTBUTTON_FILE, renderer))
		return false;

	if (!title->loadFromFile(TITLE_FILE, renderer))
		return false;

	if (!selectedMap->loadFromFile(SELECTED_FILE, renderer))
		return false;

	return true;
}

// do welcome screen stuff
std::string WelcomeScreen::run(SDL_Event& e, float& frameTime, bool& quit)
{
	//Clear screen
	SDL_RenderClear(renderer);

	// Render all media
	SDL_RenderSetViewport(renderer, viewportFull);
	background->renderMedia(0, 0, renderer);
	startButton->renderMedia(SCREEN_SIZE.w / 2 - (START_WIDTH / 2), SCREEN_SIZE.h / 2 - (START_HEIGHT / 2), renderer);
	title->renderMedia(SCREEN_SIZE.w / 2 - (TITLE_WIDTH / 2), 100, renderer);

	if (selectedX != 0 || selectedY != 0)
	{
		selectedMap->renderMedia(selectedX - 5, selectedY - 5, renderer);
	}

	for (auto it = maps.begin(); it != maps.end(); ++it)
	{
		(*it)->tex->renderMedia((*it)->x, (*it)->y, renderer);
	}

	//process inputs
	processInputs(e, frameTime, quit, SCREEN_SIZE, touchLocation);

	// Swap buffers
	SDL_RenderPresent(renderer);


	//NEEDS TO RETURN VARIABLE "selectedMapFileName"

	// if start is pressed
	if (startPressed)
		return selectedMapFileName;

	return "";
}

// handle all inputs (touch, mouse, keyboard etc)
void WelcomeScreen::processInputs(SDL_Event& event, float& frameTime, bool& quit, const SDL_Rect& screenSize, SDL_Point& touchLocation)
{
	//Handle events on queue
	while (SDL_PollEvent(&event) != 0)
	{
		//User requests quit
		if (event.type == SDL_QUIT)
		{
			quit = true;
		}

		// add mouse and touch input handlers here.....
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			touchLocation.x = event.button.x;
			touchLocation.y = event.button.y;

			// if within screen boundaries
			if (touchLocation.x > viewportFull->x && touchLocation.x < screenSize.w &&
				touchLocation.y > viewportFull->y && touchLocation.y < screenSize.h)
			{
				if (touchLocation.x > 810 && touchLocation.x < START_WIDTH + 810 &&
					touchLocation.y > 425 && touchLocation.y < START_HEIGHT + 425)
				{
					if (selectedMapFileName != "")
						startPressed = true;
				}

				//Loops through all buttons for maps
				for (auto it = maps.begin(); it != maps.end(); ++it)
				{
					if (touchLocation.x >(*it)->x && touchLocation.x < MAP_WIDTH + (*it)->x &&
						touchLocation.y >(*it)->y && touchLocation.y < MAP_HEIGHT + (*it)->y)
					{
						selectedMapFileName = (*it)->tex->getFileName();
						selectedX = (*it)->x;
						selectedY = (*it)->y;
					}
				}
			}
		} // end mousebuttondown
	} // end while pol event
}