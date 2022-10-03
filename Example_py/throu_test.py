## for throughput test, unified function.

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


def throu_rx (id_our, id_their):

    ip_our, port_our, idx_our = config_ip_port(id_our) ## get our ip and port
    ip_their, port_their, idx_their = config_ip_port(id_their) ## get their ip and port

    for i in range(len(pynq.Device.devices)):
        print("{}) {}".format(i, pynq.Device.devices[i].name))

    currentDevice = pynq.Device.devices[0]
    xclbin = '../benchmark.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_benchmark_if1.xclbin'
    ol = pynq.Overlay(xclbin,device=currentDevice)

    ## print(ol.ip_dict)

    ## print("Link interface 0 {}; link interface 1 {}".format(ol.cmac_0.link_status(),ol.cmac_1.link_status()))
    print("Link interface 1 {}".format(ol.cmac_1.link_status()))
    print(ol.networklayer_1.set_ip_address(ip_our, debug=True)) ## own ip address

    ol.networklayer_1.sockets[3] = (ip_their, port_their, port_our, True)
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
            print(arp_table)
            time.sleep(2)

    print("find arp table ---- ")
    print(ol.networklayer_1.get_arp_table())

    ol_tg = ol.traffic_generator_1_3
    ol_tg.register_map.debug_reset = 1
    ol_tg.start(TgMode.CONSUMER)

    # overhead is UDP (8), IP (20), Ethernet(14) and FCS (4), IFG (12), preamble (7), start frame delimiter (1)
    overhead = 8 + 20 + 14 + 4 + 12 + 7 + 1
    freq = int(ol.clock_dict['clock0']['frequency'])
    experiment_dict = {}
    local_dict = {}
    ol_tg.freq = freq
    for pkt in [1_000_000, 1_000_000_000]:
        ol_tg.reset_stats()
        local_dict = {}
        for i in range(23):
            beats = i + 1
            rx_tot_pkt, rx_thr, rx_time = ol_tg.compute_app_throughput('rx')
            while rx_tot_pkt != pkt:
                rx_tot_pkt, rx_thr, rx_time = ol_tg.compute_app_throughput('rx') ## sync
                time.sleep(0.1)
            #Create dict entry for this particular experiment
            entry_dict = {'size': (beats * 64), 'rx_pkts' : rx_tot_pkt, 'rx_thr': rx_thr}
            local_dict[beats] = entry_dict
            # Reset probes to prepare for next computation
            ol_tg.reset_stats()
            theoretical = (beats * 64 * 100)/((beats*64) + overhead) 
            print("Sent {:14,} size: {:4}-Byte done!	Got {:14,} took {:8.4f} sec, thr: {:.3f} Gbps, theoretical: {:.3f} Gbps, difference: {:.4f} Gbps"\
                .format(pkt,beats*64, rx_tot_pkt, rx_time, rx_thr, theoretical, theoretical-rx_thr))
            time.sleep(0.5)
        experiment_dict[pkt] = local_dict

    pynq.Overlay.free(ol)



def throu_tx (id_our, id_their):

    ip_our, port_our, idx_our = config_ip_port(id_our) ## get our ip and port
    ip_their, port_their, idx_their = config_ip_port(id_their) ## get their ip and port

    for i in range(len(pynq.Device.devices)):
        print("{}) {}".format(i, pynq.Device.devices[i].name))

    currentDevice = pynq.Device.devices[0]
    xclbin = '../benchmark.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_benchmark_if1.xclbin'
    ol = pynq.Overlay(xclbin,device=currentDevice)

    ## print(ol.ip_dict)
    ## print("Link interface 0 {}; link interface 1 {}".format(ol.cmac_0.link_status(),ol.cmac_1.link_status()))
    print("Link interface 1 {}".format(ol.cmac_1.link_status()))

    print(ol.networklayer_1.set_ip_address(ip_our, debug=True)) ## own ip address

    ol.networklayer_1.sockets[12] = (ip_their, port_their, port_our, True)
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
            print(arp_table)
            time.sleep(2)

    print("find arp table ---- ")
    print(ol.networklayer_1.get_arp_table())
    time.sleep(5)

    freq = int(ol.clock_dict['clock0']['frequency'])
    ol_tg = ol.traffic_generator_1_3
    ## ol_tg.register_map.mode = TgMode.PRODUCER
    ol_tg.freq = freq
    for pkt in [1_000_000, 1_000_000_000]:
        ol_tg.reset_stats()
        for i in range(23):
            beats = i + 1
            ol_tg.register_map.number_beats = beats
            ol_tg.start(TgMode.PRODUCER, 12, pkt, beats, 0)
            while int(ol_tg.register_map.out_traffic_packets) != pkt:
                time.sleep(0.8)
            # Reset probes to prepare for next computation
            ol_tg.reset_stats()
            time.sleep(0.5)

    pynq.Overlay.free(ol)



if __name__ == '__main__' :
    parser = argparse.ArgumentParser("Throughput measure ")
    parser.add_argument('--type', type=str,  choices=["tx", "rx"], help='FPGA type')
    parser.add_argument('--our', type=int,  choices=[1, 2, 3, 4], help='our FPGA id')
    parser.add_argument('--the', type=int,  choices=[1, 2, 3, 4], help='their FPGA id')
    ## parser.add_argument('--interface', type=int,  choices=[0, 1], help='FPGA interface id in U250') ## current = 1
    args = parser.parse_args()
    print(args)

    if args.type == "tx" :
        throu_tx(args.our, args.the)
    else:
        throu_rx(args.our, args.the)
