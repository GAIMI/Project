#pragma once

#include <SDL.h>
#include <string>
#include "Texture.h"

class Tile;


////////////// SCREEN //////////////

// 1920 x 1080 - activtable resolution - 16:9
const SDL_Rect SCREEN_SIZE = { 0, 0, 1920, 1080 };
//const SDL_Rect SCREEN_SIZE = { 0, 0, 1600, 900 };


////////////// GAME ////////////////

enum class GameStates
{
	QUIT,
	RUN_MENU,
	RUN_GAME,
	BACK_TO_MENU,
	NEXT_MISSION,
	RESTART_MISSION,
	CREATE_MENU,
	CREATE_GAME
};

enum class MissionStages
{
	INTRO,
	BRIEF,	// get given your mission
	MISSION,	// get given extra information along the way
	DEBRIEF	// after completing the mission
};

struct MissionText
{
	std::string line;
	int code;
};


////////////// MEDIA ///////////////

const std::string LEFT_UI = "Media\\Left_UI.png";
const std::string BOTTOM_UI = "Media\\BottomUI.jpg";

const std::string GO_BUTTON_UI = "Media\\GoButton.png";
const std::string OK_BUTTON_UI = "media\\OkButton.png";

const std::string FUNCTION_1 = "Media\\UpArrow.png";
const std::string FUNCTION_2 = "Media\\DownArrow.png";
const std::string FUNCTION_3 = "Media\\LeftArrow.png";
const std::string FUNCTION_4 = "Media\\RightArrow.png";
const std::string FUNCTION_5 = "Media\\DigSample.png";
const int NUM_FUNCTIONS = 5;	// how many different functions will be available to choose from in the left UI panel

const std::string LIST_MOVE = "Media\\ListMove.jpg";
const std::string EMPTY_BOX = "Media\\EmptyBox.png";
const std::string BOX_BAR = "Media\\BoxBar.png";
const std::string TRASHCAN = "Media\\TrashcanClosed.png";
const std::string OPEN_TRASHCAN = "Media\\TrashcanOpen.png";
const std::string TILE_OVERLAY_RED = "Media\\redOverlay.png";
const std::string TILE_OVERLAY_BLUE = "Media\\blueOverlay.png";


//////////// score Screen ////////////

const std::string SCORE_BACKGROUND_FILE = "Media\\ScoreBoardBackground2.png";
const std::string BLANK_STAR_FILE = "Media\\blankStar.png";
const std::string FULL_STAR_FILE = "Media\\fullStar.png";
const int NUM_OF_STARS = 3;
const std::string CONTINUE_BUTTON_FILE = "Media\\ContinueButton.png";


const SDL_Rect SCORE_BOARD_RECT{
    (SCREEN_SIZE.w / 4),
    SCREEN_SIZE.h / 6,
    1000,
    650 };

const SDL_Rect CONTINUE_BUTTON_RECT{
    SCORE_BOARD_RECT.x + SCORE_BOARD_RECT.w - 138 - 20,
    SCORE_BOARD_RECT.y + SCORE_BOARD_RECT.h - 76,
    138,
    76 };




////////////// ROBOT ///////////////

const std::string ROBOT_TEX = "Media\\Robot.png";

const float ROBOT_WIDTH = 185.0f;
const float ROBOT_HEIGHT = 142.0f;

const float START_XPOS_DEFAULT = 300.0f;
const float START_YPOS_DEFAULT = 300.0f;

const float ROBOT_MOVEMENT_DISTANCE = 200.0f;

enum class Directions
{
	NORTH,
	SOUTH,
	EAST,
	WEST
};


////////////// MAP /////////////////

struct Maps
{
	std::string mapFile;
	std::string mapMap;
	std::string missionScript;
	MissionStages first;
};
const int NUMBER_OF_MAPS = 3;

const Maps MAPS[NUMBER_OF_MAPS]{
	{ "Media\\IonisingRaditionMapLogo.png", 
	"Media\\mars1.map", "Media\\MissionScript1.txt", MissionStages::INTRO },
	{ "Media\\NitrateMapLogo.png", 
	"Media\\mars2.map", "Media\\MissionScript2.txt", MissionStages::BRIEF },
	{ "Media\\WaterMapLogo.png", 
	"Media\\mars3.map", "Media\\MissionScript3.txt", MissionStages::BRIEF }
};

const std::string TILE_SHEET_1 = "Media\\MarsSheet.png";

//The dimensions of the level
const int LEVEL_WIDTH = 3000;
const int LEVEL_HEIGHT = 3000/*2024*/;

//floor tiles for the level
const int TILE_WIDTH = 200;
const int TILE_HEIGHT = 200;

//Tile constants
const int TOTAL_TILES = (LEVEL_WIDTH / TILE_WIDTH) * (LEVEL_HEIGHT / TILE_HEIGHT);
const int TOTAL_TILE_TYPES = 21;

const int PASSABLE = 0;
const int CRATE = 1;
const int BRIDGE_CRATE = 2;
const int SURVIVOR = 3;
const int DIG_SITE = 4;
const int BRIDGE_VERT = 5;
const int BRIDGE_HORIZ = 6;
const int BRIDGE_CAP_LEFT = 7;
const int BRIDGE_CAP_TOP = 8;
const int BRIDGE_CAP_RIGHT = 9;
const int BRIDGE_CAP_BOTTOM = 10;
const int CANYON_CAP_LEFT = 11;
const int CANYON_CAP_TOP = 12;
const int CANYON_CAP_RIGHT = 13;
const int CANYON_CAP_BOTTOM = 14;
const int CANYON_CORNER_BOT_LEFT = 15;
const int CANYON_CORNER_TOP_LEFT = 16;
const int CANYON_CORNER_TOP_RIGHT = 17;
const int CANYON_CORNER_BOT_RIGHT = 18;
const int CANYON_VERT = 19;
const int CANYON_HORIZ = 20;


////////////// UI //////////////////

const int UI_LEFT_WIDTH = 260;
const int UI_LEFT_HEIGHT = SCREEN_SIZE.h;
const SDL_Rect UI_LEFT = { 0, 0, UI_LEFT_WIDTH, UI_LEFT_HEIGHT };

const int UI_BOTTOM_WIDTH = SCREEN_SIZE.w - UI_LEFT_WIDTH;
const int UI_BOTTOM_HEIGHT = 160;
const SDL_Rect UI_BOTTOM = { UI_LEFT_WIDTH, SCREEN_SIZE.h - UI_BOTTOM_HEIGHT, UI_BOTTOM_WIDTH, UI_BOTTOM_HEIGHT };

const int UI_MAIN_WIDTH = SCREEN_SIZE.w - UI_LEFT_WIDTH;
const int UI_MAIN_HEIGHT = SCREEN_SIZE.h - UI_BOTTOM_HEIGHT;
const SDL_Rect UI_MAIN = { UI_LEFT_WIDTH, 0, UI_MAIN_WIDTH, UI_MAIN_HEIGHT };

const int FUNCTION_WIDTH = 100;
const int FUNCTION_HEIGHT = 100;
const int FUNCTION_SPACING_LEFT = 20;	// between functions in the left UI
const int FUNCTION_SPACING_BOTTOM = 30;	// between functions in the bottom UI

const int BOX_BAR_WIDTH = 10;	// bar between functions in the bottom UI panel

const int TRASHCAN_WIDTH = 118;
const int TRASHCAN_HEIGHT = 144;

const int NUM_FUNC_STRING_BOXES = 9;	// how many boxes will be displayed on the screen at any one time
const int BOX_START_X_POS = UI_LEFT_WIDTH + 20 + FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM;	// the x position at the right side of the left set of iterate arrows

// top left corner position of the iterator arrows in the bottom UI panel (with relation to the viewport, not the screen)
const int ARROWS_LEFT_X_POS = 20;
const int ARROWS_LEFT_Y_POS = FUNCTION_SPACING_BOTTOM;
const int ARROWS_RIGHT_X_POS = 20 + (NUM_FUNC_STRING_BOXES + 1) * (FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM);
const int ARROWS_RIGHT_Y_POS = FUNCTION_SPACING_BOTTOM;

const SDL_Rect GO_BUTTON{
	ARROWS_RIGHT_X_POS + FUNCTION_WIDTH + FUNCTION_SPACING_BOTTOM / 2,
	FUNCTION_SPACING_BOTTOM,
	100,
	100 };

const int INVENTORY_MAX_SIZE = 4;	// how many items the robot can carry at once
const int INVENTORY_START_HEIGHT = 400;

const std::string EXIT_BUTTON_FILE = "Media\\ExitButton.png";
const std::string RESET_BUTTON_FILE = "Media\\ResetButton.png";

const SDL_Rect EXIT_BUTTON{
	20,
	SCREEN_SIZE.h - 80,
	80,
	72 };

const SDL_Rect RESET_BUTTON{
	120,
	SCREEN_SIZE.h - 70,
	120,
	53 };


////////// SPEECH TEXT ///////////////

const std::string DR_O = "Media\\DrO.png";
const std::string PROF_B = "Media\\ProfB.png";
const std::string TTF_FILE = "Media\\font.ttf";
const std::string SPEECH_BUBBLE = "Media\\SpeechBubble.png";

const int STRING_LENGTH_MAX = 30;	// the max string length before attempting to divide up
const int TEXT_HEIGHT = 45;	// the space between the top of 1 line of text to the top of the next line

// Dr Ogel, speech bubble and button //

const SDL_Point DR_O_POS{ 1000, 200 };	// master position for Dr Ogel and his speech components
const SDL_Point SPEECH_BOX_DR{ DR_O_POS.x - 750, DR_O_POS.y + 50 };	// the bubble
const SDL_Point SPEECH_DR{ DR_O_POS.x - 730, DR_O_POS.y + 70 };	// the text

// ok button for Dr Ogel
const SDL_Rect OK_BUTTON_DR{
	DR_O_POS.x - 107,
	DR_O_POS.y + 198,
	100,
	100 };


// Prof Blue's speech bubble and button //

const SDL_Point SPEECH_BOX_PROF{ 0, UI_MAIN_HEIGHT - 300 };	// the bubble (master position)
const SDL_Point SPEECH_PROF{ SPEECH_BOX_PROF.x + 70, SPEECH_BOX_PROF.y + 20 };	// the text

// ok button for Prof Blue
const SDL_Rect OK_BUTTON_PROF{
	SPEECH_BOX_PROF.x + 700,
	SPEECH_BOX_PROF.y + 147,
	100,
	100 };


////////// FUNCTION CONTROL ///////////

// the function held in the function string boxes
struct Function
{
	Texture* tex;
	bool complete;
};

// the boxes in the bottom UI panel
struct FunctionStringBox
{
	SDL_Rect position;
	Texture* emptyBox;
	Function* function;
};

// the bars between placed functions in the bottom UI panel
struct BoxBar
{
	Texture* tex;
	SDL_Rect position;
	int prevIndex;
	int nextIndex;
};


//////////// INVENTORY ///////////////

const std::string SUPPLY_ICON = "Media\\SupplyCrate.png";
const std::string BRIDGE_ICON = "Media\\Bridge.png";
const std::string SURVIVOR_ICON = "Media\\survivorIcon.png";

struct InventoryItem
{
	Texture* tex;
};

struct TileAndDirection
{
	Tile* tile;
	Directions dir;
};


//////////// ROUTE MAP ///////////////

struct RouteOverlay
{
	Texture* tex;
	SDL_Point location;
};



/////////// WELCOME SCREEN /////////////

const std::string BACKGROUND_FILE = "Media\\Background.png";
const std::string STARTBUTTON_FILE = "Media\\start.png";
const std::string TITLE_FILE = "Media\\title.png";
const std::string CREDITS_FILE = "Media\\Credits.png";
const std::string CREDITS_BUTTON_FILE = "Media\\CreditsButton.png";

const int MAP_SPACING = 250;
const int MAP_WIDTH = 400;
const int MAP_HEIGHT = 400;
const int HEIGHT_OFFSET = 200;
const int COLLISION_OFFSET = 50;

const int START_WIDTH = 300;
const int START_HEIGHT = 150;

const int TITLE_WIDTH = 800;
const int TITLE_HEIGHT = 130;

const SDL_Rect CREDITS_BUTTON{
    SCREEN_SIZE.w - 200,
    SCREEN_SIZE.h - 81,
    200,
    81 };


struct MapTexture
{
	Texture* tex;
	int x;
	int y;
};