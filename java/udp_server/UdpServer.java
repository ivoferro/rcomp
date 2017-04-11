import java.io.*;
import java.net.*;

class UdpServer {

    public static final int SERVER_PORT = 9999;

    public static final int BIND_PORT_FAILED = -1;

    public static final int INVALID_SERVER_ADDRESS = -2;

    public static void main(String args[]) throws IOException {
        DatagramSocket sock = null;
        byte[] data = new byte[300];
        byte[] dataAux = new byte[300];
        int i, len;

        try {
            sock = new DatagramSocket(SERVER_PORT);
        } catch (BindException ex) {
            System.out.println("Bind to local port failed");
            System.exit(BIND_PORT_FAILED);
        }

        DatagramPacket udpPacket = new DatagramPacket(data, data.length);
        System.out.println("Listening for requests (both over IPv6 or IPv4). Use CTRL+C to terminate");
        while (true) {
            udpPacket.setData(data);
            udpPacket.setLength(data.length);
            sock.receive(udpPacket);
            len = udpPacket.getLength();
            System.out.println("Request from: " + udpPacket.getAddress().getHostAddress() +
                    " port: " + udpPacket.getPort());

            for (i = 0; i < len; i++) {
                dataAux[len - 1 - i] = data[i];
            }

            udpPacket.setData(dataAux);
            udpPacket.setLength(len);
            sock.send(udpPacket);
        }
    }
}
