SLUDGE Development Kit for Windows
==================================

With the Dev Kit, you make SLUDGE games. Open sludge-projectmanager.exe to
create or open a project, compile it and run the game. The other tools are:

  sludge-spritebankeditor.exe    edit sprite banks (.duc)
  sludge-floormaker.exe          draw the floors of rooms (.flo)
  sludge-zbuffermaker.exe        make z-buffers (.zbu)
  sludge-translationeditor.exe   translate the text of a game (.tra)
  sludge-compiler.exe            compile a project from the command line

When you click Run Game in the project manager, we start the game with
OpenSLUDGE.exe, the SLUDGE engine in this folder. Keep all the files of
this folder together.

You need 64-bit Windows 10 or later. For the floor maker, the z-buffer
maker and the sprite bank editor, you also need graphics with OpenGL 3.2.

We keep the settings of the Dev Kit in the sludge-devkit folder, in your
local Application Data folder (%LOCALAPPDATA%).

If Windows SmartScreen says that it protected your PC, click "More info",
then "Run anyway".

The Dev Kit is open source, under the GNU GPL 3 or later, and the engine
under the GNU LGPL 2.1: https://github.com/opensludge/opensludge
The Dev Kit uses GTK 3 and the libraries it depends on, whose DLLs are in
this folder. GTK is under the GNU LGPL: https://www.gtk.org
