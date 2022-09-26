## for round trip time test, unified function

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

def rtt_tx (id_our, id_their) :

    ip_our, port_our, idx_our = config_ip_port(id_our) ## get our ip and port
    ip_their, port_their, idx_their = config_ip_port(id_their) ## get their ip and port

    for i in range(len(pynq.Device.devices)):
        print("{}) {}".format(i, pynq.Device.devices[i].name))

    currentDevice = pynq.Device.devices[0]
    xclbin = '../benchmark.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_benchmark_if1.xclbin'
    ol = pynq.Overlay(xclbin,device=currentDevice)

    ## print(ol.ip_dict)

    while (ol.cmac_1.link_status() == False):
        print("Link interface 1 {}".format(ol.cmac_1.link_status()))
        time.sleep(1)

    print(ol.networklayer_1.set_ip_address(ip_our, debug=True)) ## own ip address

    ol.networklayer_1.sockets[2] = (ip_their, port_their, port_our, True)
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
    time.sleep(2)

    send_packets   = 2 ** 20
    shape          = (send_packets,1)
    rtt_cycles     = pynq.allocate(shape, dtype=np.uint32, target=ol.bank1)
    pkt            = pynq.allocate(1, dtype=np.uint32, target=ol.bank1)

    collector_h = ol.collector_1_2.start(rtt_cycles, pkt)

    send_pkts = send_packets
    ol_tg = ol.traffic_generator_1_2
    ol_tg.reset_stats()
    ol.networklayer_1.reset_debug_stats()

    ol_tg.start(TgMode.LATENCY, 2, send_pkts, 1, 50)

    rtt_cycles.sync_from_device()
    print(rtt_cycles)

    freq = int(ol.clock_dict['clock0']['frequency'])
    rtt_usec = np.array(shape, dtype=np.float64)
    rtt_usec= rtt_cycles / freq  # convert to microseconds

    from scipy import stats
    mean, std_dev, mode = np.mean(rtt_usec), np.std(rtt_usec), stats.mode(rtt_usec)
    print("Round trip time at application level using {:,} packets".format(len(rtt_usec)))
    print("\tmean    = {:.3f} us\n\tstd_dev = {:.6f} us".format(mean,std_dev))
    print("\tmode    = {:.3f} us, which appears {:,} times".format(mode[0][0][0],mode[1][0][0]))
    print("\tmax     = {:.3f} us".format(np.max(rtt_usec)))
    print("\tmin     = {:.3f} us".format(np.min(rtt_usec)))

    del rtt_cycles
    del pkt
    pynq.Overlay.free(ol)


def rtt_rx (id_our, id_their):

    ip_our, port_our, idx_our = config_ip_port(id_our) ## get our ip and port
    ip_their, port_their, idx_their = config_ip_port(id_their) ## get their ip and port

    for i in range(len(pynq.Device.devices)):
        print("{}) {}".format(i, pynq.Device.devices[i].name))

    currentDevice = pynq.Device.devices[0]
    xclbin = '../benchmark.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_benchmark_if1.xclbin'
    ol = pynq.Overlay(xclbin,device=currentDevice)

    ## print(ol.ip_dict)

    while (ol.cmac_1.link_status() == False):
        print("Link interface 1 {}".format(ol.cmac_1.link_status()))
        time.sleep(1)


    print(ol.networklayer_1.set_ip_address(ip_our, debug=True)) ## own ip address

    ol.networklayer_1.sockets[0] = (ip_their, port_their, port_our, True)
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

    ol_tg = ol.traffic_generator_1_0

    ol_tg.start(TgMode.LOOPBACK, 0)# Use connection in position 0 to reflect

    time.sleep(30) ## to keep program running, until tx loop finish.
    print("program exit")

    pynq.Overlay.free(ol)


if __name__ == '__main__' :
    parser = argparse.ArgumentParser("Round trip time measure ")
    parser.add_argument('--type', type=str,  choices=["master", "loop"], help='FPGA type')
    parser.add_argument('--our', type=int,  choices=[1, 2, 3, 4], help='our FPGA id')
    parser.add_argument('--the', type=int,  choices=[1, 2, 3, 4], help='their FPGA id')
    ## parser.add_argument('--interface', type=int,  choices=[0, 1], help='FPGA interface id in U250') ## current = 1
    args = parser.parse_args()
    print(args)

    if args.type == "master" :
        rtt_tx(args.our, args.the)
    else:
        rtt_rx(args.our, args.the)