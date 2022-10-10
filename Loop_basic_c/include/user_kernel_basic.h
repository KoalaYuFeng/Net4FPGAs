// This file defines the user kernel in design.
// In basic data transfer, it contains MM2S, S2MM and Network.

#pragma once

#include "fpga_kernel.h"
#include "alveo_vnx_configs.h"
#include "network.h"

/**
 * @brief AlveoVnxKrnlS2MM class
 * transfer AXI stream data into Memory Map.
 * 
 */
class AlveoVnxKrnlS2MM : public FpgaKernel {

private:
    xrt::bo xrt_buffer;
    uint32_t buffer_size;
    xrt::run xrt_run;

public:
    AlveoVnxKrnlS2MM(const FpgaDevice &device, uint32_t inst_id);
    int resetBuffer();
    int transferDataToHost(char *data);
    int receivePacket(size_t size);
    int launchKernel(size_t size); // divide receivePacket into two separate functions.
    int syncKernel();
};


/**
 * @brief AlveoVnxKrnlMM2S
 * transfer Memory Map into AXI stream data.
 * 
 */
class AlveoVnxKrnlMM2S : public FpgaKernel {

private:
    xrt::bo xrt_buffer;
    size_t buffer_size;

public:
    AlveoVnxKrnlMM2S(const FpgaDevice &device, uint32_t inst_id);
    int transferDataToKrnl(char *data, size_t size);
    int sendPacket(uint32_t dest_socket);
};


/**
 * @brief NetDataTransfer
 * 
 * contains MM2S and S2MM and network.
 * 
 */
class NetDataTransfer {

private:
    AlveoVnxKrnlS2MM *rx;
    AlveoVnxKrnlMM2S *tx;
    Network          *net;

public:
    NetDataTransfer(const FpgaDevice &device, uint32_t inst_id);
    ~NetDataTransfer();
    bool getLinkStatus();
    int sendTo(const std::string &dest_ip, uint16_t dest_udp, char *buffer, size_t size);
    int receive(const std::string &src_ip, uint16_t src_udp, char *buffer);
    int basicRecvSend(const std::string &remote_ip, uint16_t remote_udp, char *tx_buffer, char *rx_buffer, size_t size);
    int setNetAddress(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port);
};