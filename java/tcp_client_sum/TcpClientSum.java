import java.io.*;
import java.net.*;

/**
 * Represents an TCP client.
 */
class TcpClientSum {

    public static final int SERVER_PORT = 9999;

    public static final int SERVER_ADDRESS_REQUIRED = -1;

    public static final int INVALID_SERVER_ADDRESS = -2;

    public static final int TCP_CONNECTION_FAILED = -3;

    /**
     * Connects to a given server and sends TCP messages.
     *
     * @param args server IP address or DNS
     * @throws IOException Input/Output exception
     */
    public static void main(String args[]) throws IOException {
        long f, i, n, num;
        String sentence;
        Socket sock = null;
        InetAddress serverAddress = null;

        if (args.length != 1) {
            System.out.println("Server IPv4/IPv6 address or DNS name is required as argument");
            System.exit(SERVER_ADDRESS_REQUIRED);
        }

        try {
            serverAddress = InetAddress.getByName(args[0]);
        } catch (UnknownHostException ex) {
            System.out.println("Invalid server address supplied: " + args[0]);
            System.exit(INVALID_SERVER_ADDRESS);
        }

        try {
            sock = new Socket(serverAddress, SERVER_PORT);
        }
        catch(IOException ex) {
            System.out.println("Failed to establish TCP connection");
            System.exit(TCP_CONNECTION_FAILED);
        }

        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
        DataOutputStream sOut = new DataOutputStream(sock.getOutputStream());
        DataInputStream sIn = new DataInputStream(sock.getInputStream());

        do {
            do {
                num = -1;
                while(num < 0) {
                    System.out.print("Enter a positive integer to SUM (zero to terminate): ");
                    sentence = in.readLine();

                    try {
                        num = Integer.parseInt(sentence);
                    }
                    catch(NumberFormatException ex) {
                        num = -1;
                    }

                    if(num < 0) {
                        System.out.println("Invalid number");
                    }
                }

                n = num;
                for(i = 0; i < 4; i++) {
                    sOut.write((byte)(n % 256));
                    n = n / 256;
                }
            }
            while(num != 0);
            num = 0;
            f = 1;
            for(i = 0; i < 4; i++) {
                num = num + f * sIn.read();
                f = f * 256;
            }
            System.out.println("SUM RESULT = " + num);
        }
        while(num != 0);

        sock.close();
    }
}
