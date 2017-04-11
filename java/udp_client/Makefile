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
	@read -p "Server Address: " sv_addr; \
	java ${EXEC} $$sv_addr

clean:
	rm -f ${CLASSES} ${EXEC}