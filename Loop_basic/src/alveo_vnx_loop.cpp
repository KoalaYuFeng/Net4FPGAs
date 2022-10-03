// used for vnx_loop project

#include "alveo_vnx_loop.h"
#include "unistd.h"

/**
* AlveoVnxLoop::AlveoVnxLoop() - class constructor
*
*
* Creates an object that combines CMAC, NetworkLayer and UDP kernels and loop kernel
*/
AlveoVnxLoop::AlveoVnxLoop(const FpgaDevice &device, uint32_t inst_id) {

    this->cmac = new AlveoVnxCmac(device, inst_id);
    this->nl = new AlveoVnxNetworkLayer(device, inst_id);
    this->loop = new AlveoVnxKrnlLoop(device, inst_id);

}


/**
* AlveoVnxLink::~AlveoVnxLoop() - class destructor
*
*/
AlveoVnxLoop::~AlveoVnxLoop() {
    delete this->cmac;
    delete this->nl;
    delete this->loop;
}


int AlveoVnxLoop::setMyAddresses(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port) {

    // conert IPv4 address string into 32b hex
    uint64_t a, b, c, d, e, f;
    char dot;
    std::stringstream ss_ip(ip_address);
    ss_ip >> a >> dot >> b >> dot >> c >> dot >> d;
    uint32_t ip_hex = (a << 24) | (b << 16) | (c << 8) | d;

    this->ip = ip_hex;

    this->nl->writeRegister("my_ip", ip_hex);

    // conert MAC address string into 48b hex
    std::stringstream ss_mac(mac_address);
    ss_mac >> a >> dot >> b >> dot >> c >> dot >> d >> dot >> e >> dot >> f;
    uint64_t mac_hex = (a << 40) | (b << 32) | (c << 24) | (d << 16) | (e << 8) | f;

    this->mac = mac_hex;

    this->nl->writeRegister("my_mac_msb", static_cast<uint32_t>(mac_hex >> 32));
    this->nl->writeRegister("my_mac_lsb", static_cast<uint32_t>(mac_hex & 0xffffffff));

    // assign UDP port
    this->udp = udp_port;

    return 0;
}

int AlveoVnxLoop::kernelLoop(const std::string &remote_ip, uint16_t port) {
    this->nl->setSocket(remote_ip, port, this->udp, 0); // set dest as 0
    this->nl->runARPDiscovery();
    // this->nl->IsARPTableFound(); // add print ARp table function.
    // ARP discovery may takes time. // print it;
    this->loop->RunLoop();
    return 0;
}

AlveoVnxKrnlLoop::AlveoVnxKrnlLoop(const FpgaDevice &device, uint32_t inst_id) :
    FpgaKernel::FpgaKernel(device, "krnl_loop:{krnl_loop_" + std::to_string(inst_id) + "}") {

}

int AlveoVnxKrnlLoop::RunLoop() {
    this->xrt_buffer = xrt::bo(this->device, 64, this->kernel.group_id(0)); // fixed size 512/8=64 byte
    auto run = xrt::run(this->kernel);
    run.set_arg(0, this->xrt_buffer);
    run.set_arg(3, 0); // dest = 0;

    // aschronus kernel start
    run.start();
    std::cout<<"start loop_back function, keeps waiting"<<std::endl;
    // keep the kernel running.
    run.wait();

    return 0;
}

