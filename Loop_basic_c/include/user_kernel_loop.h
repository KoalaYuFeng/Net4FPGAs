// This file defines the user kernel in design.
// In loop back transfer, it contains loop kernel and network object.

#pragma once

#include "fpga_kernel.h"
#include "alveo_vnx_configs.h"
#include "network.h"

/**
 * @brief AlveoVnxKrnlLoop -- user kernel for loopback function.
 * 
 */
class AlveoVnxKrnlLoop : public FpgaKernel {
private:
    xrt::bo xrt_buffer;
public:
    AlveoVnxKrnlLoop(const FpgaDevice &device, uint32_t inst_id);
    int RunLoop();
};


/**
 * @brief NetDataLoopback
 * 
 */
class NetDataLoopback {

private:
    Network          *net;
    AlveoVnxKrnlLoop *loop;

public:
    NetDataLoopback(const FpgaDevice &device, uint32_t inst_id);
    ~NetDataLoopback();
    int kernelLoop(const std::string &remote_ip, uint16_t port);
    bool getLinkStatus();
    int setNetAddress(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port);
};