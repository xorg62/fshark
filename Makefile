# fshark Makefile
# Martin Duquesnoy (150193@supinfo.com)
#

SDL_CFLAGS=	$(shell sdl-config --cflags)
SDL_LIBS=	$(shell sdl-config --libs)

CC=		gcc
SRC=		fshark.c	\
		event.c		\
		map.c		\
		plane.c		\
		render_obj.c	\
		enemy.c         \
	        ui.c            \
	        util.c          \
	        game.c

BIN=		fshark

CFLAGS=		-Wall ${SDL_CFLAGS}
LDFLAGS=	${SDL_LIBS} -lSDL_image -lSDL_ttf -lSDL_gfx -lSDL_mixer

fshark: ${SRC}
	@echo "Compile..."
	@echo "${CC} ${CFLAGS} ${LDFLAGS} ${SRC} -o ${BIN}"
	@${CC} ${CFLAGS} ${LDFLAGS} ${SRC} -o ${BIN}

clean:
	@echo "Cleaning..."
	@echo "rm -rf ${BIN} *~"
	@rm -rf ${BIN} *~