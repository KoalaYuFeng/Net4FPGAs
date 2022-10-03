## for basic data transfer test, unified function
## in this case, rx and tx must know the data size.

import pynq
import numpy as np
from _thread import *
import threading 
import socket
from vnx_utils import *
import time
import argparse

def config_ip_port(id) :
    device_dict = { 1 : { 'ip' : '192.168.0.201', 'port' : 60512, 'idx' : 201}, \
                    2 : { 'ip' : '192.168.0.202', 'port' : 62177, 'idx' : 202}, \
                    3 : { 'ip' : '192.168.0.203', 'port' : 61452, 'idx' : 203}, \
                    4 : { 'ip' : '192.168.0.204', 'port' : 60523, 'idx' : 204}}
    print (device_dict)
    return device_dict[id]['ip'], device_dict[id]['port'], device_dict[id]['idx']

def basic_tx (id_our, id_their) :

    ip_our, port_our, idx_our = config_ip_port(id_our) ## get our ip and port
    ip_their, port_their, idx_their = config_ip_port(id_their) ## get their ip and port

    for i in range(len(pynq.Device.devices)):
        print("{}) {}".format(i, pynq.Device.devices[i].name))
    
    currentDevice = pynq.Device.devices[0]
    xclbin = '../basic.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_basic_if1.xclbin'
    ol = pynq.Overlay(xclbin,device=currentDevice)

    ## check link status
    while (ol.cmac_1.link_status() == False):
        print("Link interface 1 {}".format(ol.cmac_1.link_status()))
        time.sleep(1)
    
    print(ol.networklayer_1.set_ip_address(ip_our, debug=True)) ## own ip address

    ol.networklayer_1.sockets[0] = (ip_their, port_their, port_our, True) ## set socket
    ol.networklayer_1.populate_socket_table()
    print(ol.networklayer_1.get_socket_table())

    arp_ready = False
    while (arp_ready == False):
        print("arp table is not ready ! ")
        ol.networklayer_1.arp_discovery()
        arp_table = ol.networklayer_1.get_arp_table()
        if arp_table.get(idx_their) != None: ## find target FPGA MAC address
            arp_ready = True
        else:
            time.sleep(1)

    print("find arp table ---- ")
    print(ol.networklayer_1.get_arp_table())
    time.sleep(1)

    mm2s = ol.krnl_mm2s_1
    size = 1408 * 100
    shape = (size,1)
    if hasattr(ol, 'HBM0'):
        mm2s_buf = pynq.allocate(shape, dtype=np.uint8, target=ol.HBM0)
    else:
        mm2s_buf = pynq.allocate(shape, dtype=np.uint8, target=ol.bank1)

    mm2s_buf[:] = np.random.randint(low=0, high=((2**8)-1), size=shape, dtype=np.uint8)
    print(mm2s_buf)
    mm2s_buf.sync_to_device() ## flush data to device memory
    mm2s_wh = mm2s.start(mm2s_buf,size, 0)

    print("Alveo client send data")

    del mm2s_buf
    pynq.Overlay.free(ol)


def basic_rx (id_our, id_their) :

    ip_our, port_our, idx_our = config_ip_port(id_our) ## get our ip and port
    ip_their, port_their, idx_their = config_ip_port(id_their) ## get their ip and port

    for i in range(len(pynq.Device.devices)):
        print("{}) {}".format(i, pynq.Device.devices[i].name))

    currentDevice = pynq.Device.devices[0]
    xclbin = '../basic.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_basic_if1.xclbin'
    ol = pynq.Overlay(xclbin,device=currentDevice)

    ## check link status
    while (ol.cmac_1.link_status() == False):
        print("Link interface 1 {}".format(ol.cmac_1.link_status()))
        time.sleep(1)

    print(ol.networklayer_1.set_ip_address(ip_our, debug=True)) ## own ip address

    ol.networklayer_1.sockets[0] = (ip_their, port_their, port_our, True) ## set socket
    ol.networklayer_1.populate_socket_table()
    print(ol.networklayer_1.get_socket_table())

    arp_ready = False
    while (arp_ready == False):
        print("arp table is not ready ! ")
        ol.networklayer_1.arp_discovery()
        arp_table = ol.networklayer_1.get_arp_table()
        if arp_table.get(idx_their) != None: ## find target FPGA MAC address
            arp_ready = True
        else:
            time.sleep(1)

    print("find arp table ---- ")
    print(ol.networklayer_1.get_arp_table())

    s2mm = ol.krnl_s2mm_1
    size = 1408 * 100
    shape = (size,1)
    if hasattr(ol, 'HBM0'):
        s2mm_buf = pynq.allocate(shape, dtype=np.uint8, target=ol.HBM0)
    else:
        s2mm_buf = pynq.allocate(shape, dtype=np.uint8, target=ol.bank1)

    s2mm_wh = s2mm.start(s2mm_buf,size)
    print("Basic rx waiting....")
    s2mm_wh.wait()
    s2mm_buf.sync_from_device()
    print("Basic rx data received")
    print(s2mm_buf)

    del s2mm_buf
    pynq.Overlay.free(ol)


if __name__ == '__main__' :
    parser = argparse.ArgumentParser("Basic data transfer test")
    parser.add_argument('--type', type=str,  choices=["tx", "rx"], help='FPGA type')
    parser.add_argument('--our', type=int,  choices=[1, 2, 3, 4], help='our FPGA id')
    parser.add_argument('--the', type=int,  choices=[1, 2, 3, 4], help='their FPGA id')
    ## parser.add_argument('--interface', type=int,  choices=[0, 1], help='FPGA interface id in U250') ## current = 1
    args = parser.parse_args()
    print(args)

    if args.type == "tx" :
        basic_tx(args.our, args.the)
    else:
        basic_rx(args.our, args.the)