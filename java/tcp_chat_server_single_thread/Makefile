##
## Makefile
##

# variables
SOURCES = TcpChatServer.java
CLASSES = TcpChatServer.class
EXEC = TcpChatServer

${CLASSES}: ${SOURCES}
	javac ${SOURCES}

start: ${CLASSES}
	java ${EXEC}

clean:
	rm -f ${CLASSES} ${EXEC}
