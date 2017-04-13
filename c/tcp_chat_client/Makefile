##
## Makefile
##

# variables
INCLUDES = constants.h
SOURCES = tcp_chat_client.c constants.c
OBJFILES = tcp_chat_client.o constants.o
EXEC = tcp_chat_client

${EXEC}: ${OBJFILES}
	gcc -Wall -lrt -lm -g -o ${EXEC} ${OBJFILES}

.c.o:
	gcc -Wall -lrt -lm -g -c $<

${OBJFILES}: ${SOURCES} ${INCLUDES}

start: ${EXEC}
	@read -p "Server Address: " sv_addr; \
	./${EXEC} $$sv_addr

clean:
	rm -f ${OBJFILES} ${EXEC}

clean_obj:
	rm -f ${OBJFILES}

debug: ${EXEC}
	gdb -tui ${EXEC}

