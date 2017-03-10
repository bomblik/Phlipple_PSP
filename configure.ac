AC_INIT(phlipple, 0.8.5)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE
AC_ISC_POSIX
AC_PROG_CC_C99
AC_HEADER_STDC

PKG_CHECK_MODULES(PHLIPPLE, [sdl vorbisfile glew])

AC_CHECK_LIB(SDL_image, IMG_Load, [], AC_MSG_ERROR(no SDL_image found))
AC_CHECK_LIB(SDL_mixer, Mix_Linked_Version, [], AC_MSG_ERROR(no SDL_mixer found))

AC_CHECK_LIB(GL, main, [], AC_MSG_ERROR([OpenGL library not found.]))
AC_CHECK_LIB(GLU, gluPerspective, [], AC_MSG_ERROR([GLU library not found.]))

AC_CHECK_HEADER(GL/gl.h, [], AC_MSG_ERROR(no GL/gl.h found))
AC_CHECK_HEADER(GL/glu.h, [], AC_MSG_ERROR(no GL/glu.h found))
AC_CHECK_HEADER(SDL/SDL_image.h, [], AC_MSG_ERROR(no SDL/SDL_image.h found))
AC_CHECK_HEADER(SDL/SDL_mixer.h, [], AC_MSG_ERROR(no SDL/SDL_mixer.h found))

AC_ARG_WITH(icondir,
	AS_HELP_STRING([--with-icondir=DIR],[icon files [DATAROOTDIR/pixmaps]]),
	[ with_icondir=${withval} ], [ with_icondir=${datadir}/pixmaps ])
AC_SUBST([icondir], [${with_icondir}])

AC_ARG_WITH(applicationdir,
	AS_HELP_STRING([--with-applicationdir=DIR],[application files [DATAROOTDIR/applications]]),
	[ with_applicationdir=${withval} ], [ with_applicationdir=${datadir}/applications ])
AC_SUBST([applicationdir], [${with_applicationdir}])

AC_OUTPUT([Makefile
data/Makefile
src/Makefile
extra/Makefile
])
