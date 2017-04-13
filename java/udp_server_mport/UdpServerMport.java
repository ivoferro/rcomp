import java.io.*;
import java.net.*;

class UdpServerMport {

    private static final int PORT_BASE = 9009;

    private static final int STEP = 100;

    private static final int NUM_PORTS = 6;

    private static final int BIND_PORT_FAILED = -1;

    public static void main(String args[]) throws Exception {
        int i;
        DatagramSocket sock[] = new DatagramSocket[NUM_PORTS];

        for (i = 0; i < NUM_PORTS; i++) {
            try {
                sock[i] = new DatagramSocket(PORT_BASE + STEP * i);
            } catch (BindException ex) {
                System.out.println("Failed to bind to port number " + PORT_BASE + STEP * i);
                do {
                    sock[i].close();
                    i--;
                } while (i > -1);
                System.exit(BIND_PORT_FAILED);
            }
        }
        System.out.println("Listening for requests (both over IPv6 or IPv4). Use CTRL+C to quit");
        for (i = 0; i < NUM_PORTS; i++) { // start one thread for each socket
            Thread sockThread = new Thread(new UdpServerMportThread(sock[i]));
            sockThread.start();
        }
    }
}

class UdpServerMportThread implements Runnable {

    private DatagramSocket sock;

    public UdpServerMportThread(DatagramSocket s) {
        sock = s;
    }

    public void run() {
        byte[] data = new byte[300];
        byte[] data_aux = new byte[300];
        int len, i;

        DatagramPacket udpPacket = new DatagramPacket(data, data.length);
        try {
            while (true) {
                udpPacket.setData(data);
                udpPacket.setLength(data.length);
                sock.receive(udpPacket);
                len = udpPacket.getLength();
                System.out.println("Request from: " + udpPacket.getAddress().getHostAddress() +
                        " port: " + udpPacket.getPort());
                for (i = 0; i < len; i++) data_aux[len - 1 - i] = data[i];
                udpPacket.setData(data_aux);
                udpPacket.setLength(len);
                sock.send(udpPacket);
            }
        } catch (IOException ex) {
            System.out.println("IOException");
        }
    }
}
