##
## Makefile
##

# variables
SOURCES = UdpServer.java
CLASSES = UdpServer.class
EXEC = UdpServer

${CLASSES}: ${SOURCES}
	javac ${SOURCES}

start: ${CLASSES}
	java ${EXEC}

clean:
	rm -f ${CLASSES} ${EXEC}