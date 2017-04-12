##
## Makefile
##

# variables
SOURCES = UdpClient.java
CLASSES = UdpClient.class
EXEC = UdpClient

${CLASSES}: ${SOURCES}
	javac ${SOURCES}

start: ${CLASSES}
	java ${EXEC}

clean:
	rm -f ${CLASSES} ${EXEC}
