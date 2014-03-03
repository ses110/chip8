#ifndef CHIP8
#define CHIP8

#include <stdint.h>

#define HEIGHT 32
#define WIDTH 64

typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;


class chip
{
public:
	/*
	Chip-8's display is 64 pixels wide, 32 pixels high.
	screen[row][column] (ROW-Major order)
	*/
	u8 display[HEIGHT][WIDTH];
	u8 keyboard[16];
	
    chip(const char* ROM);

	void loadROM(const char* fname);
	void execute();
    void timerdown();
private:
	/* 
	4096 bytes of RAM available
		0x000 to 0xFFF interpreter only
		load ROMs starting at this address: 0x200 
		load ETI 600 programs starting at this address: 0x600
	*/
	u8 RAM[0x1000];

	u16 PC;
	
	/*
	16 8-bit registers, called VX[0] up to VX[15]
	*/
	u8 VX[0x10];

	/*
	Special register that stores memory addresses.
	*/
	u16 VI;

	u16 stack[16];

	//Stack pointer keeps track of the top of the stack
	u8 SP;

	/* Sprites representing 16 different hex digits 
	      in 8w by 5h pixel dimensions. */
	static const u8 sprite_digits[16][5];

	int delaytimer;
	int soundtimer;

	/*
	* reset
	*	Return to clean state
	*/
	void reset();
	
	/*
	* CLS
	*	Clear the screen
	*/
	void CLS();
};

#endif
