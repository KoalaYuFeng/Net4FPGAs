// used for vnx_loop ip;


#pragma once

#include "alveo_vnx_cmac.h"
#include "alveo_vnx_nl.h"

#include "fpga_kernel.h"
#include "alveo_vnx_configs.h"


class AlveoVnxKrnlLoop : public FpgaKernel {
private:

    xrt::bo xrt_buffer;

public:
    AlveoVnxKrnlLoop(const FpgaDevice &device, uint32_t inst_id);
    int RunLoop();
};

class AlveoVnxLoop {

private:
    AlveoVnxCmac *cmac;
    AlveoVnxNetworkLayer *nl;
    AlveoVnxKrnlLoop *loop;

    uint32_t ip;
    uint64_t mac;
    uint16_t udp;

public:
    AlveoVnxLoop(const FpgaDevice &device, uint32_t inst_id);

    ~AlveoVnxLoop();

    int setMyAddresses(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port);

    int kernelLoop(const std::string &remote_ip, uint16_t port);

};
