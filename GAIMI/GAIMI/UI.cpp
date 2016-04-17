#include "UI.h"
#include <string>


// constructor
UI::UI(SDL_Renderer* rend, SDL_Rect* viewportMain, std::vector<Tile*> tileSet, Maps* map) :
    renderer(rend), textRead(0), scriptOverridden(false), currentText(""), viewportMain(viewportMain),
    templatePosX(0), templatePosY(0), useBoxPos(false), listPosition(0), numBoxes(1), gapVisible(true),
    goButtonPressed(false), functionPickedUp(nullptr), funcStringBoxIndex(-1), tempBoxCreated(nullptr),
    tempIndex(-1), currentTile(nullptr), tiles(tileSet), codeToRender(0), map(map)
{
    // create viewports
    viewportFull = new SDL_Rect({ 0, 0, SCREEN_SIZE.w, SCREEN_SIZE.h });	// full screen
    viewportLeft = new SDL_Rect(UI_LEFT);									// left UI panel
    viewportBottom = new SDL_Rect(UI_BOTTOM);								// bottom UI panel

	// load up the textures
    if (!loadMedia())
        throw;

    // load TTF font
    font = TTF_OpenFont(TTF_FILE.c_str(), 36);
    textColor = { 0, 0, 0 };

    // Loads script file line by line into the vector
    LoadScript(map->missionScript, missionScript);
    getNextLine();

	previousStage = map->first;
	currentStage = map->first;


    // left UI //

    // 20 x 400 first empty inventory box

    // create function button positions	
    for (int i = 0; i < NUM_FUNCTIONS / 2; ++i)	// num of rows
    {
        for (int j = 0; j < 2; ++j)	// num of columns
        {
            functionHitBoxes.push_back(new SDL_Rect{
                FUNCTION_SPACING_LEFT + (j * (FUNCTION_WIDTH + FUNCTION_SPACING_LEFT)),
                FUNCTION_SPACING_LEFT + (i * (FUNCTION_HEIGHT + FUNCTION_SPACING_LEFT)),
                FUNCTION_WIDTH, FUNCTION_HEIGHT });
        }
    }
    // if the number of functions is odd
    if (NUM_FUNCTIONS % 2)
    {
        functionHitBoxes.push_back(new SDL_Rect{
            80,
            FUNCTION_SPACING_LEFT + ((NUM_FUNCTIONS / 2) * (FUNCTION_HEIGHT + FUNCTION_SPACING_LEFT)),
            FUNCTION_WIDTH, FUNCTION_HEIGHT });
    }

    // create the vector of function names
    functionFilenames.push_back(FUNCTION_1);
    functionFilenames.push_back(FUNCTION_2);
    functionFilenames.push_back(FUNCTION_3);
    functionFilenames.push_back(FUNCTION_4);
    functionFilenames.push_back(FUNCTION_5);


    // bottom UI //

    // create function string button positions
    for (int i = 0; i < NUM_FUNC_STRING_BOXES; ++i)
    {
        SDL_Rect newBox = { BOX_START_X_POS + (i * (FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM)),
            SCREEN_SIZE.h - UI_BOTTOM_HEIGHT + FUNCTION_SPACING_BOTTOM, FUNCTION_WIDTH, FUNCTION_HEIGHT };
        functionStringHitBoxes.push_back(newBox);
    }

    // add the first empty box
    Texture* emptyBox = new Texture;
    emptyBox->loadFromFile(EMPTY_BOX, renderer);
    functionStringBoxes.push_back(new FunctionStringBox{ functionStringHitBoxes[0], emptyBox, nullptr });
}

// destructor
UI::~UI()
{
    leftPanel->free();
    bottomPanel->free();
    listBackButton->free();
    listForwardButton->free();
    goButton->free();
    OkButton->free();
    trashcan->free();
    openTrashcan->free();
    exitButton->free();
	resetButton->free();

    resetRouteMap();

    if (functionTemplate != nullptr)
        functionTemplate->free();

    if (speechBubble != nullptr)
        speechBubble->free();

	if (drOgel != nullptr)
		drOgel->free();

    // clean up text textures
    for (auto it = textLines.begin(); it != textLines.end(); ++it)
    {
        (*it)->free();
    }

    // clean up the textures in the left UI panel
    for (auto it = functions.begin(); it != functions.end(); ++it)
    {
        (*it)->tex->free();
    }

    // clean up the textures in the bottom UI panel
    for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
    {
        (*it)->emptyBox->free();

        if ((*it)->function != nullptr)
            (*it)->function->tex->free();
    }

    //clean up stars
    for (auto it = blankStars.begin(); it != blankStars.end(); ++it)
    {
        (*it)->free();
    }

    for (auto it = stars.begin(); it != stars.end(); ++it)
    {
        (*it)->free();
    }
}

// create a new empty box to hold a function after a placing a function
void UI::createFuncStringBox()
{
    if (!gapVisible)
    {
        ++listPosition;

        // reposition the functions and the boxes
        for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
        {
            (*it)->position.x -= FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM;
        }
    }

    // add a new empty box
    Texture* emptyBox = new Texture;
    emptyBox->loadFromFile(EMPTY_BOX, renderer);
    functionStringBoxes.push_back(new FunctionStringBox{ functionStringHitBoxes[numBoxes - listPosition], emptyBox, nullptr });
    ++numBoxes;

    createInsertBar(numBoxes - 3, numBoxes - 2);
}

// insert a new empty box to hold a function when hovering over a box bar
void UI::insertFuncStringBox(int index)
{
    // add a new empty box
    Texture* emptyBox = new Texture;
    emptyBox->loadFromFile(EMPTY_BOX, renderer);

    std::list<FunctionStringBox*>::iterator boxPos;
    boxPos = functionStringBoxes.begin();
    for (int i = 0; i < index; ++i)
        ++boxPos;

    functionStringBoxes.insert(boxPos, new FunctionStringBox{ functionStringHitBoxes[index - listPosition], emptyBox, nullptr });
    ++numBoxes;

    createInsertBar(numBoxes - 3, numBoxes - 2);

    // reposition the functions and the boxes past it
    int boxIndex = 0;
    for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
    {
        if (boxIndex == index)
            tempBoxCreated = *it;

        if (boxIndex > index)
            (*it)->position.x += FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM;

        ++boxIndex;
    }
}

// create a bar between placed functions that will allow insertion of functions between them
void UI::createInsertBar(int prevIndex, int nextIndex)
{
    Texture* boxBar = new Texture;
    boxBar->loadFromFile(BOX_BAR, renderer);
    boxBars.push_back(new BoxBar{ boxBar, prevIndex, nextIndex });
}

// trace the route on the map checking that the current moves are valid
bool UI::isValidMove(Texture* newFunction)
{
    std::string fileName = newFunction->getFileName();
    int current = currentTile->getTileNumber();
    int next = 0;

    if (fileName == FUNCTION_1)	// move up
    {
        next = current - (LEVEL_WIDTH / TILE_WIDTH);

        if (next < 0 || !checkTiles(next))
            return false;
    }
    else if (fileName == FUNCTION_2)	// move down
    {
        next = current + (LEVEL_WIDTH / TILE_WIDTH);

        if (next >= TOTAL_TILES || !checkTiles(next))
            return false;
    }
    else if (fileName == FUNCTION_3)	// move left
    {
        next = current - 1;

        if (currentTile->getPosX() <= 0 || !checkTiles(next))
            return false;
    }
    else if (fileName == FUNCTION_4)	// move right
    {
        next = current + 1;

        if (currentTile->getPosX() >= LEVEL_WIDTH - TILE_WIDTH || !checkTiles(next))
            return false;
    }
    else if (fileName == FUNCTION_5)
    {
        next = current;
        // dig sample
    }
    // update the current tile
    currentTile = tiles[next];

    return true;
}

// checks for valid tile moves
bool UI::checkTiles(int tileIndex)
{
    // iterate the array
    for (Tile* tile : tiles)
    {
        // get tile type of tile at tileIndex
        if (tile->getTileNumber() == tileIndex)
        {
            // check state
            switch (tile->getType())
            {
            case CANYON_CORNER_BOT_LEFT:
            case CANYON_CORNER_TOP_LEFT:
            case CANYON_CORNER_TOP_RIGHT:
            case CANYON_CORNER_BOT_RIGHT:
            {
                return false;
            }
            case CANYON_CAP_LEFT:
            case CANYON_CAP_TOP:
            case CANYON_CAP_RIGHT:
            case CANYON_CAP_BOTTOM:
            case CANYON_VERT:
            case CANYON_HORIZ:
            {
                bool bridgeAvail = false;
                // check our inventory to make sure we have bridges and that they are not all used already
                for (InventoryItem* item : inventoryItems)
                {
                    if (item->tex->getFileName() == BRIDGE_ICON && bridgesLeft > 0)
                    {
                        // we we have a spare bridge, use it and return true to say we can go there
                        --bridgesLeft;
                        bridgeAvail = true;
                        break;
                    }
                }
                if (!bridgeAvail)
                    return false;
            }
            default:
                break;
            }

            // create an overlay in the next movement location
            RouteOverlay* routeOverlay = new RouteOverlay;
            routeOverlay->tex = new Texture;
            routeOverlay->tex->loadFromFile(TILE_OVERLAY_BLUE, renderer);
            routeOverlay->location.x = tile->getPosX();
            routeOverlay->location.y = tile->getPosY();
            routeTiles.push_back(routeOverlay);

            break;
        }
    }

    return true;
}

// when a function is deleted or inserted, re-evaluate the whole string of functions
bool UI::checkNewFuncString()
{
    // set the current tile back to the start of the string (on the robot)
    currentTile = robotTile;

    resetRouteMap();

    bridgesLeft = bridges;

    // iterate the functions list after an insertion or deletion and check if the string of functions is still valid
    for (Function* func : functions)
    {
        // if any function is invalid return false, cancelling the remainder of the insert or delete and triggering a rollback
        if (!isValidMove(func->tex))
            return false;
    }

    return true;
}

// delete all the route overlays ready for a fresh string
void UI::resetRouteMap()
{
    // clear the route map
    for (RouteOverlay* tile : routeTiles)
    {
        tile->tex->free();
    }
    routeTiles.clear();
}

// reset the completed flag back to false on all functions
void UI::resetFunctions()
{
    // keep string after execution //

    //for (auto it = functions.begin(); it != functions.end(); ++it)
    //{
    //	(*it)->complete = false;
    //}

    // clear the string after execution //

    // free all unwanted textures
    for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
    {
        FunctionStringBox* box = *it;	// simplify the syntax

        if (box->function != nullptr)
        {
            box->emptyBox->free();
            box->function->tex->free();
        }
    }
    // delete all functions in the string
    functions.clear();

    // clear all but one box (the empty one)
    int size = functionStringBoxes.size() - 1;
    for (int i = 0; i < size; ++i)
    {
        functionStringBoxes.pop_front();
    }
    // reset the empty box position
    functionStringBoxes.front()->position.x = functionStringHitBoxes[0].x;

    // free up the box bar textures
    for (auto it = boxBars.begin(); it != boxBars.end(); ++it)
    {
        (*it)->tex->free();
    }
    // clear the box bar array
    boxBars.clear();

    // reset indexing
    listPosition = 0;
    numBoxes = 1;
}

// work out what to do as the robot passes over a tile (pick up, put down inventory)
void UI::processInventory(TileAndDirection* info)
{
    // check state
    switch (info->tile->getType())
    {
    case CANYON_CAP_LEFT:
    {
        info->tile->setType(BRIDGE_CAP_LEFT);
        InventoryItem* temp = nullptr;
        for (InventoryItem* item : inventoryItems)
        {
            if (item->tex->getFileName() == BRIDGE_ICON)
            {
                temp = item;
            }
        }
        if (temp != nullptr)
        {
            temp->tex->free();
            inventoryItems.remove(temp);
            temp = nullptr;
            --bridges;
        }
        break;
    }
    case CANYON_CAP_TOP:
    {
        info->tile->setType(BRIDGE_CAP_TOP);
        InventoryItem* temp = nullptr;
        for (InventoryItem* item : inventoryItems)
        {
            if (item->tex->getFileName() == BRIDGE_ICON)
            {
                temp = item;
            }
        }
        if (temp != nullptr)
        {
            temp->tex->free();
            inventoryItems.remove(temp);
            temp = nullptr;
            --bridges;
        }
        break;
    }
    case CANYON_CAP_RIGHT:
    {
        info->tile->setType(BRIDGE_CAP_RIGHT);
        InventoryItem* temp = nullptr;
        for (InventoryItem* item : inventoryItems)
        {
            if (item->tex->getFileName() == BRIDGE_ICON)
            {
                temp = item;
            }
        }
        if (temp != nullptr)
        {
            temp->tex->free();
            inventoryItems.remove(temp);
            temp = nullptr;
            --bridges;
        }
        break;
    }
    case CANYON_CAP_BOTTOM:
    {
        info->tile->setType(BRIDGE_CAP_BOTTOM);
        InventoryItem* temp = nullptr;
        for (InventoryItem* item : inventoryItems)
        {
            if (item->tex->getFileName() == BRIDGE_ICON)
            {
                temp = item;
            }
        }
        if (temp != nullptr)
        {
            temp->tex->free();
            inventoryItems.remove(temp);
            temp = nullptr;
            --bridges;
        }
        break;
    }
    case CANYON_VERT:
    {
        info->tile->setType(BRIDGE_HORIZ);
        InventoryItem* temp = nullptr;
        for (InventoryItem* item : inventoryItems)
        {
            if (item->tex->getFileName() == BRIDGE_ICON)
            {
                temp = item;
            }
        }
        if (temp != nullptr)
        {
            temp->tex->free();
            inventoryItems.remove(temp);
            temp = nullptr;
            --bridges;
        }
        break;
    }
    case CANYON_HORIZ:
    {
        info->tile->setType(BRIDGE_VERT);
        InventoryItem* temp = nullptr;
        for (InventoryItem* item : inventoryItems)
        {
            if (item->tex->getFileName() == BRIDGE_ICON)
            {
                temp = item;
            }
        }
        if (temp != nullptr)
        {
            temp->tex->free();
            inventoryItems.remove(temp);
            temp = nullptr;
            --bridges;
        }
        break;
    }
    case SURVIVOR:
    {
        // check we have space to hold another item
        if (inventoryItems.size() < INVENTORY_MAX_SIZE)
        {
            // change the tile type to empty terrain
            info->tile->setType(PASSABLE);
            // create the inventory item
            InventoryItem* survivor = new InventoryItem;
            survivor->tex = new Texture;
            survivor->tex->loadFromFile(SURVIVOR_ICON, renderer);
            inventoryItems.push_back(survivor);
			++survivors;
        }
        break;
    }
    case BRIDGE_CRATE:
    {
        // check we have space to hold another item
        if (inventoryItems.size() < INVENTORY_MAX_SIZE)
        {
            // change the tile type to empty terrain
            info->tile->setType(PASSABLE);
            // create the inventory item
            InventoryItem* bridge = new InventoryItem;
            bridge->tex = new Texture;
            bridge->tex->loadFromFile(BRIDGE_ICON, renderer);
            inventoryItems.push_back(bridge);
            ++bridges;
            bridgesLeft = bridges;
        }
        break;
    }
    case CRATE:
    {
        // check we have space to hold another item
        if (inventoryItems.size() < INVENTORY_MAX_SIZE)
        {
            // change the tile type to empty terrain
            info->tile->setType(PASSABLE);
            // create the inventory item
            InventoryItem* crate = new InventoryItem;
			crate->tex = new Texture;
			crate->tex->loadFromFile(SUPPLY_ICON, renderer);
            inventoryItems.push_back(crate);
            ++supplies;
        }
        break;
    }
    default:
        break;
    }
}

// what ever media we need is loaded in here
bool UI::loadMedia()
{
    leftPanel = new Texture;
    bottomPanel = new Texture;
    goButton = new Texture;
    OkButton = new Texture;
    listBackButton = new Texture;
    listForwardButton = new Texture;
    speechBubble = new Texture;
    trashcan = new Texture;
    openTrashcan = new Texture;
    scoreBackground = new Texture;
    exitButton = new Texture;
	resetButton = new Texture;
    drOgel = new Texture;
    continueButton = new Texture;

    //Loads star texture files
    for (int i = 0; i < NUM_OF_STARS; i++)
    {
        stars.push_back(new Texture);
        blankStars.push_back(new Texture);

        if(!stars.at(i)->loadFromFile(FULL_STAR_FILE, renderer))
            return false;
        if (!blankStars.at(i)->loadFromFile(FULL_STAR_FILE, renderer))
            return false;
    }


    if (!leftPanel->loadFromFile(LEFT_UI, renderer))
        return false;

    if (!bottomPanel->loadFromFile(BOTTOM_UI, renderer))
        return false;

    if (!goButton->loadFromFile(GO_BUTTON_UI, renderer))
        return false;

    if (!OkButton->loadFromFile(OK_BUTTON_UI, renderer))
        return false;

    if (!listBackButton->loadFromFile(LIST_MOVE, renderer))
        return false;

    if (!listForwardButton->loadFromFile(LIST_MOVE, renderer))
        return false;

    if (!speechBubble->loadFromFile(SPEECH_BUBBLE, renderer))
        return false;

    if (!trashcan->loadFromFile(TRASHCAN, renderer))
        return false;

    if (!openTrashcan->loadFromFile(OPEN_TRASHCAN, renderer))
        return false;

    if (!exitButton->loadFromFile(EXIT_BUTTON_FILE, renderer))
        return false;

    if (!resetButton->loadFromFile(RESET_BUTTON_FILE, renderer))
        return false;

    if (!scoreBackground->loadFromFile(SCORE_BACKGROUND_FILE, renderer))
        return false;

	if (!drOgel->loadFromFile(DR_O, renderer))
		return false;

    if (!continueButton->loadFromFile(CONTINUE_BUTTON_FILE, renderer))
        return false;

    return true;
}



// inputs //

void UI::touchInputHandler(SDL_Event& event, float& frameTime, SDL_Point& touchLocation,
    SDL_Rect& camera, const SDL_Rect& screenSize)
{
    ////////////////
    // touch down //
    ////////////////
    if (event.type == SDL_FINGERDOWN)
    {
        touchLocation.x = static_cast<int>(event.tfinger.x * static_cast<float>(screenSize.w));
        touchLocation.y = static_cast<int>(event.tfinger.y * static_cast<float>(screenSize.h));

        downPresses(touchLocation);
    }
    //////////////////
    // touch motion //
    //////////////////
    else if (event.type == SDL_FINGERMOTION)
    {
        touchLocation.x = static_cast<int>(event.tfinger.x * static_cast<float>(screenSize.w));
        touchLocation.y = static_cast<int>(event.tfinger.y * static_cast<float>(screenSize.h));

        pressReleases(touchLocation);
    }
    ///////////////////
    // touch release //
    ///////////////////
    else if (event.type == SDL_FINGERUP)
    {
        touchLocation.x = static_cast<int>(event.tfinger.x * static_cast<float>(screenSize.w));
        touchLocation.y = static_cast<int>(event.tfinger.y * static_cast<float>(screenSize.h));

        motion(touchLocation, camera);
    }
}
void UI::mouseInputHandler(SDL_Event& event, float& frameTime, SDL_Point& touchLocation,
    SDL_Rect& camera)
{
    //////////////////////////
    // mouse button clicked //
    //////////////////////////
    if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        touchLocation.x = event.button.x;
        touchLocation.y = event.button.y;

        downPresses(touchLocation);
    }

    ///////////////////////////
    // mouse button released //
    ///////////////////////////
    if (event.type == SDL_MOUSEBUTTONUP)
    {
        pressReleases(touchLocation);
    }

    //////////////////
    // mouse motion //
    //////////////////
    if (event.type == SDL_MOUSEMOTION)
    {
        touchLocation.x = event.button.x;
        touchLocation.y = event.button.y;

        motion(touchLocation, camera);
    }
}
void UI::okButtonActiveOnly(SDL_Event& event, float& frameTime, SDL_Point& touchLocation,
    SDL_Rect& camera, const SDL_Rect& screenSize)
{
	if (okPressed)
	{
		okButton(touchLocation);
	}
	////////////////
	// touch down //
	////////////////
    else if (event.type == SDL_FINGERDOWN)
    {
        touchLocation.x = static_cast<int>(event.tfinger.x * static_cast<float>(screenSize.w));
        touchLocation.y = static_cast<int>(event.tfinger.y * static_cast<float>(screenSize.h));

        okButton(touchLocation);
    }
    //////////////////////////
    // mouse button clicked //
    //////////////////////////
    else if (event.type == SDL_MOUSEBUTTONDOWN)
    {
        touchLocation.x = event.button.x;
        touchLocation.y = event.button.y;

        okButton(touchLocation);
    }
}
void UI::scoreScreenHandler(SDL_Event& event, float& frameTime, SDL_Point& touchLocation,
	SDL_Rect& camera, const SDL_Rect& screenSize)
{
	////////////////
	// touch down //
	////////////////
	if (event.type == SDL_FINGERDOWN)
	{
		touchLocation.x = static_cast<int>(event.tfinger.x * static_cast<float>(screenSize.w));
		touchLocation.y = static_cast<int>(event.tfinger.y * static_cast<float>(screenSize.h));

		scoreScreenButtons(touchLocation);
	}
	//////////////////////////
	// mouse button clicked //
	//////////////////////////
	else if (event.type == SDL_MOUSEBUTTONDOWN)
	{
		touchLocation.x = event.button.x;
		touchLocation.y = event.button.y;

		scoreScreenButtons(touchLocation);
	}
}

// input segregation

void UI::downPresses(SDL_Point& touchLocation)
{
    // if the mouse is over the left UI panel //
    if (touchLocation.x > 0 && touchLocation.x < viewportLeft->w && touchLocation.y > 0 && touchLocation.y < viewportLeft->h)
    {
        downLeftUI(touchLocation);
    }

    // if the mouse is over the bottom UI panel //
    else if (touchLocation.x > viewportBottom->x && touchLocation.x < SCREEN_SIZE.w &&
        touchLocation.y > viewportBottom->y && touchLocation.y < SCREEN_SIZE.h)
    {
        downBottomUI(touchLocation);
    }

    // if mouse is over the map //
    else if (touchLocation.x > viewportLeft->w && touchLocation.x < SCREEN_SIZE.w &&
        touchLocation.y > 0 && touchLocation.y < viewportMain->h)
    {
        downMainWindowUI(touchLocation);
    }
}
void UI::pressReleases(SDL_Point& touchLocation)
{
    // we stop the map from dragging with mouse movement
    mapDrag = false;

    // if the mouse is over the bottom UI panel //
    if (touchLocation.x > viewportBottom->x && touchLocation.x < SCREEN_SIZE.w &&
        touchLocation.y > viewportBottom->y && touchLocation.y < SCREEN_SIZE.h)
    {
        upBottomUI(touchLocation);
    }

    // we reset the function back where we found it
    else if (functionPickedUp != nullptr)
    {
        functionPickedUp->function->tex->loadFromFile(functionTemplate->getFileName(), renderer);

        functionTemplate->free();

        // reset the functionPickedUp var
        functionPickedUp = nullptr;
    }

    //clear the template
    else if (functionTemplate != nullptr && functionTemplate->getTexture() != nullptr)
        functionTemplate->free();
}
void UI::motion(SDL_Point& touchLocation, SDL_Rect& camera)
{
    // if the mouse is over the left UI panel //
    if (touchLocation.x > 0 && touchLocation.x < viewportLeft->w &&
        touchLocation.y > 0 && touchLocation.y < viewportLeft->h)
    {
        motionLeftUI(touchLocation);
    }

    // if the mouse is over the bottom UI panel //
    else if (touchLocation.x > viewportLeft->w && touchLocation.x < SCREEN_SIZE.w &&
        touchLocation.y > viewportBottom->y && touchLocation.y < SCREEN_SIZE.h)
    {
        motionBottomUI(touchLocation);
    }

    // if mouse is over the map //
    else if (touchLocation.x > viewportLeft->w && touchLocation.x < SCREEN_SIZE.w &&
        touchLocation.y > 0 && touchLocation.y < viewportMain->h)
    {
        motionMainWindowUI(touchLocation, camera);
    }
}

// input code

void UI::downLeftUI(SDL_Point& touchLocation)
{
    // if the mouse is over a button when clicked, a function template is created
    // that will follow the mouse pointer around the screen
    for (auto functionBox = functionHitBoxes.begin(); functionBox != functionHitBoxes.end(); ++functionBox)
    {
        SDL_Rect* box = (*functionBox);
        int position = functionBox - functionHitBoxes.begin();

        if (touchLocation.x > box->x && touchLocation.x < box->x + box->w &&
            touchLocation.y >box->y && touchLocation.y < box->y + box->h)
        {
            // if no template has been created before, memory is allocated, else it frees up the current template
            if (functionTemplate == nullptr)
                functionTemplate = new Texture;

            functionTemplate->loadFromFile(functionFilenames.at(position), renderer);
        }

        // Exit button
        if (touchLocation.x > EXIT_BUTTON.x &&
            touchLocation.x < EXIT_BUTTON.x + EXIT_BUTTON.w &&
            touchLocation.y > EXIT_BUTTON.y &&
            touchLocation.y < EXIT_BUTTON.y + EXIT_BUTTON.h)
        {   
            quitGame = true;
        }

        // Reset button
        if (touchLocation.x > RESET_BUTTON.x &&
            touchLocation.x < RESET_BUTTON.x + RESET_BUTTON.w &&
            touchLocation.y > RESET_BUTTON.y &&
            touchLocation.y < RESET_BUTTON.y + RESET_BUTTON.h)
        {
			restartMission = true;
        }
    }
}
void UI::downBottomUI(SDL_Point& touchLocation)
{
    // go button pressed
    if (touchLocation.x > viewportBottom->x + GO_BUTTON.x &&
        touchLocation.x < viewportBottom->x + GO_BUTTON.x + GO_BUTTON.w &&
        touchLocation.y > viewportBottom->y + GO_BUTTON.y &&
        touchLocation.y < viewportBottom->y + GO_BUTTON.y + GO_BUTTON.h)
    {
        // make sure we have something to run
        if (functions.size() > 0)
        {
            // process all movement
            goButtonPressed = true;

            // clear the function template from the cursor if there is one
            if (functionTemplate != nullptr)
                functionTemplate->free();
        }
        else
        {
            scriptOverridden = true;
            stringToRender[0] = "No functions selected!";
        }
    }

    // if over the left single arrow (1 step backward)
    else if (touchLocation.x > viewportBottom->x + ARROWS_LEFT_X_POS + FUNCTION_WIDTH / 2 &&
        touchLocation.x < viewportBottom->x + ARROWS_LEFT_X_POS + FUNCTION_WIDTH &&
        touchLocation.y > viewportBottom->y + ARROWS_LEFT_Y_POS &&
        touchLocation.y < viewportBottom->y + ARROWS_LEFT_Y_POS + FUNCTION_HEIGHT)
    {
        if (listPosition > 0)
        {
            --listPosition;

            // reposition the functions and the boxes
            for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
            {
                (*it)->position.x += FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM;
            }
        }
    }
    // if over the left double arrow (back to start)
    else if (touchLocation.x > viewportBottom->x + ARROWS_LEFT_X_POS &&
        touchLocation.x < viewportBottom->x + ARROWS_LEFT_X_POS + FUNCTION_WIDTH / 2 &&
        touchLocation.y > viewportBottom->y + ARROWS_LEFT_Y_POS &&
        touchLocation.y < viewportBottom->y + ARROWS_LEFT_Y_POS + FUNCTION_HEIGHT)
    {
        if (listPosition > 0)
        {
            for (int i = 0; i < listPosition; ++i)
            {
                // reposition the functions and the boxes
                for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
                {
                    (*it)->position.x += FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM;
                }
            }
            listPosition = 0;
        }
    }
    // if over the right single arrow (1 step forward)
    else if (touchLocation.x > viewportBottom->x + ARROWS_RIGHT_X_POS &&
        touchLocation.x < viewportBottom->x + ARROWS_RIGHT_X_POS + FUNCTION_WIDTH / 2 &&
        touchLocation.y > viewportBottom->y + ARROWS_RIGHT_Y_POS &&
        touchLocation.y < viewportBottom->y + ARROWS_RIGHT_Y_POS + FUNCTION_HEIGHT)
    {
        if (listPosition < numBoxes - NUM_FUNC_STRING_BOXES)
        {
            ++listPosition;

            // reposition the functions and the boxes
            for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
            {
                (*it)->position.x -= FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM;
            }
        }
    }
    // if over the right double arrow (skip to end)
    else if (touchLocation.x > viewportBottom->x + ARROWS_RIGHT_X_POS + FUNCTION_WIDTH / 2 &&
        touchLocation.x < viewportBottom->x + ARROWS_RIGHT_X_POS + FUNCTION_WIDTH &&
        touchLocation.y > viewportBottom->y + ARROWS_RIGHT_Y_POS &&
        touchLocation.y < viewportBottom->y + ARROWS_RIGHT_Y_POS + FUNCTION_HEIGHT)
    {
        while (listPosition < numBoxes - NUM_FUNC_STRING_BOXES)
        {
            // reposition the functions and the boxes
            for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
            {
                (*it)->position.x -= FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM;
            }

            ++listPosition;
        }
    }

    // check if over any of the boxes
    int index = 0;
    for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
    {
        FunctionStringBox* box = *it;

        if (touchLocation.x > box->position.x && touchLocation.x < box->position.x + FUNCTION_WIDTH &&
            touchLocation.y > box->position.y && touchLocation.y < box->position.y + FUNCTION_HEIGHT)
        {
            // if theres something in that box and we're actually over one of the displayed boxes
            if (box->function && index >= listPosition && index < listPosition + NUM_FUNC_STRING_BOXES)
            {
                // create a function template of that function
                functionTemplate->loadFromFile(box->function->tex->getFileName(), renderer);

                // clear the box function texture, leaving it visually an empty box (leaving the function still there though)
                box->function->tex->free();
                functionPickedUp = box;
                funcStringBoxIndex = index;
                break;
            }
        }
        ++index;
    }
}
void UI::downMainWindowUI(SDL_Point& touchLocation)
{
    mapDrag = true;
    mapLocation.x = touchLocation.x;
    mapLocation.y = touchLocation.y;
}

void UI::upBottomUI(SDL_Point& touchLocation)
{
    // if over an empty function string box in the bottom UI panel with a selected function
    if (useBoxPos && functionPickedUp == nullptr)
    {
        // providing we have a template selected and is attached to our mouse pointer
        if (functionTemplate != nullptr && functionTemplate->getTexture() != nullptr)
        {
            // when we release the mouse button we create a Texture in the image of the template at the template location
            // that will be added to a vector array and paired up with its location for rendering
            Function* newFunction = new Function;

            newFunction->tex = new Texture;

            newFunction->tex->loadFromFile(functionTemplate->getFileName(), renderer);
            newFunction->complete = false;

            if (tempBoxCreated != nullptr)
            {
                std::list<Function*>::iterator boxPos;
                boxPos = functions.begin();
                for (int i = 0; i < tempIndex; ++i)
                    ++boxPos;

                functions.insert(boxPos, newFunction);

                // if a valid change to the function string
                if (checkNewFuncString())
                {
                    int i = 0;
                    for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
                    {
                        if (i == tempIndex)
                        {
                            (*it)->function = newFunction;
                            break;
                        }

                        ++i;
                    }
                }
                else
                {
                    // if the new string of functions is invalid, rollback
                    functions.remove(newFunction);
                    newFunction->tex->free();
                    delete newFunction;
                    tempBoxCreated->emptyBox->free();
                    functionStringBoxes.remove(tempBoxCreated);
                    boxBars.back()->tex->free();
                    boxBars.pop_back();
                    --numBoxes;

                    // rerun the route map check to highlight the old route again
                    checkNewFuncString();

                    // reposition any boxes after the deleted box
                    int index = 0;
                    for (auto box = functionStringBoxes.begin(); box != functionStringBoxes.end(); ++box)
                    {
                        if (index >= tempIndex)
                        {
                            (*box)->position.x = (*box)->position.x - FUNCTION_WIDTH - FUNCTION_SPACING_BOTTOM;
                        }
                        ++index;
                    }
                }

                tempBoxCreated = nullptr;
            }
            else
                // if no temp box created (adding to the end of the string)
            {
                if (isValidMove(functionTemplate))
                {
                    functions.push_back(newFunction);
                    functionStringBoxes.back()->function = newFunction;

                    if (numBoxes - listPosition >= NUM_FUNC_STRING_BOXES)
                        gapVisible = false;
                    else
                        gapVisible = true;

                    // add a new box to be filled
                    createFuncStringBox();
                }
                else
                {
                    // we dont need the new function as it wasn't placed
                    newFunction->tex->free();
                    delete newFunction;
                }
            }

            // clear the template
            functionTemplate->free();
        }
    }
    // if we're holding a function from the function string
    else if (functionPickedUp != nullptr)
    {
        // if over the trashcan viewportBottom->w - TRASHCAN_WIDTH, viewportBottom->h - TRASHCAN_HEIGHT
        if (touchLocation.x > SCREEN_SIZE.w - TRASHCAN_WIDTH && touchLocation.x < SCREEN_SIZE.w &&
            touchLocation.y > SCREEN_SIZE.h - TRASHCAN_HEIGHT && touchLocation.y < SCREEN_SIZE.h)
        {

            overTrashcan = false; // close bin

                                  // clear the template
            functionTemplate->free();

            Function* tempFunc = new Function;
            tempFunc->tex = new Texture;
            tempFunc->tex->loadFromFile(functionPickedUp->function->tex->getFileName(), renderer);

            // clear the function from its list
            functions.remove(functionPickedUp->function);

            // if a valid change to the function string
            if (checkNewFuncString())
            {
                // clean up our tempFunc as not needed
                tempFunc->tex->free();
                delete tempFunc;

                // clear the box texture and remove the box from its list
                functionPickedUp->emptyBox->free();
                functionStringBoxes.remove(functionPickedUp);
                --numBoxes;

                // reset the functionPickedUp var
                functionPickedUp = nullptr;

                // reposition any boxes after the deleted box
                int index = 0;
                for (auto box = functionStringBoxes.begin(); box != functionStringBoxes.end(); ++box)
                {
                    if (index >= funcStringBoxIndex)
                    {
                        (*box)->position.x = (*box)->position.x - FUNCTION_WIDTH - FUNCTION_SPACING_BOTTOM;
                    }
                    ++index;
                }

                // remove the end box bar
                boxBars[numBoxes - 1]->tex->free();
                boxBars.pop_back();
            }
            else
            {
                // rollback and clean up //

                functionPickedUp->function->tex->loadFromFile(tempFunc->tex->getFileName(), renderer);

                // get an iterator position for funcStringBoxIndex
                std::list<Function*>::iterator boxPos;
                boxPos = functions.begin();
                for (int i = 0; i < funcStringBoxIndex; ++i)
                    ++boxPos;

                functions.insert(boxPos, functionPickedUp->function);
                tempFunc->tex->free();
                delete tempFunc;

                functionTemplate->free();

                // reset the functionPickedUp var
                functionPickedUp = nullptr;

                // rerun the route map check to highlight the old route again
                checkNewFuncString();
            }
        }
        // we reset the function back where we found it
        else
        {
            functionPickedUp->function->tex->loadFromFile(functionTemplate->getFileName(), renderer);

            functionTemplate->free();

            // reset the functionPickedUp var
            functionPickedUp = nullptr;
        }
    }
    //clear the template
    else if (functionTemplate != nullptr && functionTemplate->getTexture() != nullptr)
        functionTemplate->free();
}

void UI::motionLeftUI(SDL_Point& touchLocation)
{
    bool skip = false;
    // go through each button location
    for (auto functionBox = functionHitBoxes.begin(); functionBox != functionHitBoxes.end(); ++functionBox)
    {
        SDL_Rect* box = (*functionBox);

        int position = functionBox - functionHitBoxes.begin(); // Index of the box

                                                               // if the mouse is over a button
        if (touchLocation.x > box->x && touchLocation.x < box->x + box->w &&
            touchLocation.y > box->y && touchLocation.y < box->y + box->h)
        {
            // we only want to select a new button if we're not holding a template
            //if (functionTemplate == nullptr || functionTemplate->getTexture() == nullptr)	// optional
            {
                // if no memory allocation has yet been made for the selection texture, allocate it
                if (functionSelect.first == nullptr)
                {
                    functionSelect.first = new Texture;
                    functionSelect.second = new SDL_Point;
                }
                else if (functionSelect.first->getTexture() != nullptr)
                {
                    // if we are not over the same button, free up the texture
                    if (functionSelect.second->x != box->x && functionSelect.second->y != box->y)
                    {
                        functionSelect.first->free();
                    }
                    // otherwise we are over the same image as previously allocated so we don't want
                    // to waste performance re-assigning variables to the same thing
                    else
                        skip = true;
                }

                // set the variables and image if over a new button
                if (!skip)
                {
                    functionSelect.first->loadFromFile(functionFilenames.at(position), renderer);
                    functionSelect.second->x = box->x;
                    functionSelect.second->y = box->y;
                }
            }
            break;
        }
        // if we're not over a button, free up the texture
        else if (functionSelect.first != nullptr && functionSelect.first->getTexture() != nullptr)
            functionSelect.first->free();
    }
}
void UI::motionBottomUI(SDL_Point& touchLocation)
{
    // go through each box position
    int index = 0;
    for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
    {
        FunctionStringBox* box = *it;

        // check if over any of the box bars
        if (touchLocation.x > box->position.x - 20 && touchLocation.x < box->position.x - BOX_BAR_WIDTH &&
            touchLocation.y > box->position.y && touchLocation.y < box->position.y + FUNCTION_HEIGHT)
        {
            // don't render anything thats outside the boundaries
            if (index >= listPosition && index < listPosition + NUM_FUNC_STRING_BOXES && index < static_cast<int>(boxBars.size()))
            {
                // if we have a template selected
                if (functionTemplate != nullptr && functionTemplate->getTexture() != nullptr && tempBoxCreated == nullptr)
                {
                    insertFuncStringBox(index);
                    tempIndex = index;
                    break;
                }
            }
        }

        // check if over any of the boxes
        else if (touchLocation.x > box->position.x && touchLocation.x < box->position.x + FUNCTION_WIDTH &&
            touchLocation.y > box->position.y && touchLocation.y < box->position.y + FUNCTION_HEIGHT)
        {
            // if theres not something already in that box
            if (!box->function)
            {
                if ((numBoxes - listPosition <= NUM_FUNC_STRING_BOXES && tempBoxCreated == nullptr) ||
                    tempBoxCreated != nullptr)
                {
                    // if we have a template selected
                    if (functionTemplate != nullptr && functionTemplate->getTexture() != nullptr)
                    {
                        templatePosX = box->position.x;
                        templatePosY = box->position.y;
                        useBoxPos = true;
                        break;
                    }
                }
            }
        }
        else
        {
            // if a temp box has been made
            if (tempBoxCreated != nullptr)
            {
                // else if not over the temp box bar and box area
                if (touchLocation.x < tempBoxCreated->position.x - 20 || touchLocation.x > tempBoxCreated->position.x + FUNCTION_WIDTH ||
                    touchLocation.y < tempBoxCreated->position.y || touchLocation.y > tempBoxCreated->position.y + FUNCTION_HEIGHT)
                {
                    // remove the temp box and temp box bar
                    functionStringBoxes.remove(tempBoxCreated);
                    --numBoxes;

                    tempBoxCreated = nullptr;

                    // remove the extra box bar
                    boxBars.pop_back();

                    // reposition the functions and the boxes past it
                    int boxIndex = 0;
                    for (auto it = functionStringBoxes.begin(); it != functionStringBoxes.end(); ++it)
                    {
                        if (boxIndex >= tempIndex)
                        {
                            (*it)->position.x -= FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM;
                        }

                        ++boxIndex;
                    }
                    useBoxPos = false;
                    break;
                }
            }
            // if not over a box, render the function template at the mouse pointer
            useBoxPos = false;
        }

        ++index;
    }

    // Mouse over trash can
    if (functionPickedUp != nullptr)
    {
        // if over the trashcan viewportBottom->w - TRASHCAN_WIDTH, viewportBottom->h - TRASHCAN_HEIGHT
        if (touchLocation.x > SCREEN_SIZE.w - TRASHCAN_WIDTH && touchLocation.x < SCREEN_SIZE.w &&
            touchLocation.y > SCREEN_SIZE.h - TRASHCAN_HEIGHT && touchLocation.y < SCREEN_SIZE.h)
        {
            overTrashcan = true; // over trash can  
        }
        else
        {

            overTrashcan = false; // Not over trash can
        }
    }


}
void UI::motionMainWindowUI(SDL_Point& touchLocation, SDL_Rect& camera)
{
    if (mapDrag)
    {
        // drag the map
        camera.x -= touchLocation.x - mapLocation.x;
        camera.y -= touchLocation.y - mapLocation.y;

        // update map location so we don't compound the effect
        mapLocation.x = touchLocation.x;
        mapLocation.y = touchLocation.y;

        //Keep the camera in bounds
        if (camera.x < 0)
            camera.x = 0;

        if (camera.y < 0)
            camera.y = 0;

        if (camera.x > LEVEL_WIDTH - camera.w)
            camera.x = LEVEL_WIDTH - camera.w;

        if (camera.y > LEVEL_HEIGHT - camera.h)
            camera.y = LEVEL_HEIGHT - camera.h;
    }
}

void UI::okButton(SDL_Point& touchLocation)
{
	// if mouse is over the map //
	if (touchLocation.x > viewportLeft->w && touchLocation.x < SCREEN_SIZE.w &&
		touchLocation.y > 0 && touchLocation.y < viewportMain->h)
	{
		// ok button pressed (either of them)
		if (((touchLocation.x > viewportLeft->w + OK_BUTTON_DR.x &&
			touchLocation.x < viewportLeft->w + OK_BUTTON_DR.x + OK_BUTTON_DR.w &&
			touchLocation.y > OK_BUTTON_DR.y &&
			touchLocation.y < OK_BUTTON_DR.y + OK_BUTTON_DR.h) &&
			(codeToRender % 10 == 1)) ||
			((touchLocation.x > viewportLeft->w + OK_BUTTON_PROF.x &&
				touchLocation.x < viewportLeft->w + OK_BUTTON_PROF.x + OK_BUTTON_PROF.w &&
				touchLocation.y > OK_BUTTON_PROF.y &&
				touchLocation.y < OK_BUTTON_PROF.y + OK_BUTTON_PROF.h) &&
				(codeToRender % 10 == 2)))
		{
			okPressed = true;
		}
	}

	// Exit button
	else if (touchLocation.x > EXIT_BUTTON.x &&
		touchLocation.x < EXIT_BUTTON.x + EXIT_BUTTON.w &&
		touchLocation.y > EXIT_BUTTON.y &&
		touchLocation.y < EXIT_BUTTON.y + EXIT_BUTTON.h)
	{
		// Exit    
		quitGame = true;
	}

	if (okPressed)
	{
		okPressed = false;

		// script increment
		if (scriptOverridden)
		{
			scriptOverridden = false;
			stringToRender[0] = "";
		}
		else
		{
			if (textRead < static_cast<int>(missionScript.size()))
			{
				// check to see if the next string is the same code as the previous (same dialog)  
				// or we've moved on to the next mission stage (unlocking more dialog)
				if (missionScript[textRead]->code == missionScript[textRead - 1]->code ||
					missionScript[textRead]->code == missionScript[textRead - 1]->code + 1 ||
					missionScript[textRead]->code == missionScript[textRead - 1]->code - 1 ||
					previousStage != currentStage)
				{
					previousStage = currentStage;
					getNextLine(); // Gets next line and increments textRead value
				}
			}
			else
				stringToRender[0] = "";
		}

		// clear the function template from the cursor if there is one
		if (functionTemplate != nullptr)
			functionTemplate->free();
	}
}
void UI::scoreScreenButtons(SDL_Point& touchLocation)
{
	// Exit button
	if (touchLocation.x > EXIT_BUTTON.x &&
		touchLocation.x < EXIT_BUTTON.x + EXIT_BUTTON.w &&
		touchLocation.y > EXIT_BUTTON.y &&
		touchLocation.y < EXIT_BUTTON.y + EXIT_BUTTON.h)
	{
		// Exit    
		quitGame = true;
	}

	// button press here to trigger GameState change in renderScoreScreen() below....

    // Continue button
    if (touchLocation.x > CONTINUE_BUTTON_RECT.x &&
        touchLocation.x < CONTINUE_BUTTON_RECT.x + CONTINUE_BUTTON_RECT.w &&
        touchLocation.y > CONTINUE_BUTTON_RECT.y &&
        touchLocation.y < CONTINUE_BUTTON_RECT.y + CONTINUE_BUTTON_RECT.h)
    {
        continuePressed = true;
    }
}

// script //

bool UI::LoadScript(std::string filePath, std::vector<MissionText*>& missionScript)
{
    std::ifstream inFile;

    inFile.open(filePath);
    if (inFile.fail())
    {
        //printf("Error, file not found \n");
        return false;
    }

    std::string line;
    while (std::getline(inFile, line))
    {
        std::string code;
        // read the string and extract the code at the start
        for (char ch : line)
        {
            if (ch != ' ')
                code += ch;
            else
                break;
        }
        // remove the code and the space from the string
        if (code != "")
            line = line.substr(code.size() + 1, line.size() - (code.size() + 1));

        // create the MissionScript object, storing both the string (minus the code) and the code seperate
        MissionText* text = new MissionText;
        text->line = line;
        text->code = atoi(code.c_str());

        missionScript.push_back(text);
    }
    return true;

}

// getter that checks range and also increments the value
void UI::getNextLine()
{
    stringToRender.clear();

    if (textRead < static_cast<int>(missionScript.size()))
    {
        MissionText* text = missionScript.at(textRead);
        ++textRead;

        stringToRender.push_back(text->line);
        codeToRender = text->code;
    }
}



// rendering //

// render the UI to the screen
void UI::render(SDL_Point& touchLocation, SDL_Rect &camera)
{
    // render to the left view port //

    // set viewports and render textures to screen
    SDL_RenderSetViewport(renderer, viewportLeft);

    leftPanel->renderMedia(0, 0, renderer);
    exitButton->renderMedia(EXIT_BUTTON.x, EXIT_BUTTON.y, renderer);
	resetButton->renderMedia(RESET_BUTTON.x, RESET_BUTTON.y, renderer);

    //SDL_RenderCopy(renderer, leftPanel->getTexture(), 0, 0);

    // render the inventory
    int inventIndexRow = 0;
    int inventIndexCol = 0;
    for (InventoryItem* item : inventoryItems)
    {
        item->tex->renderMedia(FUNCTION_SPACING_LEFT + (inventIndexCol * (FUNCTION_WIDTH + FUNCTION_SPACING_LEFT)),
            INVENTORY_START_HEIGHT + (inventIndexRow * (FUNCTION_HEIGHT + FUNCTION_SPACING_LEFT)), renderer);

        if (inventIndexCol == 1)
        {
            inventIndexCol = 0;
            ++inventIndexRow;
        }
        else
            ++inventIndexCol;
    }

    // render the bottom view port textures //

    SDL_RenderSetViewport(renderer, viewportBottom);
    bottomPanel->renderMedia(0, 0, renderer);
    listBackButton->renderMedia(ARROWS_LEFT_X_POS, ARROWS_LEFT_Y_POS, renderer, 0, 0.0, 0, SDL_FLIP_HORIZONTAL);
    listForwardButton->renderMedia(ARROWS_RIGHT_X_POS, ARROWS_RIGHT_Y_POS, renderer);

    if (overTrashcan)
    {
        openTrashcan->renderMedia(viewportBottom->w - TRASHCAN_WIDTH, viewportBottom->h - TRASHCAN_HEIGHT, renderer);
    }
    else
    {
        trashcan->renderMedia(viewportBottom->w - TRASHCAN_WIDTH, viewportBottom->h - TRASHCAN_HEIGHT, renderer);
    }

    goButton->renderMedia(GO_BUTTON.x, GO_BUTTON.y, renderer);

    // render to the main view port (map) //

    SDL_RenderSetViewport(renderer, viewportMain);

    // render route overlays
    for (RouteOverlay* tile : routeTiles)
    {
        tile->tex->renderMedia(tile->location.x - camera.x, tile->location.y - camera.y, renderer);
    }

    // render to full screen //

    // set viewport
    SDL_RenderSetViewport(renderer, viewportFull);

    // render the function template
    if (functionTemplate != nullptr)
    {
        // if over an empty function string box, snap into position
        if (useBoxPos && functionPickedUp == nullptr)
            functionTemplate->renderMedia(templatePosX, templatePosY, renderer);
        else
            functionTemplate->renderMedia(touchLocation.x - functionTemplate->getWidth() / 2, touchLocation.y - functionTemplate->getHeight() / 2, renderer);
    }

    if (functionSelect.first != nullptr && functionSelect.first->getTexture() != nullptr)
    {
        functionSelect.first->renderMedia(functionSelect.second->x, functionSelect.second->y, renderer);
    }

    // render the function string boxes
    int index = 0;
    for (auto function = functionStringBoxes.begin(); function != functionStringBoxes.end(); ++function)
    {
        // don't render anything thats outside the boundaries
        if (index < listPosition)
        {
            ++index;
            continue;
        }
        else if (index >= listPosition + NUM_FUNC_STRING_BOXES)
        {
            break;
        }

        SDL_Rect pos = (*function)->position;

        (*function)->emptyBox->renderMedia(pos.x, pos.y, renderer);

        if ((*function)->function != nullptr)
            (*function)->function->tex->renderMedia(pos.x, pos.y, renderer);

        // render the box bar to the left of the rendered box
        if (index < static_cast<int>(boxBars.size()))
            boxBars[index]->tex->renderMedia(pos.x - 20, pos.y, renderer);

        ++index;
    }
}

// allows non scripted speech to be displayed
void UI::setStringToRender(std::string text)
{ 
	codeToRender = 12;
	stringToRender[0] = text; 
	scriptOverridden = true; 
}

// render the speech bubbles
bool UI::renderText(int moves, int numSupplies, int numSurvivors)
{
    if (static_cast<int>(stringToRender.size()) > 0)
    {
        if (currentText != stringToRender[0])
        {
            if (stringToRender[0] != "")
            {
                // recursive division of the string
                divideString(stringToRender, 0);

                // go through each line of text and create a texture for each
                textLines.clear();
                for (std::string text : stringToRender)
                {
                    Texture* textTex = new Texture;
                    if (!textTex->loadText(text, textColor, renderer, font))
                        return false;

                    textLines.push_back(textTex);
                }
            }
            currentText = stringToRender[0];
        }
    }

    okActive = false;
    //Render text and background
    if (currentText != "")
    {
		// render Dr Ogel
		if (codeToRender % 100 != 11 && codeToRender % 100 != 12)
			drOgel->renderMedia(DR_O_POS.x, DR_O_POS.y, renderer);

		// Dr Ogel's lines
		if (codeToRender % 10 == 1)
		{
			// render Dr Ogel's speech bubble
			speechBubble->renderMedia(SPEECH_BOX_DR.x, SPEECH_BOX_DR.y, renderer, 0, 0.0, 0, SDL_FLIP_HORIZONTAL);
			int count = 0;
			for (Texture* text : textLines)
			{
				text->renderMedia(SPEECH_DR.x, SPEECH_DR.y + (count * TEXT_HEIGHT), renderer);
				++count;
			}
			OkButton->renderMedia(OK_BUTTON_DR.x, OK_BUTTON_DR.y, renderer);
		}
		// Prof Blue's lines
		else
		{
			// render Prof Blue's speech bubble
			speechBubble->renderMedia(SPEECH_BOX_PROF.x, SPEECH_BOX_PROF.y, renderer);

			int count = 0;
			for (Texture* text : textLines)
			{
				text->renderMedia(SPEECH_PROF.x, SPEECH_PROF.y + (count * TEXT_HEIGHT), renderer);
				++count;
			}
			OkButton->renderMedia(OK_BUTTON_PROF.x, OK_BUTTON_PROF.y, renderer);
		}
        okActive = true;
    }
	// if at the end of the intro, transition straight into the brief
	else if (currentStage == MissionStages::INTRO)
	{
		drOgel->renderMedia(DR_O_POS.x, DR_O_POS.y, renderer);
		currentStage = MissionStages::BRIEF;
		okPressed = true;
		okActive = true;
	}
	else if (currentStage == MissionStages::BRIEF && 
		map->missionScript == MAPS[0].missionScript && moves == 5)
	{
		currentStage = MissionStages::MISSION;
		okPressed = true;
		okActive = true;
	}
	else if (currentStage == MissionStages::BRIEF && 
		map->missionScript == MAPS[1].missionScript && bridges == 1)
	{
		currentStage = MissionStages::MISSION;
		okPressed = true;
		okActive = true;
	}
	else if (currentStage == MissionStages::BRIEF && 
		map->missionScript == MAPS[2].missionScript && moves == 10)
	{
		currentStage = MissionStages::MISSION;
		okPressed = true;
		okActive = true;
	}
	else if (currentStage == MissionStages::MISSION && dig)
	{
		currentStage = MissionStages::DEBRIEF;
		okPressed = true;
		okActive = true;
	}
	else if (currentStage == MissionStages::DEBRIEF && !endOfMission)
	{
		// work out how to score the player
		int objectives = 0;

		if (numSupplies > 0)
			++objectives;
		if (numSurvivors > 0)
			++objectives;

		// if only 1 objective, give the player 1 star for partly finishing an objective
		if (objectives == 1)
		{
			if (supplies > 0 || survivors > 0)
				++score;
		}

		// increase the score (number of stars) by 1 if an objective is completed
		if (numSupplies > 0 && supplies == numSupplies)
			++score;

		if (numSurvivors > 0 && survivors == numSurvivors)
			++score;

		// end the mission
		endOfMission = true;
	}

    return true;
}

// divide up the textual strings that are spoken by the Dr and Prof (recursive)
void UI::divideString(std::vector<std::string>& stringToRender, int stringNum)
{
    int actualStringLength = 0;	// the actual string length after completing the current word

                                // if the string is longer than MAX find a nice place to divide up by looking for a space
    int count = 0;
    for (char character : stringToRender[stringNum])
    {
        if (count > STRING_LENGTH_MAX)
        {
            if (character == ' ')
            {
                actualStringLength = count;
                break;
            }
        }

        ++count;
    }

    // divide up the string if too long
    if (actualStringLength > STRING_LENGTH_MAX)
    {
        stringToRender.push_back(stringToRender[stringNum].substr(actualStringLength,
            static_cast<int>(stringToRender[stringNum].size()) - actualStringLength));
        stringToRender[stringNum] = stringToRender[stringNum].substr(0, actualStringLength);

        // remove the space at the start of the second string
        stringToRender[stringNum + 1] = stringToRender[stringNum + 1].substr(
            1, static_cast<int>(stringToRender[stringNum + 1].size()) - 1);

        divideString(stringToRender, stringNum + 1);
    }
}

// show how many stars the player earnt for that mission
void UI::renderScoreScreen(GameStates& state)
{
    if (endOfMission) // Rendering only triggered when at the end of the mission
    {
        SDL_RenderSetViewport(renderer, viewportFull);
        scoreBackground->renderMedia(SCORE_BOARD_RECT.x, SCORE_BOARD_RECT.y, renderer); // background position

        int X = SCORE_BOARD_RECT.x + (SCORE_BOARD_RECT.w / 2) - 300;
        int Y = SCORE_BOARD_RECT.y + (SCORE_BOARD_RECT.h / 2) - 50;

        for (int i = 0; i < NUM_OF_STARS; i++) // Renders all of the blank stars on the score baackground
        {
            blankStars.at(i)->renderMedia(X, Y, renderer);
            X += 250; // 200 star width and 50 spacing
        }

        X = SCORE_BOARD_RECT.x + (SCORE_BOARD_RECT.w / 2) - 300;
        for (int i = 0; i < score; i++) //Renders the number of coloured stars relative to the score gained (between 1 and NUMOFSTARS)
        {
            stars.at(i)->renderMedia(X, Y, renderer);
            X += 250; // 200 star width and 50 spacing
        }
        
        // render continueButton
        continueButton->renderMedia(CONTINUE_BUTTON_RECT.x, CONTINUE_BUTTON_RECT.y, renderer);

        if (continuePressed)
        {
            state = GameStates::NEXT_MISSION;
        }
    }
}