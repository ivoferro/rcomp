import java.io.*;
import java.net.*;
import java.util.concurrent.*;

/**
 * Represents a TCP chat server.
 */
class TcpChatServer {

    public static int MAX_CLIENTS = 100;

    public static final int INVALID_PORT_NUMBER = -1;

    /**
     * Runs the TCP chat server.
     *
     * @param args arguments from command line
     * @throws Exception exception
     */
    public static void main(String args[]) throws Exception {
        Socket[] cliSock = new Socket[MAX_CLIENTS];
        Socket newCli;
        DataOutputStream[] sOut = new DataOutputStream[MAX_CLIENTS];
        DataInputStream[] sIn = new DataInputStream[MAX_CLIENTS];
        Boolean[] inUse = new Boolean[MAX_CLIENTS];
        ServerSocket sock = null;
        int i, j, nChars;
        byte[] data = new byte[300];

        try {
            sock = new ServerSocket(9999);
        } catch (IOException ex) {
            System.out.println("Local port number not available.");
            System.exit(INVALID_PORT_NUMBER);
        }

        for (i = 0; i < MAX_CLIENTS; i++) inUse[i] = false;
        sock.setSoTimeout(100); // set the socket timeout
        while (true) {
            try { // is there a new client connection request
                newCli = sock.accept();
                newCli.setSoTimeout(100); // set the connected socket timeout
                for (i = 0; i < MAX_CLIENTS; i++) {
                    if (!inUse[i]) {
                        break; // find a free slot
                    }
                }
                inUse[i] = true;

                cliSock[i] = newCli;
                sOut[i] = new DataOutputStream(cliSock[i].getOutputStream());
                sIn[i] = new DataInputStream(cliSock[i].getInputStream());
            } catch (SocketTimeoutException to) {
                // TIME OUT
            }

            for (i = 0; i < MAX_CLIENTS; i++) {
                if (inUse[i]) {
                    try { // try reading the line size
                        nChars = sIn[i].read();
                        if (nChars == 0) { // empty line - client wants to exit
                            sOut[i].write(nChars); // send back an empty line
                            cliSock[i].close();
                            inUse[i] = false;
                        } else {
                            sIn[i].read(data, 0, nChars); // read the line
                            for (j = 0; j < MAX_CLIENTS; j++) {// forward the line to all clients
                                if (inUse[j]) {
                                    sOut[j].write(nChars);
                                    sOut[j].write(data, 0, nChars);
                                }
                            }
                        }
                    } catch (SocketTimeoutException to) {
                        // TIME OUT
                    }
                }
            }
        }
    }
}