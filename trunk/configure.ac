
AC_PREREQ(2.59)
AC_INIT(openredalert, 0.1, damien.carol@gmail.com)
AM_INIT_AUTOMAKE

AC_PROG_CXX
AC_PROG_MAKE_SET



# Check for SDL
SDL_VERSION=1.2.5
AM_PATH_SDL($SDL_VERSION,
            :,
            AC_MSG_ERROR([*** SDL version $SDL_VERSION not found!])
)
AM_CFLAGS="$AM_CFLAGS $SDL_CFLAGS"
AM_LDFLAGS="$AM_LDFLAGS $SDL_LIBS"



# Check for SDL_mixer library
AC_CHECK_LIB(SDL_mixer, Mix_OpenAudio, , AC_MSG_ERROR([
*** Unable to find SDL_mixer libary
(http://www.libsdl.org/projects/SDL_mixer/)
]))
# TODO : add flags to the build



AC_CONFIG_FILES([
    Makefile
])

AC_OUTPUT
