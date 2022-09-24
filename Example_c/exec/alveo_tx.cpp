// Copyright (C) FPGA-FAIS at Jagiellonian University Cracow
//
// SPDX-License-Identifier: BSD-3-Clause

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>

#include "fpga_device.h"
#include "alveo_vnx_link.h"


int main(int argc, char *argv[]) {

    std::string xclbin;
    std::string file_to_transfer;
    int alveo_id;

    if (argc != 3) {
        std::cout << "Usage: ./tx <xclbin_file> <alveo_id>" << std::endl;
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

        alveo_id = atoi(argv[2]);
        if ( alveo_id < 0 || alveo_id > 15 ) {
            std::cout << "Make sure Alveo ID is a correct number" << std::endl;
            return EXIT_FAILURE;
        }

        // f = std::ifstream(argv[3]);
        // if (f.good() == false) {
        //     std::cout << "Make sure the path to the file <file_to_transfer> is correct" << std::endl;
        //     return EXIT_FAILURE;
        // }
        // f.close();
        // file_to_transfer = std::string(argv[3]);
    }

    auto u250 = FpgaDevice(alveo_id);
    
    // fetch the string with the card name and compare to the target
    std::string name = u250.getName();
    // if ( name.compare(ALVEO_DEVICE) != 0) {
    //     std::cerr << "ERR: FpgaDevice: could not connect to the target accelerator card" << std::endl;
    //     return EINVAL;
    // }
    std::cout << "Device created: " << u250.getName()<<std::endl;

    auto uuid = u250.loadBitfile(xclbin);
    std::cout << "Bitfile loaded: " << uuid << std::endl;
    

    auto l0 = AlveoVnxLink(u250, 1); // for interface 1
    l0.setMyAddresses("192.168.0.1", "ab:cd:ef:01:01", 10000);

    std::cout << "l0 created" << std::endl;

    // std::ifstream infile(file_to_transfer, std::ios::binary | std::ios::ate);
    // size_t infile_size = infile.tellg();
    // infile.seekg(0, std::ios::beg);

    // char *tx_buf = new char[infile_size];
    // infile.read(tx_buf, infile_size);
    // infile.close();

    char *tx_buf = new char[SIZE_TX_DATA];
    for (int i = 0; i < SIZE_TX_DATA; i++) {
        tx_buf[i] = '0' + rand() % 10;
    }

    std::cout<< "Transfering data array " << SIZE_TX_DATA << " bytes" << std::endl;

    l0.sendTo("192.168.0.2", 10001, tx_buf, SIZE_TX_DATA);

    std::cout << "Packet sent" << std::endl;

    return 0;
}
