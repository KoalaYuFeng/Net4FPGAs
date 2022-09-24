## for throuput test use, rx CONSUMER

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

print(ol.networklayer_1.set_ip_address('10.1.212.190', debug=True)) ## own ip address

ol.networklayer_1.sockets[3] = ('10.1.212.199', 62177, 60512, True)
ol.networklayer_1.populate_socket_table()
print(ol.networklayer_1.get_socket_table())
ol.networklayer_1.arp_discovery()
print(ol.networklayer_1.get_arp_table())


ol_tg = ol.traffic_generator_1_3
ol_tg.register_map.debug_reset = 1
ol_tg.start(TgMode.CONSUMER)

import time
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
