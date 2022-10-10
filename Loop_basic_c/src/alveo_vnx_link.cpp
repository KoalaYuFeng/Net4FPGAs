// Copyright (C) FPGA-FAIS at Jagiellonian University Cracow
//
// SPDX-License-Identifier: BSD-3-Clause
//
// modified by yufeng@nus.edu.sg.
// add link status check function.
// modify set IP and MAC address function.
// modify sendTo and receive function.
// add basicRecvSend() function for loop test.


#include "alveo_vnx_link.h"
#include "unistd.h"

/**
* AlveoVnxLink::AlveoVnxLink() - class constructor
*
*
* Creates an object that combines CMAC, NetworkLayer and UDP kernels
*/
AlveoVnxLink::AlveoVnxLink(const FpgaDevice &device, uint32_t inst_id) {

    this->cmac = new AlveoVnxCmac(device, inst_id);
    this->nl = new AlveoVnxNetworkLayer(device, inst_id);
    this->rx = new AlveoVnxKrnlS2MM(device, inst_id);
    this->tx = new AlveoVnxKrnlMM2S(device, inst_id);

}


/**
* AlveoVnxLink::~AlveoVnxLink() - class destructor
*
*/
AlveoVnxLink::~AlveoVnxLink() {
    delete this->cmac;
    delete this->nl;
    delete this->tx;
    delete this->rx;
}


/**
* AlveoVnxLink::getLinkStatus()
* return True or False.
*/
bool AlveoVnxLink::getLinkStatus() {
    uint32_t status = this->cmac->readRegister("stat_rx_status");
    return (status & 0x1); // status is the last bit.
}


/**
* AlveoVnxLink::setMyAddresses() - configures VnxNetworkLayer with supplied addresses
*
* @param ip_address
*  string, IP address in format "192.168.0.1"
* @param mac_address
*  string, MAC address in format "ab:cd:ef:01:02"
* @param udp_port
*  uint16_t, UDP port as decimal or hex
* @return
*  int, 0: OK
*/
int AlveoVnxLink::setMyAddresses(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port) {

    // conert IPv4 address string into 32b hex
    uint64_t a, b, c, d, e, f;
    char dot;
    std::stringstream ss_ip(ip_address);
    ss_ip >> a >> dot >> b >> dot >> c >> dot >> d;
    // std::cout <<' '<< a <<' '<< b <<' '<< c <<' '<< d  <<std::endl;
    uint32_t ip_hex = (a << 24) | (b << 16) | (c << 8) | d;

    this->ip = ip_hex;
    this->nl->writeRegister("my_ip", ip_hex);
    uint32_t ip_add = this->nl->readRegister("my_ip");
    // std::cout << std::hex <<"ip_addr is "<< ip_add <<std::endl;

    // conert MAC address string into 48b hex
    std::stringstream ss_mac(mac_address);
    // std::cout<<"mac_adress "<< mac_address <<std::endl;
    std::string t = mac_address;
    std::string mac_temp = t.replace(2,1,"");
    mac_temp = t.replace(4,1,"");
    mac_temp = t.replace(6,1,"");
    mac_temp = t.replace(8,1,"");
    mac_temp = t.replace(10,1,"");
    // std::cout << "mac_temp" <<mac_temp<<std::endl; // 000a35029dc9;
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
    // std::cout<<"mac addr is "<<this->mac<<std::endl;

    this->nl->writeRegister("my_mac_msb", static_cast<uint32_t>((hex_mac >> 32)& 0x0000ffff)); //add a mask
    this->nl->writeRegister("my_mac_lsb", static_cast<uint32_t>(hex_mac & 0xffffffff));
    // std::cout <<" my_msb is "<< static_cast<uint32_t>(hex_mac >> 32) << \
                " my lsb is "<< static_cast<uint32_t>(hex_mac & 0xffffffff) <<std::endl;

    uint32_t mac_msb = this->nl->readRegister("my_mac_msb");
    uint32_t mac_lsb = this->nl->readRegister("my_mac_lsb");
    // std::cout << "mac_msb is "<< mac_msb << " mac_lsb is "<< mac_lsb <<std::endl;

    // assign UDP port
    this->udp = udp_port;
    // std::cout << std::dec << this->udp << std::endl;
    return 0;
}


/**
* AlveoVnxLink::sendTo() - transfers complete set of data to destination
*                                fragments into UDP packets when needed
*                                inserts an additional header word in front with EOF flag and size
*
* @param dest_ip
*  string, destination IP address in format "192.168.0.1"
* @param dest_udp
*  uint16_t, destination UDP port as decimal or hex
* @param buffer
*  char*, pointer to previously allocated and prepared memory with the payload
* @param size
*  size_t, size of the complete payload in bytes
* @return
*  int, 0: OK
*/
int AlveoVnxLink::sendTo(const std::string &dest_ip, uint16_t dest_udp, char *buffer, size_t size) {

    this->nl->setSocket(dest_ip, dest_udp, this->udp, 0);
    this->nl->getSocketTable();

    // In basic ip, tx need to check the right arp table before transfer data.
    bool ARP_ready = false;
    while(!ARP_ready) {
        std::cout << "wait ARP ready!" << std::endl;
        this->nl->runARPDiscovery();
        usleep(500000);
        ARP_ready = this->nl->IsARPTableFound(dest_ip);
    }
    sleep(2);

    this->tx->transferDataToKrnl(buffer, SIZE_RX_BUFFER);
    this->tx->sendPacket(0); // this is the socket table index, to index = 0;
    std::cout << "l0 packet sent" << std::endl;
  
    return 0;
}

/**
* AlveoVnxLink::receive() - receives data packets until EOF flag is received
*                                the data is reassembled and stored in external buffer
*
* @param src_ip
*  string, IP address of the sender in format "192.168.0.1"
* @param src_udp
*  uint16_t, UDP port of the sender as decimal or hex
* @param buffer
*  char*, pointer to previously allocated memory to store the received data
* @return
*  int, total size of received transaction in bytes
*/
int AlveoVnxLink::receive(const std::string &src_ip, uint16_t src_udp, char *buffer) {

    this->nl->setSocket(src_ip, src_udp, this->udp, 0);
    // this->nl->setSocket("192.168.0.142", 8065, this->udp, 1);  // for test
    this->nl->getSocketTable();
    bool ARP_ready = false;
    while(!ARP_ready) {
        std::cout << "wait ARP ready!" << std::endl;
        this->nl->runARPDiscovery();
        usleep(500000);
        ARP_ready = this->nl->IsARPTableFound(src_ip);
    }

    this->rx->receivePacket(SIZE_RX_BUFFER);
    this->rx->transferDataToHost(buffer);

    return SIZE_RX_BUFFER;
}

/**
* AlveoVnxLink::basicRecvSend() - unified basic function, bind original 
*                                   send and recv function into one function.
* @param remote_ip
*  string, IP address of the remote client
* @param src_udp
*  uint16_t, UDP port of the remote client
* @param tx_buffer
*  char*, pointer to previously allocated memory to store the transferred data
* @param rx_buffer
*  char*, pointer to previously allocated memory to store the received data
* @param size
*  size_t, size of the complete payload in bytes
* @return
*  int, total size of received transaction in bytes
*/

int AlveoVnxLink::basicRecvSend(const std::string &remote_ip, uint16_t remote_udp, 
                                char *tx_buffer, char *rx_buffer, size_t size) {

    this->nl->setSocket(remote_ip, remote_udp, this->udp, 0);
    this->nl->getSocketTable();

    bool ARP_ready = false;
    while(!ARP_ready) {
        std::cout << "wait ARP ready!" << std::endl;
        this->nl->runARPDiscovery();
        usleep(500000);
        ARP_ready = this->nl->IsARPTableFound(remote_ip);
    }
    sleep(2); // wait until another FPGA is ready.

    this->rx->launchKernel(SIZE_RX_BUFFER); // need to wait rx launch down. since the launch operation is async.
    this->tx->transferDataToKrnl(tx_buffer, SIZE_RX_BUFFER);
    sleep(5); // need to wait tx launch down 
    this->tx->sendPacket(0);
    std::cout << "tx packet sent" << std::endl;

    this->rx->syncKernel(); // wait rx receive data
    this->rx->transferDataToHost(rx_buffer);

    return SIZE_RX_BUFFER;
}