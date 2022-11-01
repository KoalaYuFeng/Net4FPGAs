// C++ version of enabling tx and rx in one FPGA simultaneously.
// yufeng@nus.edu.sg
// initial -> load xclbin -> check link status up -> 
// -> set our ip -> arp discovery finish -> send data -> wait data come -> get data.
// using basic xclbin.

// in this case, receive data port is 5001 (default).

#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>

#include "fpga_device.h"
#include "alveo_vnx_link.h"
#include "network.h"


int main(int argc, char *argv[]) {

    std::string xclbin;
    int our_id, their_id;

    if (argc != 4) {
        std::cout << "Usage: ./round <xclbin_file> <our fpga_id> <their fpga_id>" << std::endl;
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
        linkStatus = l1.getLinkStatus();
    }
    std::cout<<"link status up"<<std::endl;

    // get FPGA configuration.
    std::string our_ip = FPGA_config[our_id]["ip_addr"];
    std::string our_port = FPGA_config[our_id]["port"];
    std::string our_idx = FPGA_config[our_id]["idx"];
    std::string our_mac = FPGA_config[our_id]["MAC_addr"];
    std::cout << "our fpga config " << our_ip << our_port << our_idx << std::endl;

    std::string their_ip = FPGA_config[their_id]["ip_addr"];
    std::string their_port = FPGA_config[their_id]["port"];
    std::string their_idx = FPGA_config[their_id]["idx"];
    std::string their_mac = FPGA_config[their_id]["MAC_addr"];
    std::cout << "their fpga config " << their_ip << their_port << their_idx << std::endl;

    l1.setMyAddresses(our_ip, our_mac, stoi(our_port));
    std::cout << "link loop created" << std::endl;

    // enable tx and rx, need to enable rx firstly then tx.
    char *tx_buf = new char[SIZE_RX_BUFFER];
    for (int i = 0; i < SIZE_RX_BUFFER; i++) {
        tx_buf[i] = '0' + rand() % 10;
    }
    std::cout << " random number file generated "<< std::endl;
    
    char *rx_buf = new char[SIZE_RX_BUFFER]; // initial rx buffer.

    size_t size = l1.basicRecvSend(their_ip, stoi(their_port), tx_buf, rx_buf, SIZE_RX_BUFFER);
    // size_t size = l1.basicRecvSend("192.168.0.101", 5001, tx_buf, rx_buf, SIZE_RX_BUFFER);

    std::cout << "Packet received " << size << " bytes" << std::endl;

    std::cout << "data verifying ... " << std::endl;
    int count = 0;
    for (int i = 0; i < SIZE_RX_BUFFER; i++) {
        if (tx_buf[i] != rx_buf[i]) {
            count++;
            std::cout << "tx buf ["<<i<<"] :" << tx_buf[i] << " v.s. " << rx_buf[i] << std::endl;
        }
    }
    std::cout << count <<" data differ !"<< std::endl;
    // std::ofstream outfile("out.bin", std::ios::binary | std::ios::ate);
    // outfile.write(rx_buf, size);
    // outfile.close();

    return 0;
}
