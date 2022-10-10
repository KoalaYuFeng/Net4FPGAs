// C++ version of tranfer data to another FPGA card.
// yufeng@nus.edu.sg
// initial -> load xclbin -> check link status up -> 
// -> set our ip -> arp discovery finish -> send data.
// using basic xclbin.

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include "fpga_device.h"
#include "alveo_vnx_link.h"
#include "alveo_vnx_loop.h"
#include "network.h"

int main(int argc, char *argv[]) {

    std::string xclbin;
    int our_id, their_id;

    if (argc != 4) {
        std::cout << "Usage: ./tx <xclbin_file> <our fpga_id> <their fpga_id>" << std::endl;
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
        std::cout<<"LinkStatus is " << linkStatus <<std::endl;
        sleep(1);
        linkStatus = l1.getLinkStatus();// usually is 3
    }
    std::cout<<"Link status up"<<std::endl;

    // get FPGA configuration.
    std::string our_ip = FPGA_config[our_id]["ip_addr"];
    std::string our_port = FPGA_config[our_id]["port"];
    std::string our_idx = FPGA_config[our_id]["idx"];
    std::string our_mac = FPGA_config[our_id]["MAC_addr"];
    std::cout << "our fpga " << " ip:" << our_ip <<" port: "<< our_port << std::endl;

    std::string their_ip = FPGA_config[their_id]["ip_addr"];
    std::string their_port = FPGA_config[their_id]["port"];
    std::string their_idx = FPGA_config[their_id]["idx"];
    std::string their_mac = FPGA_config[their_id]["MAC_addr"];
    std::cout << "their fpga " << " ip:" << their_ip <<" port: "<< their_port << std::endl;

    l1.setMyAddresses(our_ip, our_mac, stoi(our_port));
    std::cout << "link loop created" << std::endl;

    char *tx_buf = new char[SIZE_RX_BUFFER];
    for (int i = 0; i < SIZE_RX_BUFFER; i++) {
        tx_buf[i] = '0' + rand() % 10;
    }

    std::cout<< "Transfering data array " << SIZE_RX_BUFFER << " bytes" << std::endl;

    l1.sendTo(their_ip, stoi(their_port), tx_buf, SIZE_RX_BUFFER);

    std::cout << "Packet sent" << std::endl;

    return 0;
}
