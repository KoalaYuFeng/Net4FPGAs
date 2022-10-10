// Copyright (C) FPGA-FAIS at Jagiellonian University Cracow
//
// SPDX-License-Identifier: BSD-3-Clause
// 
// modified by yufeng@nus.edu.sg.
// add FPGA configuration.
// combine cmac and networklayer into Network class.


#pragma once

#include "alveo_vnx_cmac.h"
#include "alveo_vnx_nl.h"
#include <map>
#include <string>

extern std::map<int, std::map<std::string, std::string>> FPGA_config;
/**
 * @brief Network class
 * encapsulates cmac and networklayer
 * 
 */
class Network {

private:
    AlveoVnxCmac *cmac;
    AlveoVnxNetworkLayer *nl;
    uint32_t ip;
    uint64_t mac;
    uint16_t udp;

public:
    Network(const FpgaDevice &device, uint32_t inst_id);
    ~Network();
    bool getLinkStatus();
    int setMyAddresses(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port);
    int setSocketTable(const std::string &dest_ip, uint16_t dest_udp, int index);
    int arpDiscovery(const std::string &dest_ip);
};