SUBDIRS = support

bin_PROGRAMS = sludge-floormaker sludge-zbuffermaker sludge-spritebankeditor sludge-translationeditor sludge-projectmanager sludge-compiler

check_PROGRAMS = view_test sh_cmd_test
TESTS = view_test sh_cmd_test

view_test_SOURCES = \
	../tests/devkit/view_test.cpp \
	../source/DevKit/GraphicalTools/draw2d.cpp ../source/DevKit/GraphicalTools/draw2d.h \
	../source/DevKit/GraphicalTools/floormaker.cpp ../source/DevKit/GraphicalTools/floormaker.h \
	../source/DevKit/GraphicalTools/sprites.cpp ../source/DevKit/GraphicalTools/sprites.h \
	../source/DevKit/GraphicalTools/zbuffer.cpp ../source/DevKit/GraphicalTools/zbuffer.h \
	../source/DevKit/Common/moreio.cpp ../source/DevKit/Common/moreio.h \
	../source/DevKit/Common/helpers.cpp ../source/DevKit/Common/helpers.h \
	../source/CommonCode/tga.cpp ../source/CommonCode/tga.h
view_test_CPPFLAGS = $(AM_CPPFLAGS) -DVIEW_TEST_FOLDER=\"$(abs_top_srcdir)/tests/devkit\"
view_test_LDADD = @GTK_LIBS@ @LIBPNG_LIBS@ @EPOXY_LIBS@

sh_cmd_test_SOURCES = ../tests/devkit/sh_cmd_test.cpp Common.cpp Common.h
sh_cmd_test_LDADD = @GTK_LIBS@

sludge_floormaker_SOURCES = \
	FloorMakerMain.cpp SludgeFloorMaker.cpp SludgeFloorMaker.h \
	SludgeGLApplication.cpp SludgeGLApplication.h \
	../source/DevKit/GraphicalTools/draw2d.cpp ../source/DevKit/GraphicalTools/draw2d.h \
	SludgeApplication.cpp SludgeApplication.h \
	Common.cpp Common.h ../source/DevKit/Common/interface.h \
	../source/DevKit/GraphicalTools/floormaker.cpp ../source/DevKit/GraphicalTools/floormaker.h \
	../source/DevKit/GraphicalTools/sprites.cpp ../source/DevKit/GraphicalTools/sprites.h \
	../source/DevKit/Common/moreio.cpp ../source/DevKit/Common/moreio.h \
	../source/DevKit/Common/helpers.cpp ../source/DevKit/Common/helpers.h \
	../source/CommonCode/tga.cpp ../source/CommonCode/tga.h \
	../source/CommonCode/utf8.cpp ../source/CommonCode/utf8.h \
	../source/CommonCode/version.h

sludge_zbuffermaker_SOURCES = \
	ZBufferMakerMain.cpp SludgeZBufferMaker.cpp SludgeZBufferMaker.h \
	SludgeGLApplication.cpp SludgeGLApplication.h \
	../source/DevKit/GraphicalTools/draw2d.cpp ../source/DevKit/GraphicalTools/draw2d.h \
	SludgeApplication.cpp SludgeApplication.h \
	Common.cpp Common.h ../source/DevKit/Common/interface.h \
	../source/DevKit/GraphicalTools/zbuffer.cpp ../source/DevKit/GraphicalTools/zbuffer.h \
	../source/DevKit/GraphicalTools/sprites.cpp ../source/DevKit/GraphicalTools/sprites.h \
	../source/DevKit/Common/moreio.cpp ../source/DevKit/Common/moreio.h \
	../source/DevKit/Common/helpers.cpp ../source/DevKit/Common/helpers.h \
	../source/CommonCode/tga.cpp ../source/CommonCode/tga.h \
	../source/CommonCode/utf8.cpp ../source/CommonCode/utf8.h \
	../source/CommonCode/version.h

sludge_spritebankeditor_SOURCES = \
	SpriteBankEditorMain.cpp SludgeSpriteBankEditor.cpp SludgeSpriteBankEditor.h \
	SludgeGLApplication.cpp SludgeGLApplication.h \
	../source/DevKit/GraphicalTools/draw2d.cpp ../source/DevKit/GraphicalTools/draw2d.h \
	SludgeApplication.cpp SludgeApplication.h \
	Common.cpp Common.h ../source/DevKit/Common/interface.h \
	../source/DevKit/GraphicalTools/sprites.cpp ../source/DevKit/GraphicalTools/sprites.h \
	../source/DevKit/Common/moreio.cpp ../source/DevKit/Common/moreio.h \
	../source/DevKit/Common/helpers.cpp ../source/DevKit/Common/helpers.h \
	../source/CommonCode/tga.cpp ../source/CommonCode/tga.h \
	../source/CommonCode/utf8.cpp ../source/CommonCode/utf8.h \
	../source/CommonCode/version.h

# The OpenGL view of the editors, see EditorView.h.
if COND_MACOS
EDITOR_VIEW = EditorView.h OffscreenView.cpp
else
EDITOR_VIEW = EditorView.h GLAreaView.cpp
endif
sludge_floormaker_SOURCES += $(EDITOR_VIEW)
sludge_zbuffermaker_SOURCES += $(EDITOR_VIEW)
sludge_spritebankeditor_SOURCES += $(EDITOR_VIEW)

sludge_translationeditor_SOURCES = \
	TranslationEditorMain.cpp TranslationEditorMain.h \
	SludgeTranslationEditor.cpp SludgeTranslationEditor.h \
	SludgeApplication.cpp SludgeApplication.h \
	Common.cpp Common.h ../source/DevKit/Common/interface.h \
	../source/DevKit/TranslationEditor/translator.cpp ../source/DevKit/TranslationEditor/translator.h \
	../source/DevKit/Common/moreio.cpp ../source/DevKit/Common/moreio.h \
	../source/DevKit/Common/helpers.cpp ../source/DevKit/Common/helpers.h \
	../source/DevKit/Common/stringarray.cpp ../source/DevKit/Common/stringarray.h \
	../source/CommonCode/utf8.cpp ../source/CommonCode/utf8.h \
	../source/CommonCode/version.h

sludge_projectmanager_SOURCES = \
	ProjectManagerMain.cpp ProjectManagerMain.h \
	SludgeProjectManager.cpp SludgeProjectManager.h \
	SludgeApplication.cpp SludgeApplication.h \
	Common.cpp Common.h ../source/DevKit/Common/interface.h \
	../source/DevKit/ProjectManager/project.cpp ../source/DevKit/ProjectManager/project.hpp \
	../source/DevKit/ProjectManager/settings.cpp ../source/DevKit/ProjectManager/settings.h \
	../source/DevKit/ProjectManager/messbox.cpp ../source/DevKit/ProjectManager/messbox.h \
	../source/DevKit/ProjectManager/errorlinktofile.h \
	../source/DevKit/Compiler/compiler.cpp ../source/DevKit/Compiler/compiler.hpp \
	../source/DevKit/Compiler/compilerinfo.cpp ../source/DevKit/Compiler/compilerinfo.h \
	../source/DevKit/Compiler/allknown.cpp ../source/DevKit/Compiler/allknown.h \
	../source/DevKit/Compiler/checkused.cpp ../source/DevKit/Compiler/checkused.h \
	../source/DevKit/Compiler/dumpfiles.cpp ../source/DevKit/Compiler/dumpfiles.h \
	../source/DevKit/Compiler/floor.cpp ../source/DevKit/Compiler/floor.h \
	../source/DevKit/Compiler/hsi.cpp ../source/DevKit/Compiler/hsi.h \
	../source/DevKit/Compiler/linker.cpp ../source/DevKit/Compiler/linker.h \
	../source/DevKit/Compiler/objtype.cpp ../source/DevKit/Compiler/objtype.h \
	../source/DevKit/Compiler/preproc.cpp ../source/DevKit/Compiler/preproc.h \
	../source/DevKit/Compiler/realproc.cpp ../source/DevKit/Compiler/realproc.h \
	../source/DevKit/Compiler/sludge_functions.cpp ../source/DevKit/Compiler/sludge_functions.h \
	../source/DevKit/Compiler/splitter.cpp ../source/DevKit/Compiler/splitter.hpp \
	../source/DevKit/Compiler/tokens.cpp ../source/DevKit/Compiler/tokens.h \
	../source/DevKit/Compiler/translation.cpp ../source/DevKit/Compiler/translation.h \
	../source/DevKit/Compiler/csludge.h ../source/DevKit/Compiler/typedef.h \
	../source/DevKit/Common/moreio.cpp ../source/DevKit/Common/moreio.h \
	../source/DevKit/Common/helpers.cpp ../source/DevKit/Common/helpers.h \
	../source/DevKit/Common/stringarray.cpp ../source/DevKit/Common/stringarray.h \
	../source/CommonCode/tga.cpp ../source/CommonCode/tga.h \
	../source/CommonCode/utf8.cpp ../source/CommonCode/utf8.h \
	../source/CommonCode/functionlist.h \
	../source/CommonCode/version.h

# On macOS, we put the menus of the tools into the macOS menu bar, see
# MacMenuBar.h.
if COND_MACOS
sludge_floormaker_SOURCES += MacMenuBar.cpp MacMenuBar.h
sludge_zbuffermaker_SOURCES += MacMenuBar.cpp MacMenuBar.h
sludge_spritebankeditor_SOURCES += MacMenuBar.cpp MacMenuBar.h
sludge_translationeditor_SOURCES += MacMenuBar.cpp MacMenuBar.h
sludge_projectmanager_SOURCES += MacMenuBar.cpp MacMenuBar.h
endif

sludge_compiler_SOURCES = \
	CompilerMain.cpp ../source/DevKit/Common/interface.h \
	../source/DevKit/ProjectManager/project.cpp ../source/DevKit/ProjectManager/project.hpp \
	../source/DevKit/ProjectManager/settings.cpp ../source/DevKit/ProjectManager/settings.h \
	../source/DevKit/ProjectManager/messbox.cpp ../source/DevKit/ProjectManager/messbox.h \
	../source/DevKit/ProjectManager/errorlinktofile.h \
	../source/DevKit/Compiler/compiler.cpp ../source/DevKit/Compiler/compiler.hpp \
	../source/DevKit/Compiler/compilerinfo.cpp ../source/DevKit/Compiler/compilerinfo.h \
	../source/DevKit/Compiler/allknown.cpp ../source/DevKit/Compiler/allknown.h \
	../source/DevKit/Compiler/checkused.cpp ../source/DevKit/Compiler/checkused.h \
	../source/DevKit/Compiler/dumpfiles.cpp ../source/DevKit/Compiler/dumpfiles.h \
	../source/DevKit/Compiler/floor.cpp ../source/DevKit/Compiler/floor.h \
	../source/DevKit/Compiler/hsi.cpp ../source/DevKit/Compiler/hsi.h \
	../source/DevKit/Compiler/linker.cpp ../source/DevKit/Compiler/linker.h \
	../source/DevKit/Compiler/objtype.cpp ../source/DevKit/Compiler/objtype.h \
	../source/DevKit/Compiler/preproc.cpp ../source/DevKit/Compiler/preproc.h \
	../source/DevKit/Compiler/realproc.cpp ../source/DevKit/Compiler/realproc.h \
	../source/DevKit/Compiler/sludge_functions.cpp ../source/DevKit/Compiler/sludge_functions.h \
	../source/DevKit/Compiler/splitter.cpp ../source/DevKit/Compiler/splitter.hpp \
	../source/DevKit/Compiler/tokens.cpp ../source/DevKit/Compiler/tokens.h \
	../source/DevKit/Compiler/translation.cpp ../source/DevKit/Compiler/translation.h \
	../source/DevKit/Compiler/csludge.h ../source/DevKit/Compiler/typedef.h \
	../source/DevKit/Common/moreio.cpp ../source/DevKit/Common/moreio.h \
	../source/DevKit/Common/helpers.cpp ../source/DevKit/Common/helpers.h \
	../source/DevKit/Common/stringarray.cpp ../source/DevKit/Common/stringarray.h \
	../source/CommonCode/tga.cpp ../source/CommonCode/tga.h \
	../source/CommonCode/utf8.cpp ../source/CommonCode/utf8.h \
	../source/CommonCode/functionlist.h \
	../source/CommonCode/version.h

dist_pkgdata_DATA = \
	FloorMaker.glade ZBufferMaker.glade \
	SpriteBankEditor.glade ProjectManager.glade \
	TranslationEditor.glade \
	../images/floormodeicon1.png \
	../images/floormodeicon2.png \
	../images/floormodeicon3.png \
	../images/floormodeicon4.png \
	../images/floormodeicon5.png \
	../images/flags/flags_16x16x32.png ../images/flags/flags_32x32x32.png \
	../images/flags/flags_128x128x32.png ../images/flags/flags_256x256x32.png \
	../images/floorIcon/floorIcon_16x16x32.png ../images/floorIcon/floorIcon_32x32x32.png \
	../images/floorIcon/floorIcon_128x128x32.png ../images/floorIcon/floorIcon_256x256x32.png \
	../images/zIcon/zIcon_16x16x32.png ../images/zIcon/zIcon_32x32x32.png \
	../images/zIcon/zIcon_128x128x32.png ../images/zIcon/zIcon_256x256x32.png \
	../images/spriteIcon/spriteIcon_16x16x32.png ../images/spriteIcon/spriteIcon_32x32x32.png \
	../images/spriteIcon/spriteIcon_128x128x32.png ../images/spriteIcon/spriteIcon_256x256x32.png \
	../images/ProjIcon/ProjIcon_16x16x32.png ../images/ProjIcon/ProjIcon_32x32x32.png \
	../images/ProjIcon/ProjIcon_128x128x32.png ../images/ProjIcon/ProjIcon_256x256x32.png \
	../images/compiler/compiler_16x16x32.png ../images/gameIcon/gameIcon_16x16x32.png

EXTRA_DIST = \
	ToolNames.h \
	FloorMaker.rc ZBufferMaker.rc SpriteBankEditor.rc TranslationEditor.rc ProjectManager.rc \
	../images/flags/flags.ico \
	../images/floorIcon/floorIcon.ico ../images/zIcon/zIcon.ico \
	../images/spriteIcon/spriteIcon.ico ../images/ProjIcon/ProjIcon.ico

AM_CPPFLAGS = -I$(top_srcdir)/source/CommonCode -I$(top_srcdir)/source/DevKit/Common -I$(top_srcdir)/source/DevKit/Compiler -I$(top_srcdir)/source/DevKit/ProjectManager -I$(top_srcdir)/source/DevKit/GraphicalTools -I$(top_srcdir)/source/DevKit/TranslationEditor @GTK_CFLAGS@ @LIBPNG_CFLAGS@ @EPOXY_CFLAGS@

if COND_WIN32
AM_CPPFLAGS += \
	-mms-bitfields  \
	-DDATADIR='"resources/"'

AM_LDFLAGS = \
	-Wl,-subsystem,windows

# We run the compiler from the command line, so we link it as a console
# program, without the flags above.
sludge_compiler_LDFLAGS =

sludge_floormaker_LDADD = FloorMaker.o @GTK_LIBS@ @LIBPNG_LIBS@ @EPOXY_LIBS@
sludge_zbuffermaker_LDADD = ZBufferMaker.o @GTK_LIBS@ @LIBPNG_LIBS@ @EPOXY_LIBS@
sludge_spritebankeditor_LDADD = SpriteBankEditor.o @GTK_LIBS@ @LIBPNG_LIBS@ @EPOXY_LIBS@
sludge_translationeditor_LDADD = TranslationEditor.o @GTK_LIBS@ @LIBPNG_LIBS@
sludge_projectmanager_LDADD = ProjectManager.o @GTK_LIBS@ @LIBPNG_LIBS@
else
AM_CPPFLAGS += \
	-std=c++0x \
	-DDATADIR='"$(pkgdatadir)/"'

if COND_MACOS
# On macOS, we find the Resources folder of the app with CoreFoundation, and
# create the OpenGL contexts of the editor views with CGL, which Apple marks
# as deprecated.
AM_CPPFLAGS += -DGL_SILENCE_DEPRECATION @MAC_INTEGRATION_CFLAGS@
AM_LDFLAGS = -framework CoreFoundation -framework OpenGL
endif

# MAC_INTEGRATION_LIBS is empty on the other systems.
sludge_floormaker_LDADD = @GTK_LIBS@ @LIBPNG_LIBS@ @EPOXY_LIBS@ @MAC_INTEGRATION_LIBS@
sludge_zbuffermaker_LDADD = @GTK_LIBS@ @LIBPNG_LIBS@ @EPOXY_LIBS@ @MAC_INTEGRATION_LIBS@
sludge_spritebankeditor_LDADD = @GTK_LIBS@ @LIBPNG_LIBS@ @EPOXY_LIBS@ @MAC_INTEGRATION_LIBS@
sludge_translationeditor_LDADD = @GTK_LIBS@ @LIBPNG_LIBS@ @MAC_INTEGRATION_LIBS@
sludge_projectmanager_LDADD = @GTK_LIBS@ @LIBPNG_LIBS@ @MAC_INTEGRATION_LIBS@
endif

sludge_compiler_LDADD = @GLIB_LIBS@ @LIBPNG_LIBS@

# On Windows, we compile the icon and the version information of each tool
# from its .rc file, for example FloorMaker.rc into FloorMaker.o.
SUFFIXES = .rc
.rc.o:
	$(WINDRES) $< -O coff -o $@

# The .rc files include version.h, so we compile them again when it changes.
FloorMaker.o ZBufferMaker.o SpriteBankEditor.o TranslationEditor.o ProjectManager.o: ../source/CommonCode/version.h
