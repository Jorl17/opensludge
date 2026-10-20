SLUDGE Development Kit for macOS
================================

With the Dev Kit, you make SLUDGE games. Open SLUDGE Project Manager to
create or open a project, compile it and run the game. The other tools are:

  SLUDGE Sprite Bank Editor    edit sprite banks (.duc)
  SLUDGE Floor Maker           draw the floors of rooms (.flo)
  SLUDGE Z-Buffer Maker        make z-buffers (.zbu)
  SLUDGE Translation Editor    translate the text of a game (.tra)

When you click Run Game in the project manager, we start the game with
OpenSLUDGE, the SLUDGE engine in this folder. When you open a file of the
project in the project manager, we open it in the tool for files of its
type, or in the app that macOS opens files of that type with.

To compile a project from the Terminal, run sludge-compiler, which is in
the project manager app:

  "SLUDGE Project Manager.app/Contents/MacOS/sludge-compiler" game.slp

You need macOS 15 or later, on a Mac with Apple Silicon.

We keep the settings of the Dev Kit in ~/.config/sludge-devkit.

If macOS says that an app can't be opened because it is from an
unidentified developer, or that it is damaged, Control-click the app,
choose Open, then choose Open again. On recent versions of macOS, you may
have to go to System Settings > Privacy & Security and click "Open Anyway"
instead. To do this for all the apps at once, run this in the Terminal, in
the folder that contains the Dev Kit folder:

  xattr -dr com.apple.quarantine "OpenSLUDGE Dev Kit"

The Dev Kit is open source, under the GNU GPL 3 or later, and the engine
under the GNU LGPL 2.1: https://github.com/opensludge/opensludge
The Dev Kit uses GTK 3 and the libraries it depends on, which are in the
Frameworks folder of each app. GTK is under the GNU LGPL:
https://www.gtk.org
