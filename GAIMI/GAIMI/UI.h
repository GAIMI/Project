#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <fstream>
#include <vector>
#include <list>

#include "Texture.h"
#include "Globals.h"
#include "Tile.h"


class UI
{
private:
    SDL_Renderer* renderer;

    Texture* leftPanel;	// holds available functions
    Texture* bottomPanel;	// holds the selected list of functions
    Texture* goButton;
    Texture* OkButton;
    Texture* listBackButton;	// arrows on the end of the function string boxes
    Texture* listForwardButton;
    Texture* trashcan;	// for function deletion
    Texture* openTrashcan;
    Texture* functionTemplate;
    Texture* speechBubble;	//backdrop for text on the map viewport
    Texture* scoreBackground;
    Texture* exitButton;
    Texture* clearButton;
	Texture* drOgel;

    bool quitGame = false;

    TTF_Font* font;

    SDL_Color textColor;

    SDL_Rect* viewportFull;
    SDL_Rect* viewportLeft;
    SDL_Rect* viewportBottom;
    SDL_Rect* viewportMain;

    int templatePosX;	// used when the template is over an empty function string box
    int templatePosY;	// used when the template is over an empty function string box

    // story teller //

    MissionStages previousStage;
    MissionStages currentStage;
    std::vector<MissionText*> missionScript;
    int textRead;
	bool dig = false;

    // speech text //

    bool scriptOverridden; // if the script has been overridden by an in game function
    std::vector<std::string> stringToRender; // String that is rendered at the end of every frame
    int codeToRender;	// the code of the string to be rendered, so we know who is speaking
    std::string currentText; // holds a copy of the text on screen for comparison so we dont keep creating a new texture if its the same
    std::vector<Texture*> textLines;	// holds the textures of each line of text after dividing up the current text string
    bool okActive = false;	// allows the ok button to be pressed
	bool okPressed = false;	// allows remote pressing of the button without actually pressing it

   // score //

    bool endOfMission = false; // Needs to be in private 
    int score = 2;
    std::vector<Texture*> stars;
    std::vector<Texture*> blankStars;

    // left ui //

    std::vector<SDL_Rect*> functionHitBoxes;	// the size and position of the functions in the left UI panel
    std::vector<std::string> functionFilenames;	// to identify the hit boxes

    std::list<InventoryItem*> inventoryItems;
    int bridges = 0;	// how many bridges in the inventory
    int bridgesLeft = 0;	// how many of the picked up bridges are left over after the current function string

    // bottom ui //

    std::list<Function*> functions;	// holds the placed functions in the bottom UI panel
    std::vector<SDL_Rect> functionStringHitBoxes;	// the size and position of the functions in the bottom UI panel
    std::list<FunctionStringBox*> functionStringBoxes;	// the boxes in the bottom UI when functions are added
    bool useBoxPos;
    int listPosition;	// how many positions the box list has been moved along (searching through a long list of boxes)
    int numBoxes;	// how many function string boxes have been made
    bool gapVisible;	// if the boxes have gone off the end of the screen from scrolling back, set false
    FunctionStringBox* functionPickedUp;	// picked up from the bottom UI panel
    int funcStringBoxIndex;	// the index of the picked up box (so we know where in the list to reposition boxes)
    std::vector<BoxBar*> boxBars;	// bars between placed functions in the bottom UI
    FunctionStringBox* tempBoxCreated;	// contains the temp box when hovering over a boxbar with a function
    int tempIndex;	// the position of an insertion
    bool overTrashcan = false;

    // map //

    bool goButtonPressed;
    std::vector<Tile*> tiles;
    Tile* robotTile;	// the tile the robot is on (the backup incase we need to back track due to delete or insert)
    Tile* currentTile;	// the tile the current string of moves would take the robot to
    std::vector<RouteOverlay*> routeTiles;	// holds the overlays for the route currently planned
    bool mapDrag = false;	// true when touching/click and hold on the map
    SDL_Point mapLocation;	// the point where the player will drag the map from

	// selection //

    std::pair<Texture*, SDL_Point*> functionSelect;	// the highlighting texture for functions int the left UI panel

    // private methods //

    // create a new empty box to hold a function after a placing a function
    void createFuncStringBox();
    // insert a new empty box to hold a function when hovering over a box bar
    void insertFuncStringBox(int index);
    // create a bar between placed functions that will allow insertion of functions between them
    void createInsertBar(int prevIndex, int nextIndex);
    // trace the route on the map checking that the current moves are valid
    bool isValidMove(Texture* newFunction);
    // checks for valid tile moves
    bool checkTiles(int tileIndex);
    // when a function is deleted or inserted, re-evaluate the whole string of functions
    bool checkNewFuncString();
    // divide up the textual strings that are spoken by the Dr and Prof
    void divideString(std::vector<std::string>& stringToRender, int stringNum);

    // input control //

    void downPresses(SDL_Point& touchLocation);
    void pressReleases(SDL_Point& touchLocation);
    void motion(SDL_Point& touchLocation, SDL_Rect& camera);

    void downLeftUI(SDL_Point& touchLocation);
    void downBottomUI(SDL_Point& touchLocation);
    void downMainWindowUI(SDL_Point& touchLocation);

    void upBottomUI(SDL_Point& touchLocation);

    void motionLeftUI(SDL_Point& touchLocation);
    void motionBottomUI(SDL_Point& touchLocation);
    void motionMainWindowUI(SDL_Point& touchLocation, SDL_Rect& camera);

    void okButton(SDL_Point& touchLocation);

public:
    // constructor
    UI(SDL_Renderer* rend, SDL_Rect* viewportMain, std::vector<Tile*> tileSet);

    // destructor
    ~UI();

    // getters //

    std::list<Function*>& getFunctionsList() { return functions; }

    bool getExit() const { return quitGame; }
    bool getGoButtonPressed() const { return goButtonPressed; }
    Tile*& getCurrentTile() { return currentTile; }
    bool getEndOfMission() const { return endOfMission; }
    bool getOkActive() const { return okActive; }
	bool getOkPressed() const { return okPressed; }


    // setters //

	void setDigState(bool state) { dig = state; }
    void setCurrentStage(MissionStages stage) { currentStage = stage; }
    void setGoButtonPressed(bool boolToSet) { goButtonPressed = boolToSet; }
    // set the start position for the next function string
    void setRobotTile(Tile*& current) { robotTile = current; }
    void setCurrentTile(Tile*& current) { currentTile = current; }

    // delete all the route overlays ready for a fresh string
    void resetRouteMap();
    // reset the completed flag back to false on all functions
    void resetFunctions();

    void processInventory(TileAndDirection* info);

    // what ever media we need is loaded in here
    bool loadMedia();

    // render the speech bubbles
    bool renderText(int moves);
    bool LoadScript(std::string filePath, std::vector<MissionText*>& missionScript);
    void getNextLine();

    // handling touch input
    void touchInputHandler(SDL_Event& e, float& frameTime, SDL_Point& touchLocation, SDL_Rect& camera,
        const SDL_Rect& screenSize);

    // handling mouse input
    void mouseInputHandler(SDL_Event& e, float& frameTime, SDL_Point& touchLocation, SDL_Rect& camera);

    // handle inputs for when speech is on the screen (can only press ok)
    void okButtonActiveOnly(SDL_Event& event, float& frameTime, SDL_Point& touchLocation,
        SDL_Rect& camera, const SDL_Rect& screenSize);

    // render the UI to the screen
    void render(SDL_Point& touchLocation, SDL_Rect &camera);


    void renderScoreScreen();
};
