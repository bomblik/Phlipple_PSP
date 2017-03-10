AM_CPPFLAGS = \
	-DPACKAGE_LOCALE_DIR=\""$(localedir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\"

AM_CFLAGS =\
	 -Wall\
	 -g

bin_PROGRAMS = phlipple

phlipple_SOURCES = \
	camera.c\
	camera.h\
	engine.c\
	engine.h\
	game.c\
	gfxconstants.c\
	gfxconstants.h\
	gradientrenderer.c\
	gradientrenderer.h\
	interpolator.c\
	interpolator.h\
	levels.c\
	levels.h\
	main.c\
	mainscreen.c\
	osinterface.h\
	osinterface_sdl.c\
	phlipplerootscene.c\
	phlipplerootscene.h\
	phlipplescenes.h\
	quad.c\
	quad.h\
	quadrenderer.c\
	quadrenderer.h\
	renderstate.h\
	scenes.c\
	scenes.h\
	settings.c\
	settings.h\
	textrenderer.c\
	textrenderer.h\
	texture.c\
	texture.h\
	vertex.c\
	vertex.h
	
phlipple_LDFLAGS = 

phlipple_LDADD = $(PHLIPPLE_LIBS) $(GL_LIBS) 
