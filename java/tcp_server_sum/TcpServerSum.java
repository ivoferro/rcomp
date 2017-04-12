import java.io.*;
import java.net.*;

/**
 * Represents a TCP server that handles sums.
 */
class TcpServerSum {

    public static final int SERVER_PORT = 9999;

    public static final int OPEN_SOCKET_FAILED = -1;

    /**
     * Starts the TCP server.
     *
     * @param args arguments from command line
     * @throws IOException input/output exception
     */
    public static void main(String args[]) throws IOException {
        ServerSocket sock = null;
        Socket cliSock;

        try {
            sock = new ServerSocket(SERVER_PORT);
        } catch (IOException ex) {
            System.out.println("Failed to open server socket");
            System.exit(OPEN_SOCKET_FAILED);
        }

        System.out.println("Server is running, waiting for connections (press \"CTRL + c\" to quit).");
        while (true) {
            cliSock = sock.accept();
            Thread clientThread = new Thread(new TcpServerSumThread(cliSock));
            clientThread.start();
        }
    }
}

/**
 * Thread to manage a TCP connection.
 */
class TcpServerSumThread implements Runnable {
    private Socket s;
    private DataOutputStream sOut;
    private DataInputStream sIn;

    public TcpServerSumThread(Socket clientSocket) {
        s = clientSocket;
    }

    @Override
    public void run() {
        long f, i, num, sum;
        InetAddress clientIP;
        clientIP = s.getInetAddress();
        System.out.println("New client connection from " + clientIP.getHostAddress()
                + ", port number " + s.getPort());
        try {
            sOut = new DataOutputStream(s.getOutputStream());
            sIn = new DataInputStream(s.getInputStream());
            do {
                sum = 0;
                do {
                    num = 0;
                    f = 1;
                    for (i = 0; i < 4; i++) {
                        num = num + f * sIn.read();
                        f = f * 256;
                    }
                    sum = sum + num;
                }
                while (num > 0);
                num = sum;
                for (i = 0; i < 4; i++) {
                    sOut.write((byte) (num % 256));
                    num = num / 256;
                }
            }
            while (sum > 0);
            System.out.println("Client " + clientIP.getHostAddress()
                    + ", port number: " + s.getPort() + " disconnected");
            s.close();
        } catch (IOException ex) {
            System.out.println("IOException");
        }
    }
}
