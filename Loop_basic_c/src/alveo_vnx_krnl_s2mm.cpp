// Copyright (C) FPGA-FAIS at Jagiellonian University Cracow
//
// SPDX-License-Identifier: BSD-3-Clause



#include "alveo_vnx_krnl_s2mm.h"

/**
* AlveoVnxKrnlS2MM::AlveoVnxKrnlS2MM() - class constructor
*
* @param device
*  xrt::device, particular Alveo device type to connect to
* @param uuid
*  xrt::uuid, unique ID of the Alveo device
* @param inst_id
*  uint32_t, instance id
*
* Creates an object representing VNX UDP TX
*/

AlveoVnxKrnlS2MM::AlveoVnxKrnlS2MM(const FpgaDevice &device, uint32_t inst_id) :
        FpgaKernel::FpgaKernel(device, "krnl_s2mm:{krnl_s2mm_" + std::to_string(inst_id) + "}"), buffer_size{} {

}

int AlveoVnxKrnlS2MM::resetBuffer() {
    return 0;
}

/**
* AlveoVnxKrnlS2MM::transferDataToHost() - transfers size amount of bytes to data pointer
*
* @param data
*  char*, pointer to allocated memory
* @return
*  0: OK, 
*/
int AlveoVnxKrnlS2MM::transferDataToHost(char *data) {
    this->xrt_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    this->xrt_buffer.read(data);
    return 0;
}


/**
* AlveoVnxKrnlS2MM::receivePacket() - fires the kernel to receive packets
*
* @param src_socket
*  uint32_t, ID of the predefined socket to use
* @param size
*  size_t, amount of bytes to receive
* @return
*  int, 0: OK
*/
int AlveoVnxKrnlS2MM::receivePacket(size_t size) {

    this->xrt_buffer = xrt::bo(this->device, size, this->kernel.group_id(0));

    auto run = xrt::run(this->kernel);
    run.set_arg(0, this->xrt_buffer);
    run.set_arg(2, size);

    // aschronus kernel start
    run.start();
    // wait until kernel finishes
    run.wait();

    return 0;
}

int AlveoVnxKrnlS2MM::launchKernel(size_t size) {
    this->xrt_buffer = xrt::bo(this->device, size, this->kernel.group_id(0));
    this->xrt_run = xrt::run(this->kernel);
    this->xrt_run.set_arg(0, this->xrt_buffer);
    this->xrt_run.set_arg(2, size);
    // aschronus kernel start
    this->xrt_run.start();
    std::cout << "kernel launched" << std::endl;
    return 0;
}

int AlveoVnxKrnlS2MM::syncKernel() {
    // wait until kernel finishes
    std::cout << "kernel waiting" << std::endl;
    this->xrt_run.wait(); // wait 1000 ms
    return 0;
}


