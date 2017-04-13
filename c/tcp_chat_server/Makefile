##
## Makefile
##

# variables
INCLUDES = constants.h
SOURCES = tcp_chat_server.c constants.c
OBJFILES = tcp_chat_server.o constants.o
EXEC = tcp_chat_server

${EXEC}: ${OBJFILES}
	gcc -Wall -lrt -lm -g -o ${EXEC} ${OBJFILES}

.c.o:
	gcc -Wall -lrt -lm -g -c $<

${OBJFILES}: ${SOURCES} ${INCLUDES}

start: ${EXEC}
	./${EXEC}

clean:
	rm -f ${OBJFILES} ${EXEC}

clean_obj:
	rm -f ${OBJFILES}

debug: ${EXEC}
	gdb -tui ${EXEC}
