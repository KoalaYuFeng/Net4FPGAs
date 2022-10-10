// add function support for network class.
// modified by yufeng@nus.edu.sg.

#include "network.h"
#include "unistd.h"
#include <map>
#include <string>

// define the FPGA configuration for different id.
std::map<int, std::map<std::string, std::string>> FPGA_config = \
        {{1 , {{"ip_addr" , "192.168.0.201"}, {"port" , "60512"}, {"idx" , "201"}, {"MAC_addr" , "00:0a:35:02:9d:c9"}}}, \
         {2 , {{"ip_addr" , "192.168.0.202"}, {"port" , "62177"}, {"idx" , "202"}, {"MAC_addr" , "00:0a:35:02:9d:ca"}}}, \
         {3 , {{"ip_addr" , "192.168.0.203"}, {"port" , "61452"}, {"idx" , "203"}, {"MAC_addr" , "00:0a:35:02:9d:cb"}}}, \
         {4 , {{"ip_addr" , "192.168.0.204"}, {"port" , "60523"}, {"idx" , "204"}, {"MAC_addr" , "00:0a:35:02:9d:cc"}}}};

/**
* Network::Network() - class constructor
*
*
* Creates an object that combines CMAC, NetworkLayer and UDP kernels
*/
Network::Network(const FpgaDevice &device, uint32_t inst_id) {
    this->cmac = new AlveoVnxCmac(device, inst_id);
    this->nl = new AlveoVnxNetworkLayer(device, inst_id);
}


/**
* Network::~Network() - class destructor
*
*/
Network::~Network() {
    delete this->cmac;
    delete this->nl;
}


/**
* Network::getLinkStatus()
* return True or False.
*/
bool Network::getLinkStatus() {
    uint32_t status = this->cmac->readRegister("stat_rx_status");
    return (status & 0x1); // status is the last bit.
}


/**
* Network::setMyAddresses() - configures Network with supplied addresses
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
int Network::setMyAddresses(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port) {

    // conert IPv4 address string into 32b hex
    uint64_t a, b, c, d, e, f;
    char dot;
    std::stringstream ss_ip(ip_address);
    ss_ip >> a >> dot >> b >> dot >> c >> dot >> d;
    // std::cout <<' '<< a <<' '<< b <<' '<< c <<' '<< d  <<std::endl;
    uint32_t ip_hex = (a << 24) | (b << 16) | (c << 8) | d;

    this->ip = ip_hex;
    this->nl->writeRegister("my_ip", ip_hex);
    // uint32_t ip_add = this->nl->readRegister("my_ip");
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

    // uint32_t mac_msb = this->nl->readRegister("my_mac_msb");
    // uint32_t mac_lsb = this->nl->readRegister("my_mac_lsb");
    // std::cout << "mac_msb is "<< mac_msb << " mac_lsb is "<< mac_lsb <<std::endl;

    // assign UDP port
    this->udp = udp_port;
    // std::cout << std::dec << this->udp << std::endl;
    return 0;
}


/**
* Network::setSocketTable() - set socket table.
*
* @param dest_ip
*  string, destination IP address in format "192.168.0.1"
* @param dest_udp
*  uint16_t, destination UDP port as decimal or hex
* @param index
*  int, socket table index
* @return
*  int, 0: OK
*/
int Network::setSocketTable(const std::string &dest_ip, uint16_t dest_udp, int index) {
    this->nl->setSocket(dest_ip, dest_udp, this->udp, index);
    this->nl->getSocketTable();
    return 0;
}


/**
* Network::arpDiscovery() - run arp table discovery
*
* @param dest_ip
*  string, destination IP address to be found in arp table.
* @return
*  int, 0: OK
*/
int Network::arpDiscovery(const std::string &dest_ip) {
    bool ARP_ready = false;
    while(!ARP_ready) {
        std::cout << "wait ARP ready!" << std::endl;
        this->nl->runARPDiscovery();
        usleep(500000);
        ARP_ready = this->nl->IsARPTableFound(dest_ip);
    }
    return 0;
}
