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
    int alveo_id = 0; // set alveo_id = 0;

    if (argc != 2) {
        std::cout << "Usage: ./loop <xclbin_file>" << std::endl;
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
    
    auto l0 = AlveoVnxLoop(u250, 1);
    l0.setMyAddresses("192.168.0.2", "ab:cd:ef:02:02", 10001);

    std::cout << "link loop created" << std::endl;
    l0.kernelLoop("192.168.0.1", 10000);
    
    std::cout << "host side is sleeping, FPGA side is running" <<std::endl;

    return 0;
}
