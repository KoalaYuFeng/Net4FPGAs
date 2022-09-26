## for rtt test use, rx side, latency

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

ol.networklayer_1.sockets[2] = ('192.168.0.201', 60512, 62177, True)
ol.networklayer_1.populate_socket_table()
print(ol.networklayer_1.get_socket_table())

while (len(ol.networklayer_1.get_arp_table()) == 0):
    print("arp table is not ready ! ")
    ol.networklayer_1.arp_discovery()
    time.sleep(1)

print("find arp table ---- ")
print(ol.networklayer_1.get_arp_table())

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
