// This file defines the user kernel in design.
// In basic data transfer, it contains MM2S and S2MM.

#include "user_kernel_basic.h"
#include "unistd.h"

/**
* NetDataTransfer::NetDataTransfer() - class constructor
* Creates an object that combines user kernels
*/
NetDataTransfer::NetDataTransfer(const FpgaDevice &device, uint32_t inst_id) {
    this->rx = new AlveoVnxKrnlS2MM(device, inst_id);
    this->tx = new AlveoVnxKrnlMM2S(device, inst_id);
    this->net = new Network(device, inst_id);
}


/**
* NetDataTransfer::~NetDataTransfer() - class destructor
*
*/
NetDataTransfer::~NetDataTransfer() {
    delete this->tx;
    delete this->rx;
    delete this->net;
}

/**
* NetDataTransfer::getLinkStatus()
* return True or False.
*/
bool NetDataTransfer::getLinkStatus() {
    uint32_t status = this->net->getLinkStatus();
    return (status & 0x1); // status is the last bit.
}

/**
* NetDataTransfer::sendTo() - transfers complete set of data to destination
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
int NetDataTransfer::sendTo(const std::string &dest_ip, uint16_t dest_udp, char *buffer, size_t size) {

    this->net->setSocketTable(dest_ip, dest_udp, 0);
    this->net->arpDiscovery(dest_ip);
    sleep(2);

    this->tx->transferDataToKrnl(buffer, SIZE_RX_BUFFER);
    this->tx->sendPacket(0); // this is the socket table index, to index = 0;
    std::cout << "l0 packet sent" << std::endl;
    return 0;
}

/**
* NetDataTransfer::receive() - receives data packets from remote FPGA
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
int NetDataTransfer::receive(const std::string &src_ip, uint16_t src_udp, char *buffer) {
    this->net->setSocketTable(src_ip, src_udp, 0);
    this->net->arpDiscovery(src_ip);
    this->rx->receivePacket(SIZE_RX_BUFFER);
    this->rx->transferDataToHost(buffer);
    return SIZE_RX_BUFFER;
}

/**
* NetDataTransfer::basicRecvSend() - unified basic function, bind original 
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

int NetDataTransfer::basicRecvSend(const std::string &remote_ip, uint16_t remote_udp, 
                                char *tx_buffer, char *rx_buffer, size_t size) {

    this->net->setSocketTable(remote_ip, remote_udp, 0);
    this->net->arpDiscovery(remote_ip);
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

/**
* NetDataTransfer::setNetAddress()
*
*/
int NetDataTransfer::setNetAddress(const std::string &ip_address, const std::string &mac_address, uint16_t udp_port) {
    this->net->setMyAddresses(ip_address, mac_address, udp_port);
}