##
## Makefile
##

# variables
SOURCES = TcpClientSum.java
CLASSES = TcpClientSum.class
EXEC = TcpClientSum

${CLASSES}: ${SOURCES}
	javac ${SOURCES}

start: ${CLASSES}
	@read -p "Server Address: " sv_addr; \
	java ${EXEC} $$sv_addr

clean:
	rm -f ${CLASSES} ${EXEC}
