// Copyright (C) FPGA-FAIS at Jagiellonian University Cracow
//
// SPDX-License-Identifier: BSD-3-Clause
// 
// modified by yufeng@nus.edu.sg.
// add FPGA configuration.


#pragma once

#include "alveo_vnx_cmac.h"
#include "alveo_vnx_nl.h"
#include "alveo_vnx_krnl_mm2s.h"
#include "alveo_vnx_krnl_s2mm.h"
#include <map>
#include <string>

// define the FPGA configuration for different id.
std::map<int, std::map<std::string, std::string>> FPGA_config = \
        {{1 , {{"ip_addr" , "192.168.0.201"}, {"port" , "60512"}, {"idx" , "201"}, {"MAC_addr" , "00:0a:35:02:9d:c9"}}}, \
         {2 , {{"ip_addr" , "192.168.0.202"}, {"port" , "62177"}, {"idx" , "202"}, {"MAC_addr" , "00:0a:35:02:9d:ca"}}}, \
         {3 , {{"ip_addr" , "192.168.0.203"}, {"port" , "61452"}, {"idx" , "203"}, {"MAC_addr" , "00:0a:35:02:9d:cb"}}}, \
         {4 , {{"ip_addr" , "192.168.0.204"}, {"port" , "60523"}, {"idx" , "204"}, {"MAC_addr" , "00:0a:35:02:9d:cc"}}}};

/**
 * @brief AlveoVnxLink class
 * 
 * Encapsulates all required objects to tansmit and receive UDP packets
 * using the XUP VNx design.
 * 
 */

class AlveoVnxLink {

private:
    AlveoVnxCmac *cmac;
    AlveoVnxNetworkLayer *nl;
    AlveoVnxKrnlS2MM *rx;
    AlveoVnxKrnlMM2S *tx;

    uint32_t ip;
    uint64_t mac;
    uint16_t udp;

public:
    AlveoVnxLink(const FpgaDevice &device, uint32_t inst_id);

    ~AlveoVnxLink();

    int setMyAddresses(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port);

    int sendTo(const std::string &dest_ip, uint16_t dest_udp, char *buffer, size_t size);

    int receive(const std::string &src_ip, uint16_t src_udp, char *buffer);

    int basicRecvSend(const std::string &remote_ip, uint16_t remote_udp, char *tx_buffer, char *rx_buffer, size_t size);

    bool getLinkStatus();

};