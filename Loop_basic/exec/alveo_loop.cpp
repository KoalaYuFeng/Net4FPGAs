// enable loop function. 9-23 loop_back function.

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include "fpga_device.h"
#include "alveo_vnx_link.h"
#include "alveo_vnx_loop.h"


int main(int argc, char *argv[]) {

    std::string xclbin;
    int our_id, their_id;

    if (argc != 4) {
        std::cout << "Usage: ./loop <xclbin_file> <our fpga_id> <their fpga_id>" << std::endl;
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
        our_id = atoi(argv[2]); // get our fpga id, for configure
        their_id = atoi(argv[3]); // need to get another fpga id
    }

    auto u250 = FpgaDevice(0); // in VM, each host has one fpga U250.
    std::cout << "Device created: " << u250.getName()<<std::endl;
    auto uuid = u250.loadBitfile(xclbin);
    std::cout << "Bitfile loaded " << uuid << std::endl;

    auto l1 = AlveoVnxLink(u250, 1); // 1 for interface 1.
    // read cmac link status;
    bool linkStatus = l1.getLinkStatus();
    while(!linkStatus) {
        sleep(1);
        linkStatus = l1.getLinkStatus();// usually is 3
    }
    std::cout<<"link status up"<<std::endl;

    // get FPGA configuration.
    std::string our_ip = FPGA_config[our_id]["ip_addr"];
    std::string our_port = FPGA_config[our_id]["port"];
    std::string our_idx = FPGA_config[our_id]["idx"];
    std::cout << "our fpga config" << our_ip << our_port << our_idx << std::endl;

    std::string their_ip = FPGA_config[their_id]["ip_addr"];
    std::string their_port = FPGA_config[their_id]["port"];
    std::string their_idx = FPGA_config[their_id]["idx"];
    std::cout << "their fpga config" << their_ip << their_port << their_idx << std::endl;

    l1.setMyAddresses(our_ip, "ab:cd:ef:02:02", stoi(our_port));
    std::cout << "link loop created" << std::endl;

    char *tx_buf = new char[SIZE_TX_DATA];
    for (int i = 0; i < SIZE_TX_DATA; i++) {
        tx_buf[i] = '0' + rand() % 10;
    }

    std::cout<< "Transfering data array " << SIZE_TX_DATA << " bytes" << std::endl;

    l1.sendTo(their_ip, stoi(their_port), tx_buf, SIZE_TX_DATA);

    std::cout << "Packet sent" << std::endl;

    return 0;
}
