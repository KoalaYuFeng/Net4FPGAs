// used for loop kernel test.
// yufeng@nus.edu.sg
// add NetDataLoopback struct.
// add RunLoop() function.

#include "user_kernel_loop.h"
#include "unistd.h"

/**
* NetDataLoopback::NetDataLoopback() - class constructor
*
* Creates an object that combines CMAC, NetworkLayer and UDP kernels and loop kernel
*/
NetDataLoopback::NetDataLoopback(const FpgaDevice &device, uint32_t inst_id) {
    this->net = new Network(device, inst_id);
    this->loop = new AlveoVnxKrnlLoop(device, inst_id);
}


/**
* NetDataLoopback::~NetDataLoopback() - class destructor
*
*/
NetDataLoopback::~NetDataLoopback() {
    delete this->net;
    delete this->loop;
}

/**
* NetDataLoopback::getLinkStatus()
* return True or False.
*/
bool NetDataLoopback::getLinkStatus() {
    uint32_t status = this->net->getLinkStatus();
    return (status & 0x1); // status is the last bit.
}

/**
* NetDataLoopback::kernelLoop() - run loop back function.
*
*/
int NetDataLoopback::kernelLoop(const std::string &remote_ip, uint16_t port) {
    this->net->setSocketTable(remote_ip, port, 0);
    this->net->arpDiscovery(remote_ip);
    this->loop->RunLoop();
    return 0;
}

// /**
// * AlveoVnxKrnlLoop::AlveoVnxKrnlLoop() - class constructor
// *
// */
// AlveoVnxKrnlLoop::AlveoVnxKrnlLoop(const FpgaDevice &device, uint32_t inst_id) :
//     FpgaKernel::FpgaKernel(device, "krnl_loop:{krnl_loop_" + std::to_string(inst_id) + "}") {

// }

// /**
// * AlveoVnxKrnlLoop::RunLoop() - run loop function.
// *
// */
// int AlveoVnxKrnlLoop::RunLoop() {
//     this->xrt_buffer = xrt::bo(this->device, 64, this->kernel.group_id(0)); // fixed size 512/8=64 byte
//     auto run = xrt::run(this->kernel);
//     run.set_arg(0, this->xrt_buffer);
//     run.set_arg(3, 0); // dest = 0;
//     // aschronus kernel start
//     run.start();
//     std::cout<<"start loop_back function, keeps running"<<std::endl;
//     // keep the kernel running.
//     run.wait();
//     return 0;
// }

/**
* NetDataLoopback::setNetAddress()
*
*/
int NetDataLoopback::setNetAddress(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port) {
    this->net->setMyAddresses(ip_address, mac_address, udp_port);
}