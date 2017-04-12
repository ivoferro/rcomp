import java.io.*;
import java.net.*;

/**
 * Represents an UDP client.
 */
class UdpClient {

    public static final int SERVER_PORT = 9999;

    public static final int SERVER_ADDRESS_REQUIRED = -1;

    public static final int INVALID_SERVER_ADDRESS = -2;

    private static int TIMEOUT = 3;

    /**
     * Connects to a given server and sends UDP messages.
     *
     * @param args server IP address or DNS
     * @throws Exception exception
     */
    public static void main(String args[]) throws IOException {
        byte[] data = new byte[300];
        String sentence;
        InetAddress serverAddress = InetAddress.getByName("255.255.255.255");

        DatagramSocket sock = new DatagramSocket();
        sock.setBroadcast(true);
        sock.setSoTimeout(1000 * TIMEOUT); // set the socket timeout

        DatagramPacket udpPacket = new DatagramPacket(data, data.length, serverAddress, SERVER_PORT);
        BufferedReader in = new BufferedReader(new InputStreamReader(System.in));

        while (true) {
            System.out.print("Request sentence to send (\"exit\" to quit): ");
            sentence = in.readLine();
            if (sentence.compareTo("exit") == 0) {
                break;
            }

            udpPacket.setData(sentence.getBytes());
            udpPacket.setLength(sentence.length());
            sock.send(udpPacket);

            udpPacket.setData(data);
            udpPacket.setLength(data.length);

            try {
                sock.receive(udpPacket);
                sentence = new String(udpPacket.getData(), 0, udpPacket.getLength());
                System.out.println("Received reply: " + sentence);
            } catch (SocketTimeoutException ex) {
                System.out.println("No reply from server");
            }
        }
        sock.close();
    }
}
