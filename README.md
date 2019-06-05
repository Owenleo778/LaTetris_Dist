# What is this?
A simple of game of Tetris to be played on the LaFortuna Board 

## Instructions

- Use the left and right directions to move where the block is falling down.
- You can press the up direction in order to rotate the block by 90° to the right (if possible).
- You can press (and hold) the down direction in order to make the block fall faster.

## Flash Instructions

In order to be able to play, you need to 'flash' a .hex file of the game onto your board.

- Plug in the LaFortuna to your computer using the USB to Mini-USB cable.
- Press the S6 button at the top of the LaFortuna. The screen should turn off and a connection made sound should play.
- Open the command line in the directory containing the hex file.
- Enter dfu-programmer.exe at90usb1286 erase.
- Enter dfu-programmer.exe at90usb1286 flash main.hex.

Note, if you do not have a working .hex file, you can use the supplied makefile (or create your own) to compile one.
