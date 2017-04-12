##
## Makefile
##

# variables
INCLUDES = constants.h
SOURCES = tcp_client_sum.c constants.c
OBJFILES = tcp_client_sum.o constants.o
EXEC = tcp_client_sum

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

