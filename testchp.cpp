#include "chp.h"
#include <iostream>
#include "SDL/SDL.h"

#include <chrono>
#include <thread>
#include <iomanip> // cout setw() 

void draw(chip& emul, SDL_Surface* screen) {
	SDL_FillRect(screen, NULL, 0x000000);
	for(int y = 0; y < 32; y++) {
		for(int x = 0; x < 64; x++) 
		{
			if(emul.display[y][x] == 1) {
				SDL_Rect pix;
				pix.x = x;
				pix.y = y;
				pix.w = 1;
				pix.h = 1;
				SDL_FillRect(screen, &pix, SDL_MapRGB(screen->format, 0xff, 0xff, 0xff));
			}
		}
	}
}
void asciidraw(chip & emul) {
    using namespace std;

    cout << dec;

    cout << "_|";
    for(int i = 0; i < 64; i++)
        cout << setw(2) << i;
    cout << endl;
    for(int y = 0; y < 32; y++) { 
        cout << setw(2) << y << " ";
        for(int x = 0; x < 64; x++) {
            if(emul.display[y][x] != 0)
                cout << "0 ";
            else
                cout << ". ";
        }
        cout << endl;
    }
}
void input_handler(chip & emul, SDL_Event event, bool & quit) {
    using namespace std;
    

    switch(event.type) {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
                case SDLK_1: emul.keyboard[0x1]=1; cout << "pressed 1"; break;
                case SDLK_2: emul.keyboard[0x2]=1; cout << "pressed 2"; break;
                case SDLK_3: emul.keyboard[0x3]=1; cout << "pressed 3"; break;
                case SDLK_4: emul.keyboard[0xc]=1; cout << "pressed 4"; break;
                case SDLK_q: emul.keyboard[0x4]=1; cout << "pressed q"; break;
                case SDLK_w: emul.keyboard[0x5]=1; cout << "pressed w"; break;
                case SDLK_e: emul.keyboard[0x6]=1; cout << "pressed e"; break;
                case SDLK_r: emul.keyboard[0xd]=1; cout << "pressed r"; break;
                case SDLK_a: emul.keyboard[0x7]=1; cout << "pressed a"; break;
                case SDLK_s: emul.keyboard[0x8]=1; cout << "pressed s"; break;
                case SDLK_d: emul.keyboard[0x9]=1; cout << "pressed d"; break;
                case SDLK_f: emul.keyboard[0xe]=1; cout << "pressed f"; break;
                case SDLK_z: emul.keyboard[0xa]=1; cout << "pressed z"; break; 
                case SDLK_x: emul.keyboard[0x0]=1; cout << "pressed x"; break;
                case SDLK_c: emul.keyboard[0xb]=1; cout << "pressed c"; break;
                case SDLK_v: emul.keyboard[0xf]=1; cout << "pressed v"; break;
                default: break;
            }
            break;
        case SDL_KEYUP:
            switch(event.key.keysym.sym){
                case SDLK_1: emul.keyboard[0x1]=0; break;
                case SDLK_2: emul.keyboard[0x2]=0; break;
                case SDLK_3: emul.keyboard[0x3]=0; break;
                case SDLK_4: emul.keyboard[0xc]=0; break;
                case SDLK_q: emul.keyboard[0x4]=0; break;
                case SDLK_w: emul.keyboard[0x5]=0; break;
                case SDLK_e: emul.keyboard[0x6]=0; break;
                case SDLK_r: emul.keyboard[0xd]=0; break;
                case SDLK_a: emul.keyboard[0x7]=0; break;
                case SDLK_s: emul.keyboard[0x8]=0; break;
                case SDLK_d: emul.keyboard[0x9]=0; break;
                case SDLK_f: emul.keyboard[0xe]=0; break;
                case SDLK_z: emul.keyboard[0xa]=0; break; 
                case SDLK_x: emul.keyboard[0x0]=0; break;
                case SDLK_c: emul.keyboard[0xb]=0; break;
                case SDLK_v: emul.keyboard[0xf]=0; break;
                default: break;
            }
            break;
        case SDL_QUIT:
            quit = true;
            break;
    }
}
int main(int argc, char **argv) {
	using std::cout;
	using std::endl;

	cout << "running testchp.cpp" << endl;

	if(argc > 1) {
		chip test_c8(argv[1]);
		
        SDL_Event input;
		SDL_Surface * screen;
		
		SDL_Init(SDL_INIT_VIDEO);
		if((screen = SDL_SetVideoMode(64, 32, 24, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_RESIZABLE)) == NULL)
			cout << "\t\tERROR with SDL SETUP" << endl;
		else
			cout << "\t\tSUCCESS" << endl;
		SDL_WM_SetCaption("chip8", "chip8");
	
        bool quit = false;	
        int gettick = SDL_GetTicks();
        int numopcodes = 0;
		while(!quit)
		{
            while(SDL_PollEvent(&input)) {
                input_handler(test_c8, input, quit);
            }

            if(numopcodes < 10) {
    			test_c8.execute();
                //std::this_thread::sleep_for(std::chrono::seconds(1));
                numopcodes++;
            }
            if(SDL_GetTicks()-gettick >= 1000*10/60) {
			    test_c8.timerdown();    
                gettick = SDL_GetTicks();
                // draw(test_c8, screen);
                asciidraw(test_c8);
			    SDL_Flip(screen);
                numopcodes = 0;
		    }
        }
		SDL_FreeSurface(screen);
		SDL_Quit();
	} else {
		cout << "Not enough arguments." << endl;
	}
}
