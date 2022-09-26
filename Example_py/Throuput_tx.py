## for throuput test use, tx PRODUCER


import pynq
import numpy as np
from _thread import *
import threading 
import socket
from vnx_utils import *
import time


for i in range(len(pynq.Device.devices)):
    print("{}) {}".format(i, pynq.Device.devices[i].name))

currentDevice = pynq.Device.devices[0]
xclbin = '../benchmark.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_benchmark_if1.xclbin'
ol = pynq.Overlay(xclbin,device=currentDevice)

## print(ol.ip_dict)

## print("Link interface 0 {}; link interface 1 {}".format(ol.cmac_0.link_status(),ol.cmac_1.link_status()))
print("Link interface 1 {}".format(ol.cmac_1.link_status()))

print(ol.networklayer_1.set_ip_address('192.168.0.202', debug=True)) ## own ip address

ol.networklayer_1.sockets[12] = ('192.168.0.201', 60512, 62177, True)
ol.networklayer_1.populate_socket_table()
print(ol.networklayer_1.get_socket_table())

arp_ready = False
while (arp_ready == False):
    print("arp table is not ready ! ")
    ol.networklayer_1.arp_discovery()
    arp_table = ol.networklayer_1.get_arp_table()
    if arp_table.get(201) != None: ## find target FPGA MAC address
        arp_ready = True
    else:
        print(arp_table)
        time.sleep(5)

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
