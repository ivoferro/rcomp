import java.io.*;
import java.net.*;

/**
 * Represents a TCP chat client.
 */
class TcpChatClient {

    public static final int SERVER_ADDRESS_REQUIRED = -1;

    public static final int INVALID_SERVER_ADDRESS = -2;

    public static final int CONNECTION_FAILED = -3;

    /**
     * Runs the TCP chat receiving the server address by parameter.
     *
     * @param args server address
     * @throws Exception exception
     */
    public static void main(String args[]) throws Exception {
        String nick, sentence;
        byte[] data = new byte[300];
        Socket sock = null;
        InetAddress destinationAddress = null;

        if (args.length != 1) {
            System.out.println("Server IPv4/IPv6 address or DNS name is required as argument");
            System.exit(SERVER_ADDRESS_REQUIRED);
        }

        try {
            destinationAddress = InetAddress.getByName(args[0]);
        } catch (UnknownHostException ex) {
            System.out.println("Invalid server: " + args[0]);
            System.exit(INVALID_SERVER_ADDRESS);
        }

        try {
            sock = new Socket(destinationAddress, 9999);
        } catch (IOException ex) {
            System.out.println("Failed to connect.");
            System.exit(CONNECTION_FAILED);
        }

        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        DataOutputStream sOut = new DataOutputStream(sock.getOutputStream());

        System.out.print("Nickname: ");
        nick = in.readLine();

        Thread serverConnection = new Thread(new TcpChatClientConnection(sock));
        serverConnection.start();

        while (true) {
            sentence = in.readLine();
            if (sentence.compareTo("exit") == 0) {
                sOut.write(0);
                break;
            }
            sentence = "(" + nick + ") " + sentence;

            data = sentence.getBytes();
            sOut.write((byte) sentence.length());
            sOut.write(data, 0, (byte) sentence.length());
        }

        serverConnection.join();
        sock.close();
    }
}

/**
 * Represents a TCP chat client connection to receive messages.
 */
class TcpChatClientConnection implements Runnable {

    private Socket s;
    private DataInputStream sIn;

    public TcpChatClientConnection(Socket tcpSocket) {
        s = tcpSocket;
    }

    @Override
    public void run() {
        int nChars;
        byte[] data = new byte[300];
        String sentence;

        try {
            sIn = new DataInputStream(s.getInputStream());
            while (true) {
                nChars = sIn.read();
                if (nChars == 0) {
                    break;
                }
                sIn.read(data, 0, nChars);
                sentence = new String(data, 0, nChars);
                System.out.println(sentence);
            }
        } catch (IOException ex) {
            System.out.println("Client disconnected.");
        }
    }
}