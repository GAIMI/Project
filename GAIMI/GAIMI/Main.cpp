#include <SDL.h>
#include "Gaimi.h"


int main(int argc, char** argv)
{
	// create a game instance
	Gaimi* game = new Gaimi;

	game->run();

	return 0;
}