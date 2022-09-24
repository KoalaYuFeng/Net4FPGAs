// Enable tx and rx.

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include "fpga_device.h"
#include "alveo_vnx_link.h"


int main(int argc, char *argv[]) {

    std::string xclbin;
    int alveo_id = 0; // set alveo_id = 0;

    if (argc != 2) {
        std::cout << "Usage: ./basic <xclbin_file>" << std::endl;
        return EXIT_FAILURE;
    }
    else {
        std::ifstream f(argv[1]);
        if (f.good() == false) {
            std::cout << "Make sure the path to the bitfile <xclbin_file> is correct" << std::endl;
            return EXIT_FAILURE;
        }
        f.close();
        xclbin = std::string(argv[1]);            

    }

    auto u250 = FpgaDevice(alveo_id);

    std::cout << "Device created: " << u250.getName()<<std::endl;

    auto uuid = u250.loadBitfile(xclbin);
    std::cout << "Bitfile loaded " << uuid << std::endl;

    // read cmac link status;
    
    
    auto l1 = AlveoVnxLink(u250, 1);
    l1.setMyAddresses("192.168.0.1", "ab:cd:ef:01:01", 10000);

    std::cout << "link basic created" << std::endl;

    // enable tx and rx, need to enable rx firstly then tx.
    char *tx_buf = new char[SIZE_TX_DATA];
    for (int i = 0; i < SIZE_TX_DATA; i++) {
        tx_buf[i] = '0' + rand() % 10;
    }
    std::cout << " random number file generated "<< std::endl;
    
    char *rx_buf = new char[SIZE_RX_BUFFER];

    size_t size = l1.basicRecvSend("192.168.0.2", 10001, tx_buf, rx_buf, SIZE_TX_DATA);

    std::cout << "Packet received " << size << " bytes" << std::endl;

    std::ofstream outfile("out.bin", std::ios::binary | std::ios::ate);
    outfile.write(rx_buf, size);
    outfile.close();

    return 0;
}
