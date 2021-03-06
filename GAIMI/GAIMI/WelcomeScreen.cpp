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
    title->free();
    exitButton->free();
    creditsButton->free();
    credits->free();

	for (auto it = maps.begin(); it != maps.end(); ++it)
	{
		(*it)->tex->free();
	}
}

bool WelcomeScreen::loadMedia()
{
	background = new Texture;
	title = new Texture;
    exitButton = new Texture;
    creditsButton = new Texture;
    credits = new Texture;

	int startX = 200;
	int startY = 450;

	for (int i = 0; i < NUMBER_OF_MAPS; i++)
	{
		maps.push_back(new MapTexture({
			new Texture,
			startX,
			startY
		}));

        if (i == 0)
        {
            startY += HEIGHT_OFFSET;
        }
        else
        {
            startY = 450;
        }

        startX += MAP_WIDTH + MAP_SPACING;      

		if (!maps.at(i)->tex->loadFromFile(MAPS[i].mapFile, renderer))
			return false;
	}

	if (!background->loadFromFile(BACKGROUND_FILE, renderer))
		return false;

	if (!title->loadFromFile(TITLE_FILE, renderer))
		return false;

    if (!exitButton->loadFromFile(EXIT_BUTTON_FILE, renderer))
        return false;

    if (!creditsButton->loadFromFile(CREDITS_BUTTON_FILE, renderer))
        return false;

    if (!credits->loadFromFile(CREDITS_FILE, renderer))
        return false;


	return true;
}

// do welcome screen stuff
void WelcomeScreen::run(SDL_Event& e, float& frameTime, GameStates& state, Maps*& chosenMap)
{
	//Clear screen
	SDL_RenderClear(renderer);

	//process inputs
	processInputs(e, frameTime, state, SCREEN_SIZE, touchLocation, chosenMap);

	// Render all media
	SDL_RenderSetViewport(renderer, viewportFull);

    if (!showCredits)
    {
        background->renderMedia(0, 0, renderer);

    }
    else
    {
        credits->renderMedia(0, 0, renderer);
    }
	
	title->renderMedia(SCREEN_SIZE.w / 2 - (TITLE_WIDTH / 2), 100, renderer);
    exitButton->renderMedia(20, SCREEN_SIZE.h - 80, renderer);
    creditsButton->renderMedia(CREDITS_BUTTON.x, CREDITS_BUTTON.y, renderer);

    if (!showCredits)
    {
        for (auto it = maps.begin(); it != maps.end(); ++it)
        {
            (*it)->tex->renderMedia((*it)->x, (*it)->y, renderer);
        }
    }
	

	// Swap buffers
	SDL_RenderPresent(renderer);
}

// handle all inputs (touch, mouse, keyboard etc)
void WelcomeScreen::processInputs(SDL_Event& event, float& frameTime, GameStates& state, 
	const SDL_Rect& screenSize, SDL_Point& touchLocation, Maps*& chosenMap)
{
	//Handle events on queue
	while (SDL_PollEvent(&event) != 0)
	{
		//User requests quit
		if (event.type == SDL_QUIT)
		{
			state = GameStates::QUIT;
		}

		// add mouse and touch input handlers here.....
		if (event.type == SDL_MOUSEBUTTONDOWN)
		{
			touchLocation.x = event.button.x;
			touchLocation.y = event.button.y;

            if (!showCredits)
            {
                //Loops through all buttons for maps
                for (auto it = maps.begin(); it != maps.end(); ++it)
                {
                    if (touchLocation.x > (*it)->x + COLLISION_OFFSET
                        && touchLocation.x < MAP_WIDTH + (*it)->x - COLLISION_OFFSET
                        && touchLocation.y >(*it)->y + COLLISION_OFFSET
                        && touchLocation.y < MAP_HEIGHT + (*it)->y - COLLISION_OFFSET)
                    {
                        for (Maps map : MAPS)
                        {
                            if ((*it)->tex->getFileName() == map.mapFile)
                            {
                                chosenMap = new Maps;
                                chosenMap->first = map.first;
                                chosenMap->mapFile = map.mapFile;
                                chosenMap->mapMap = map.mapMap;
                                chosenMap->missionScript = map.missionScript;
                            }
                        }
                    }
                } // end of iterator
            }  // if !show credits

            //Exit button
            if (touchLocation.x > viewportFull->x + 20 &&
                touchLocation.x < viewportFull->x + 20 + EXIT_BUTTON.w &&
                touchLocation.y > viewportFull->y + SCREEN_SIZE.h - 80 &&
                touchLocation.y < viewportFull->y + SCREEN_SIZE.h - 80 + EXIT_BUTTON.h)
            {
                //Exit    
				state = GameStates::QUIT;
            }

            //credits button
            if (touchLocation.x > CREDITS_BUTTON.x
                && touchLocation.x < CREDITS_BUTTON.w + CREDITS_BUTTON.x
                && touchLocation.y >CREDITS_BUTTON.y
                && touchLocation.y < CREDITS_BUTTON.h + CREDITS_BUTTON.y)
            {
                showCredits = !showCredits;
            }


		} // end mousebuttondown
	} // end while pol event
}