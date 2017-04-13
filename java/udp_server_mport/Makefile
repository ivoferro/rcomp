##
## Makefile
##

# variables
SOURCES = UdpServerMport.java
CLASSES = UdpServerMport.class
EXEC = UdpServerMport

${CLASSES}: ${SOURCES}
	javac ${SOURCES}

start: ${CLASSES}
	java ${EXEC}

clean:
	rm -f ${CLASSES} ${EXEC}