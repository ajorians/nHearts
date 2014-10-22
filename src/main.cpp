extern "C"
{
	#include <os.h>
	#include "SDL/SDL.h"
	#include "ArchiveLib/ArchiveLib.h"
}

#include "Menu.h"
#include "Game.h"
#include "CardImages.h"
#include "Config.h"
#include "Options.h"
#include "Help.h"

#define SCREEN_BIT_DEPTH        (16)
#define SCREEN_VIDEO_MODE (SDL_SWSURFACE|SDL_FULLSCREEN|SDL_HWPALETTE)

int main(int argc, char *argv[]) 
{
	printf("Initializing SDL.\n");
	
	/* Initialize the SDL library (starts the event loop) */
    if ( SDL_Init(SDL_INIT_VIDEO ) < 0 )
	{
        fprintf(stderr,
                "Couldn't initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }
	
	printf("SDL initialized.\n");
	
	SDL_Surface* pScreen = NULL;
	pScreen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BIT_DEPTH, SCREEN_VIDEO_MODE);

	ArchiveSetCurrentDirectory(argv[0]);
	Config config;

	if( pScreen == NULL )
	{
		fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n", SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BIT_DEPTH, SDL_GetError());
		exit(1);
	}
	else
	{
		/* Hides the cursor */
		SDL_ShowCursor(SDL_DISABLE);

		CardImages cardImages;

		while(true)
		{
			bool bShowHelp = false, bShowOptions = false;
			SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);

			if( argc != 2 )
			{
				MainMenu menu(pScreen, &config);
				while(menu.Loop()){}
				if( menu.ShouldQuit() )
					break;
				bShowHelp = menu.ShouldShowHelp();
				bShowOptions = menu.ShowShowOptions();
			}
			
			if( bShowOptions ) {
				Options ops(pScreen, &config);
				while(ops.Loop()){}
				continue;
			}
			else if( bShowHelp )
			{
				HeartsHelp help(pScreen);
				while(help.Loop()){}
				continue;
			}
			else
			{
				//bool bPlay = true;
				Game game(pScreen, &cardImages);
				while(game.Loop()){}
				break;
			}
		}
	}
	
	printf("Quitting SDL.\n");
    
    /* Shutdown all subsystems */
    SDL_Quit();

    printf("Quitting...\n");

	return 0;
}
