Chip8 emulator
=====

Personal project written to emulate simple games originally written for the chip-8 interpreter. 
The chip-8 was a form of "virtual machine" to make it easier to write games for old microcomputers 
made in the 1970's such as the COSMAC VIP.

To compile, you need the SDL library installed in your system. Use the makefile and type:

  make sdl

or the equivalent command:

  g++ -pedantic -std=c++0x -Wall chp.cpp testchp.cpp -o chip8 -lSDL
  
To run a game, type:

chip8 GAMEFILE
