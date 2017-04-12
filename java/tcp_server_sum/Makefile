##
## Makefile
##

# variables
SOURCES = TcpServerSum.java
CLASSES = TcpServerSum.class
EXEC = TcpServerSum

${CLASSES}: ${SOURCES}
	javac ${SOURCES}

start: ${CLASSES}
	java ${EXEC}

clean:
	rm -f ${CLASSES} ${EXEC}
