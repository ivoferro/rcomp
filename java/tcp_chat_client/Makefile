##
## Makefile
##

# variables
SOURCES = TcpChatClient.java
CLASSES = TcpChatClient.class
EXEC = TcpChatClient

${CLASSES}: ${SOURCES}
	javac ${SOURCES}

start: ${CLASSES}
	@read -p "Server Address: " sv_addr; \
	java ${EXEC} $$sv_addr

clean:
	rm -f ${CLASSES} ${EXEC}
