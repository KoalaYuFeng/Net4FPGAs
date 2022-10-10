// used for loop kernel test.
// yufeng@nus.edu.sg
// add AlveoVnxLoop struct.
// add RunLoop() function.

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

/**
* AlveoVnxLoop::getLinkStatus()
* return True or False.
*/
bool AlveoVnxLoop::getLinkStatus() {
    uint32_t status = this->cmac->readRegister("stat_rx_status");
    return (status & 0x1); // status is the last bit.
}


int AlveoVnxLoop::setMyAddresses(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port) {

    // conert IPv4 address string into 32b hex
    uint64_t a, b, c, d, e, f;
    char dot;
    std::stringstream ss_ip(ip_address);
    ss_ip >> a >> dot >> b >> dot >> c >> dot >> d;
    std::cout <<' '<< a <<' '<< b <<' '<< c <<' '<< d  <<std::endl;
    uint32_t ip_hex = (a << 24) | (b << 16) | (c << 8) | d;

    this->ip = ip_hex;
    this->nl->writeRegister("my_ip", ip_hex);
    uint32_t ip_add = this->nl->readRegister("my_ip");
    std::cout << std::hex <<"ip_addr is "<< ip_add <<std::endl;

    // conert MAC address string into 48b hex
    std::stringstream ss_mac(mac_address);
    std::cout<<"mac_adress "<< mac_address <<std::endl;
    std::string t = mac_address;
    std::string mac_temp = t.replace(2,1,"");
    mac_temp = t.replace(4,1,"");
    mac_temp = t.replace(6,1,"");
    mac_temp = t.replace(8,1,"");
    mac_temp = t.replace(10,1,"");
    std::cout << "mac_temp" <<mac_temp<<std::endl; // 000a35029dc9;
    uint64_t hex_mac = 0;
    for (int i = 0; i < 12; i++) {
        char temp = mac_temp.at(i);
        if ((temp <= 'f') && (temp >= 'a')) hex_mac += ((temp - 'a' + 10UL) << (11 - i)*4);
        if ((temp <= 'F') && (temp >= 'A')) hex_mac += ((temp - 'A' + 10UL) << (11 - i)*4);
        if ((temp <= '9') && (temp >= '0')) hex_mac += ((temp - '0' +  0UL) << (11 - i)*4);
    }
    // ss_mac >> a >> dot >> b >> dot >> c >> dot >> d >> dot >> e >> dot >> f;
    // std::cout <<' '<< a <<' '<< b <<' '<< c <<' '<< d  <<' '<< e <<' '<< f <<' '<<std::endl;
    // uint64_t hex_mac = (a << 40) | (b << 32) | (c << 24) | (d << 16) | (e << 8) | f;

    this->mac = hex_mac;
    std::cout<<"mac addr is "<<this->mac<<std::endl;

    this->nl->writeRegister("my_mac_msb", static_cast<uint32_t>((hex_mac >> 32)& 0x0000ffff)); //add a mask
    this->nl->writeRegister("my_mac_lsb", static_cast<uint32_t>(hex_mac & 0xffffffff));
    std::cout <<" my_msb is "<< static_cast<uint32_t>(hex_mac >> 32) << \
                " my lsb is "<< static_cast<uint32_t>(hex_mac & 0xffffffff) <<std::endl;

    uint32_t mac_msb = this->nl->readRegister("my_mac_msb");
    uint32_t mac_lsb = this->nl->readRegister("my_mac_lsb");
    std::cout << "mac_msb is "<< mac_msb << " mac_lsb is "<< mac_lsb <<std::endl;

    // assign UDP port
    this->udp = udp_port;
    std::cout << std::dec << this->udp << std::endl;
    return 0;
}

int AlveoVnxLoop::kernelLoop(const std::string &remote_ip, uint16_t port) {
    this->nl->setSocket(remote_ip, port, this->udp, 0); // set dest as 0
    // this->nl->setSocket("192.168.0.101", 5001, this->udp, 1);
    this->nl->runARPDiscovery();

    bool ARP_ready = false;
    while(!ARP_ready) {
        std::cout << "wait ARP ready!" << std::endl;
        this->nl->runARPDiscovery();
        usleep(500000);
        ARP_ready = this->nl->IsARPTableFound(remote_ip);
    }

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
    std::cout<<"start loop_back function, keeps running"<<std::endl;
    // keep the kernel running.
    // run.wait();
    sleep(1);

    return 0;
}

