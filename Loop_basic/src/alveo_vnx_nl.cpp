// Copyright (C) FPGA-FAIS at Jagiellonian University Cracow
//
// SPDX-License-Identifier: BSD-3-Clause
//
// modified by yufeng@nus.edu.sg
// add getSocketTable() function.


#include "alveo_vnx_nl.h"

/**
* AlveoVnxNetworkLayer::AlveoVnxNetworkLayer() - class constructor
*
* @param device
*  xrt::device, particular Alveo device type to connect to
* @param uuid
*  xrt::uuid, unique ID of the Alveo device
* @param inst_id
*  uint32_t, instance id
*
* Creates an object representing VNX Network Layer IP
*/

AlveoVnxNetworkLayer::AlveoVnxNetworkLayer(const FpgaDevice &device, uint32_t inst_id) :
        FpgaIP::FpgaIP(device, "networklayer:{networklayer_" + std::to_string(inst_id) + "}") {

    // Attention : need to keep consistent with address in kernel.xml.
    this->registers_map["my_mac_lsb"] = 0x0010;
    this->registers_map["my_mac_msb"] = 0x0014;
    this->registers_map["my_ip"] = 0x0018;

    this->registers_map["arp_discovery"] = 0x1010;

    this->registers_map["arp_valid_offset"] = 0x1100;
    this->registers_map["arp_ip_addr_offset"] = 0x1400;
    this->registers_map["arp_mac_addr_offset_lsb"] = 0x1800;
    this->registers_map["arp_mac_addr_offset_msb"] = 0x1804;

    this->registers_map["eth_in_packets"] = 0x0410;
    this->registers_map["eth_out_packets"] = 0x04B8;

    this->registers_map["udp_theirIP_offset"] = 0x0810;
    this->registers_map["udp_theirPort_offset"] = 0x0890;
    this->registers_map["udp_myPort_offset"] = 0x0910;
    this->registers_map["udp_valid_offset"] = 0x0990;
    this->registers_map["udp_number_offset"] = 0x0A10;
}


/**
* AlveoVnxNetworkLayer::addSocket() - adds an entry into the HW register region
*
* @param remote_ip
*  string, IP address of the remote partner
* @param remote_udp
*  uint32_t, UDP port number of the remote partner
* @param local_udp
*  uint32_t, UDP port number of local socket
* @param socket_index
*  int, index of the slot socket to configure
* @return
*  int, 0: OK
*
* Adds an entry into the HW register region that allows to transmit and receive UDP packets
*/
int AlveoVnxNetworkLayer::setSocket(const std::string &remote_ip, uint32_t remote_udp, uint32_t local_udp, int socket_index) {

    // conert IPv4 address string into 32b hex
    uint32_t a, b, c, d;
    char dot;
    std::stringstream ss(remote_ip);
    ss >> a >> dot >> b >> dot >> c >> dot >> d;
    uint32_t ip_hex = (a << 24) | (b << 16) | (c << 8) | d;

    // store the socket addresses in socket region with concecutive index
    this->writeRegisterAddr(this->registers_map["udp_theirIP_offset"] + socket_index * 8, ip_hex);
    this->writeRegisterAddr(this->registers_map["udp_theirPort_offset"] + socket_index * 8, remote_udp);
    this->writeRegisterAddr(this->registers_map["udp_myPort_offset"] + socket_index * 8, local_udp);
    this->writeRegisterAddr(this->registers_map["udp_valid_offset"] + socket_index * 8, 1);

    return 0;
}


/**
* AlveoVnxNetworkLayer::runARPDiscovery() - fires the ARP procedure
*
* @return
*  int, 0: done
*
*/
int AlveoVnxNetworkLayer::runARPDiscovery() {
    this->writeRegister("arp_discovery", 0);
    this->writeRegister("arp_discovery", 1);
    this->writeRegister("arp_discovery", 0);

    return 0;
}


/**
* AlveoVnxNetworkLayer::IsARPTableFound(const std::string &dest_ip)
*  return whether the dest_ip is found by client.
*  // need to debug mac address.
*  max arp table number 256.
* @return
*  int, 0: done
*
*/
bool AlveoVnxNetworkLayer::IsARPTableFound(const std::string &dest_ip) {
    uint32_t valid_entry = 0, valid = 0;
    uint32_t mac_lsb, mac_msb, arp_ip;
    uint64_t mac_addr;

    uint32_t a, b, c, d;
    char dot;
    std::stringstream ss(dest_ip);
    ss >> a >> dot >> b >> dot >> c >> dot >> d;
    uint32_t dest_ip_dec = (a << 24) | (b << 16) | (c << 8) | d;

    for (int i = 0; i < 256; i++) { // max 256, only return valid arp table.
        if (i % 4 == 0)
            valid_entry = this->readRegister_offset("arp_valid_offset", (i / 4) * 4);

        valid = (valid_entry >> ((i % 4) * 8)) & 0x1;
        if (valid == 1) {
            mac_lsb = this->readRegister_offset("arp_mac_addr_offset_lsb", (i * 2 * 4));
            mac_msb = this->readRegister_offset("arp_mac_addr_offset_msb", ((i * 2 + 1) * 4));
            arp_ip  = this->readRegister_offset("arp_ip_addr_offset", (i * 4));
            mac_addr =  mac_msb << 32 + mac_lsb;

            uint32_t ip_addr[4];
            ip_addr[0] = (arp_ip & 0xff000000) >> 24;
            ip_addr[1] = (arp_ip &   0xff0000) >> 16;
            ip_addr[2] = (arp_ip &     0xff00) >> 8;
            ip_addr[3] =  arp_ip &       0xff;

            std::cout<< " ARP ip_addr " << ip_addr[3] << '.'  << ip_addr[2] << '.' << ip_addr[1] << '.' << ip_addr[0] << std::endl;
            uint32_t temp = ip_addr[3] * 256 * 256 *256 + ip_addr[2] * 256 * 256 + ip_addr[1] * 256 + ip_addr[0];
            // std::cout << std::hex << " mac_addr " << mac_msb << "..." << mac_lsb << "mac address"<< mac_addr << std::endl; // some problems in MAC address
            if (temp == dest_ip_dec) {
                std::cout << "arp find dest ip" << dest_ip << std::endl;
                return true;
            }
        }
    }
    return false;
}


/**
* AlveoVnxNetworkLayer::getSocketTable()
*  At current stage, FPGA side can only maintain max 16 size socket table.
* @return
*  int, 0: done
*
*/
int AlveoVnxNetworkLayer::getSocketTable() {
    // need to get it iteratively
    uint32_t socket_num = this->readRegister("udp_number_offset");
    uint32_t udp_their_ip, udp_their_port, udp_our_port, valid;

    for (int i = 0; i < socket_num; i++) {
        udp_their_ip    =  this->readRegister_offset("udp_theirIP_offset"   , i * 8);
        udp_their_port  =  this->readRegister_offset("udp_theirPort_offset" , i * 8);
        udp_our_port    =  this->readRegister_offset("udp_myPort_offset"    , i * 8);
        valid           =  this->readRegister_offset("udp_valid_offset"     , i * 8);

        if (valid == 1) { // only print valid ip port and address
            uint32_t ip_addr[4];
            ip_addr[0] = (udp_their_ip & 0xff000000) >> 24;
            ip_addr[1] = (udp_their_ip &   0xff0000) >> 16;
            ip_addr[2] = (udp_their_ip &     0xff00) >> 8;
            ip_addr[3] =  udp_their_ip &       0xff;

            std::cout<< "Socket table[" << i << "]" << "  Their ip_addr " << ip_addr[0] << '.'  \
                     << ip_addr[1] << '.' << ip_addr[2] << '.' << ip_addr[3] << "  Their port " \
                     << udp_their_port << "  Our port " << udp_our_port <<std::endl;         
        }
    }
    return 0;
}


/**
* AlveoVnxNetworkLayer::invalidSocketTable()
*  after data transaction, need to invalid all socket tables.
* @return
*  int, 0: done
*
*/
int AlveoVnxNetworkLayer::invalidSocketTable() {
    uint32_t socket_num = this->readRegister("udp_number_offset");
    uint32_t udp_their_ip, udp_their_port, udp_our_port, valid;

    for (int i = 0; i < socket_num; i++) {
        udp_their_ip    =  this->readRegister_offset("udp_theirIP_offset"   , i * 8);
        udp_their_port  =  this->readRegister_offset("udp_theirPort_offset" , i * 8);
        udp_our_port    =  this->readRegister_offset("udp_myPort_offset"    , i * 8);
        valid           =  this->readRegister_offset("udp_valid_offset"     , i * 8);

        if (valid == 1) { // only print valid ip port and address
            this->writeRegisterAddr(this->registers_map["udp_valid_offset"] + i * 8, 0);
            uint32_t ip_addr[4];
            ip_addr[0] = (udp_their_ip & 0xff000000) >> 24;
            ip_addr[1] = (udp_their_ip &   0xff0000) >> 16;
            ip_addr[2] = (udp_their_ip &     0xff00) >> 8;
            ip_addr[3] =  udp_their_ip &       0xff;

            std::cout<< "Invalid Socket table[" << i << "]" << "  Their ip_addr " << ip_addr[0] << '.'  \
                     << ip_addr[1] << '.' << ip_addr[2] << '.' << ip_addr[3] << "  Their port " \
                     << udp_their_port << "  Our port " << udp_our_port <<std::endl;         
        }
    }
    return 0;
}