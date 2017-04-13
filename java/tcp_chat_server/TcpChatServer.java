import java.io.*;
import java.net.*;
import java.util.concurrent.*;

/**
 * Represents a TCP chat server.
 */
class TcpChatServer {

    public static final int MAX_CLIENTS = 100;

    public static final int INVALID_PORT = -1;

    public static Socket[] cliSock = new Socket[MAX_CLIENTS];

    public static DataOutputStream[] sOut = new DataOutputStream[MAX_CLIENTS];

    public static Boolean[] inUse = new Boolean[MAX_CLIENTS];

    public static Semaphore changeLock = new Semaphore(1);

    public static ServerSocket sock;

    /**
     * Runs the TCP chat server.
     *
     * @param args arguments from command line.
     * @throws Exception exception
     */
    public static void main(String args[]) throws Exception {
        int i;

        try {
            sock = new ServerSocket(9999);
        } catch (IOException ex) {
            System.out.println("Local port number not available.");
            System.exit(INVALID_PORT);
        }

        for (i = 0; i < MAX_CLIENTS; i++) {
            inUse[i] = false;
        }

        while (true) {
            changeLock.acquire();
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (!inUse[i]) {
                    break; // find a free socket
                }
            }
            changeLock.release();

            cliSock[i] = sock.accept(); // wait for a new client connection request

            changeLock.acquire();
            inUse[i] = true;
            sOut[i] = new DataOutputStream(cliSock[i].getOutputStream());
            changeLock.release();

            Thread clientConnection = new Thread(new TcpChatServerClient(i));
            clientConnection.start();
        }
    }
}

/**
 * Represents a TCP chat server connection to a client.
 */
class TcpChatServerClient implements Runnable {

    int clientNumber;

    public TcpChatServerClient(int clientNumber) {
        this.clientNumber = clientNumber;
    }

    @Override
    public void run() {
        int i, nChars;
        byte[] data = new byte[300];
        DataInputStream sIn;

        try {
            sIn = new DataInputStream(TcpChatServer.cliSock[clientNumber].getInputStream());
            while (true) {
                nChars = sIn.read();
                if (nChars == 0) break; // empty line
                sIn.read(data, 0, nChars);
                TcpChatServer.changeLock.acquire();
                for (i = 0; i < TcpChatServer.MAX_CLIENTS; i++) // retransmit the line
                    if (TcpChatServer.inUse[i]) {
                        TcpChatServer.sOut[i].write(nChars);
                        TcpChatServer.sOut[i].write(data, 0, nChars);
                    }
                TcpChatServer.changeLock.release();
            }
            
            // the client wants to exit
            TcpChatServer.changeLock.acquire();
            TcpChatServer.sOut[clientNumber].write(nChars);
            TcpChatServer.inUse[clientNumber] = false;
            TcpChatServer.cliSock[clientNumber].close();
            TcpChatServer.changeLock.release();
        } catch (IOException ex) {
            System.out.println("IOException");
        } catch (InterruptedException ex) {
            System.out.println("Interrupted");
        }
    }
}