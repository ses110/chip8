#include "chp.h"
#include <string.h> //memset, memclear
#include <stdlib.h> //srand
#include <time.h> // for randomization
#include <iostream> //cout
#include <iomanip> // std::hex
#include <fstream> // ifstream, file input
#include <iomanip> // cout setw (for whitespace spacing)
#include <chrono> //sleep, debugging purposes
#include <thread> //sleep, debug purposes

const u8 chip::sprite_digits[16][5] =
	{{0xF0, 0x90, 0x90, 0x90, 0xF0},
	{0x20, 0x60, 0x20, 0x20, 0x70},
	{0xF0, 0x10, 0xF0, 0x80, 0xF0},
	{0xF0, 0x10, 0xF0, 0x10, 0xF0},
	{0x90, 0x90, 0xF0, 0x10, 0x10},
	{0xF0, 0x80, 0xF0, 0x10, 0xF0},
	{0xF0, 0x80, 0xF0, 0x90, 0xF0},
	{0xF0, 0x10, 0x20, 0x40, 0x40},
	{0xF0, 0x90, 0xF0, 0x90, 0xF0},
	{0xF0, 0x90, 0xF0, 0x10, 0xF0},
	{0xF0, 0x90, 0xF0, 0x90, 0x90},
	{0xE0, 0x90, 0xE0, 0x90, 0xE0},
	{0xF0, 0x80, 0x80, 0x80, 0xF0},
	{0xE0, 0x90, 0x90, 0x90, 0xE0},
	{0xF0, 0x80, 0xF0, 0x80, 0xF0},
	{0xF0, 0x80, 0xF0, 0x80, 0x80}};

chip::chip(const char* ROM) {
	// using namespace std;

	// cout << "entering chip() constructor." << endl;
	reset();
	loadROM(ROM);
	//while(PC < 0x0200 + 246){
	//	execute();
	//	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	//}
	// for(int i = 0; i< 80; i++)
	// 	std::cout << "["<<i<<"]"<< std::hex << (int)(RAM[i]) << std::endl;
}
void chip::reset() {
	using namespace std;
    CLS();

	for(int i=0; i < 16; i++) {
		for(int j=0; j < 5; j++) {
			RAM[5*i+j] = sprite_digits[i][j];
		}
	}

	memset(VX, 0, 16);
	VI = 0;

	memset(stack, 0, 16);
	SP = 0;

	delaytimer = 0;
	soundtimer = 0;

	memset(keyboard, 0, 16);

	PC = 0x0200;
}
void chip::timerdown() {
    if(delaytimer > 0) --delaytimer;
    if(soundtimer > 0) --soundtimer;
}
void chip::CLS() {
	memset(display, 0, sizeof(display[0][0])*WIDTH*HEIGHT);
}

void chip::loadROM(const char* fname) {
	std::cout << "\t Entering loadROM()" << std::endl;
	std::cout << "with filename: " << fname << std::endl;
	
	std::ifstream fh_ROM;
	fh_ROM.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		fh_ROM.open(fname, std::ios::in | std::ios::binary | std::ios::ate);
		if(fh_ROM.is_open()) {
			std::cout << "ROM was read successfully." << std::endl;
			std::ifstream::pos_type size = fh_ROM.tellg();
			if(size <= (0xFFF-0x200)) {
				fh_ROM.seekg(0, std::ios::beg);
				fh_ROM.read(reinterpret_cast<char*>(&RAM[0x200]), size);
				std::cout << "file size: " << size << " bytes" << std::endl;
				// cout.setf(ios::hex, ios::basefield);
				// for(int i = 0x200; i < (0x200+size); i++)
				// 	cout << dec << "["<<i<<"] "<< hex << setw(2) << static_cast<int>(RAM[i]) << endl;
			} else
				 throw std::ifstream::failure("File size too large.");
		}
		
		fh_ROM.close();
	}
	catch (std::ifstream::failure e) {
		std::cerr << "Error opening file: " << e.what() << '\n'; 
	}
}
void chip::execute() {
	using namespace std;

    bool debug = false;

	u16 opcode;
	if(PC >= 0x0200 && PC <= 0x0FFF) {
		opcode = (RAM[PC] << 8) | RAM[PC+1];
		PC += 2;
		
        if(debug) cout << "\naddress: 0x" << hex << PC << "\topcode: 0x" << hex << (int)opcode << endl;

		//opcode = 0xF30A;
		//keyboard[14] = 1;

		u8 x = 0,
            set_x = 0;
		u8 y = 0,
            set_y = 0;
		u16 nnn=0,
            carry=0;

		switch ((opcode & 0xF000) >> 12)
		{
            if(debug) 
		    	cout << ((opcode & 0xF000)  >> 12) << endl;
			case (0x0):
			    if(debug) 
            	cout << "0: 0x00XX -> case 0: " << endl;
				if(debug) 
                cout << (opcode & 0x00FF) << endl;
				if ( (opcode & 0x00FF) == 0xE0) {
					if(debug) cout << "Clear screen" << endl;
					CLS();
					break;
				}
				else
				if ((opcode & 0x00FF) == 0xEE) 
				{
					if(debug) cout << "Return from subroutine" << endl;
					//TODO: corner case sp = 0 to -1?
                    if(debug) cout << "Return to address: " << (int)stack[SP-1] << endl;
					PC = stack[--SP];
					break;
				}
				else
				{	
					if(debug) cout << "0: Jump to routine @ 0x" << (0x0FFF & opcode) << endl;
					nnn = 0x0FFF & opcode;
					stack[SP++] = PC;
					PC = nnn;
				}
				break;
			case (0x1):
				if(debug) cout << "1: Jump to " << (0x0FFF & opcode) << endl;
				nnn = 0x0FFF & opcode;
				PC = nnn;
				break;
			case (0x2):
				nnn = 0x0FFF & opcode;
				if(debug) cout << "2: Call subroutine @ " << nnn << endl;
				stack[SP++] = PC;
				PC = nnn;
				break;
			case (0x3):
				if(debug) cout << "3: 3xnn Skip next instruction if Vx == nn" << endl;
				nnn = opcode & 0x00FF;
				x = ((opcode & 0x0F00) >> 8);
				VX[x] = nnn;
				if (VX[x] == nnn)
					PC += 2;
				if(debug) cout << "x: " << dec << (int)x << std::endl;
				if(debug) cout << "nnn: " << dec << (int)nnn << std::endl;
				if(debug) cout << "V[" << dec << (int)x << "] = " << (int)VX[x] << std::endl;
				break;
			case (0x4):
				if(debug) cout << "4: Skip next instruction if V[x] != nn" << endl;
				nnn = opcode & 0x00FF;
				x = ((opcode & 0x0F00) >> 8);
				VX[x] = nnn;
				if (VX[x] != nnn)
					PC += 2;
				if(debug) cout << "x: " << dec << (int)x << std::endl;
				if(debug) cout << "nnn: " << dec << (int)nnn << std::endl;
				if(debug) cout << "V[" << dec << (int)x << "] = " << (int)VX[x] << std::endl;
				break;
			case (0x5):
				if(debug) cout << "5: 0x5XY0 Skip next instruction if Vx == Vy." << endl;
				x = ((opcode & 0x0F00) >> 8);
				y = ((opcode & 0x00F0) >> 4);
				if(VX[x] == VX[y])
					PC += 2;
				break;
			case (0x6):
				if(debug) cout << "6: 0x6xkk : Set Vx = kk. Set register to a value" << endl;
				x = ((opcode & 0x0F00) >> 8);
				nnn = (opcode & 0x00FF);
				VX[x] = nnn;
		 		break;
			case (0x7):
				if(debug) cout << "7: 0x7XKK : Set vx = vx+kk" << endl;
				x = ((opcode & 0x0F00) >> 8);
				nnn = (opcode & 0x00FF);
				VX[x] += nnn;
				break;
			case (0x8):
				switch(opcode & 0x000F) 
				{
					case 0:
						if(debug) cout << "0x8xy0: Stores the value of register Vy in register Vx." << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        y = ((opcode & 0x00F0) >> 4);
                        VX[x] = VX[y];
						break;
					case 1:
						if(debug) cout << "0x8xx1: Set Vx = Vx OR Vy." << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        y = ((opcode & 0x00F0) >> 4);
                        VX[x] = VX[x] | VX[y];
						break;
					case 2:
						if(debug) cout << "0x8xx2: Set Vx = Vx AND Vy." << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        y = ((opcode & 0x00F0) >> 4);
                        VX[x] = VX[x] & VX[y];
						break;
					case 3:
						if(debug) cout << "0x8xx3: Set Vx = Vx XOR Vy." << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        y = ((opcode & 0x00F0) >> 4);
                        VX[x] = VX[x] ^ VX[y];
						break;
					case 4:
						if(debug) cout << "0x8xx4: Set Vx = Vx + Vy, set VF = carry if sum is greater than 255" << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        y = ((opcode & 0x00F0) >> 4);
                        carry = VX[x] + VX[y];
                        if(carry > 255)
                            VX[0xF] = 1;
                        VX[x] = (carry & 0xFF); 
						break;
					case 5:
						if(debug) cout << "0x8xx5: Set Vx = Vx - Vy, set VF = NOT borrow."
                            << "If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, and the results stored in Vx." << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        y = ((opcode & 0x00F0) >> 4);
                        if(VX[x] >= VX[y])
                            VX[0xF] = 1;
                        else
                            VX[0xF] = 0;
                        VX[x] = VX[x] - VX[y];
						break;
					case 6:
						if(debug) cout << "0x8xx6: Set Vx = Vx SHR 1."
                            << "If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is divided by 2." << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        if((VX[x] & 0x01) == 1)
                            VX[0xF] = 1;
                        VX[x] = VX[x] >> 1;
						break;
					case 7:
						if(debug) cout << "0x8xx7: Set Vx = Vy - Vx, set VF = NOT borrow."
                             << "If Vy > Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, and the results stored in Vx." << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        y = ((opcode & 0x00F0) >> 4);
                        if(VX[y] >= VX[x])
                            VX[0xF] = 1;
                        VX[x] = VX[y] - VX[x];
						break;
					case 0xE:
                        //TODO: check acc
						if(debug) cout << "0x8xxE: Set Vx = Vx SHL 1."
                             << "If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is multiplied by 2." << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        y = ((opcode & 0x00F0) >> 4);
                        if((VX[x] & 0x80) == 1)
                            VX[0xF] = 1;
                        VX[x] = VX[x] << 1;
						break;
				}
				break;
			case (0x9):
				if(debug) cout << "9: Skip next instruction if" << endl;
				x = ((opcode & 0x0F00) >> 8);
				y = ((opcode & 0x00F0) >> 4);
				if(VX[x] != VX[y])
					PC += 2;
				break;
			case (0xA):
				if(debug) cout << "A: Set I = nnn" << endl;
				nnn = (opcode & 0x0FFF);
				VI = nnn;
				break;
			case (0xB):
				if(debug) cout << "B: 0xBNNN: Jump to location nnn + V0." << endl;
				nnn = (opcode & 0x0FFF);
				PC = nnn + VX[0];
				break;
			case (0xC):
				if(debug) cout << "C: 0xCXKK Set Vx = random byte AND kk" << endl;
				x = ((opcode & 0x0F00) >> 8);
				nnn = (opcode & 0x00FF);
                srand(time(0));
				//TODO: check accuracy 
				VX[x] = nnn & (rand() % 255);
				break;
			case (0xD):
				if(debug) cout << "D: 0xDXYN: Draw sprite at (VX, VY) with a width of 8 px and height of N pixels." << endl;
                //TODO: check implementation
                VX[0xF] = 0;
				x = ((opcode & 0x0F00) >> 8);
				y = ((opcode & 0x00F0) >> 4);
                nnn = (opcode & 0x000F);
                if(debug) cout << "Display X = " << (int)VX[x] << hex << endl << 
                        "Display Y = " << (int)VX[y] << endl << 
                        "Nnn bytes (height): " << nnn << endl; 
                for(int i = 0; i < nnn; i++) { //Each iteration works through each height
                    //At particular height, at VX+ curr_x, xor pixel
                    //We ad bytes from memory starting at address stored in I.
                    for(int c_bit = 0; c_bit < 8; ++c_bit) {  
                        set_x = (VX[x] + c_bit) % (WIDTH);
                        set_y = (VX[y] + i) % (HEIGHT);
                        if((RAM[VI+i] & (0x80 >> c_bit)) != 0) { 
                            if(display[set_y][set_x] == 1)
                                VX[0xF] = 1;
                            display[set_y][set_x] ^= 1;
                        }                
                    }
                }
				break;
			case (0xE):
				if(debug) cout << "E:" << endl;
				if  ((opcode & 0x00FF) == 0x9E) {
					if(debug) cout << "0xEX9E: Skip next instruction if key with the value of VX is pressed." << endl;
                    x = ((opcode & 0x0F00) >> 8);
                    if(keyboard[VX[x]] == 1);
                        PC += 2;
                }
				else
                {
				    if ((opcode & 0x00FF) == 0xA1) {
					    if(debug) cout << "0xEXA1: Skip next instruction if key with value of VX is <<<NOT>>> pressed." << endl;
                        x = ((opcode & 0x0F00) >> 8);
                        if(keyboard[VX[x]] == 0);
                            PC += 2;
                    }
                }
				break;
			case (0xF):
				if(debug) cout << "F:" << endl;
				switch(opcode & 0x00FF)
				{
					case 0x07:
						if(debug) cout << "0xFX07: Set Vx = delay timer value." << endl;
						x = ((opcode & 0x0F00) >> 8);
						VX[x] = delaytimer;
						break;
					case 0x0A:
						if(debug) cout << "0xFX0A: Wait for a key press, store the value of the key in Vx." << endl;
                        
                        PC -= 2;
						
                        for(nnn=0; nnn < 16; nnn++) {
							if(debug) cout << "waiting for a key..." << endl;
							if(keyboard[nnn]==1) {
						        x = ((opcode & 0x0F00) >> 8);
						        VX[x] = nnn;
                                PC+=2;
						        if(debug) cout << "key " << nnn << " was pressed" << endl;
                                break;
                            }
						}
						break;
					case 0x15:
						if(debug) cout << "0xFX15: Set delay timer = Vx." << endl;
						x = ((opcode & 0x0F00) >> 8);
						delaytimer = VX[x];
						break;
					case 0x18:
						if(debug) cout << "0xFX18: Set sound timer = Vx." << endl;
						x = ((opcode & 0x0F00) >> 8);
						soundtimer = VX[x];
						break;
					case 0x1E:
						if(debug) cout << "0xFX1E: Set I = I + Vx" << endl;
						x = ((opcode & 0x0F00) >> 8);
						if(VI += VX[x] > 0xFFF) VX[0xF] = 1;
                        else VX[0xF] = 0;
						break;
					case 0x29:
						if(debug) cout << "0xFX29: Set I = location of sprite for digit Vx." << endl;
						x = ((opcode & 0x0F00) >> 8);
						VI = VX[x] * 5;
						break;
					case 0x33:
                        //TODO check acc
						if(debug) cout << "0xFX33: Store BCD representation of Vx in memory locations I, I+1, and I+2." << endl;
						x = ((opcode & 0x0F00) >> 8);
						RAM[VI]   = (VX[x]%1000)/100; // Most significant digit
						RAM[VI+1] = VX[x]%1000%100/10;
						RAM[VI+2] = VX[x] %1000%100%10/1; // Least significant digit

						break;
					case 0x55:
						if(debug) cout << "0xFX55: Store registers V0 through Vx in memory starting at location I." << endl;
						x = ((opcode & 0x0F00) >> 8);
                        for(int i=0; i <= x; i++) {
                            RAM[VI+i] = VX[i];
                        }
						break;
					case 0x65:
						if(debug) cout << "0xFX65: Read registers V0 through Vx from memory starting at location I." << endl;
						x = ((opcode & 0x0F00) >> 8);
                        for(int i=0; i <= x; i++) {
                            VX[i] = RAM[VI+i];
                        }
						break;
				}
				break;
			default:
				break;
		};
	} else {
		cerr << "PC address offrange: SEG FAULT." << endl;
        exit(0);
	}
    if(debug) {
        for(int i=0; i < 16; i++)
            cout << "V[" << i << "] = " << hex << (int)VX[i] << endl; 
        for(int i=0; i < 16; i++)
            cout << "Stack[" << i << "]: " << (int)stack[i] << endl;
        cout << "VI = " << VI << endl << endl;
    }
}
#include "chp.h"
