## for rtt test use, rx side, LOOP_BACK

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

print(ol.networklayer_1.set_ip_address('10.1.212.167', debug=True)) ## own ip address

ol.networklayer_1.sockets[0] = ('10.1.212.165', 62177, 60512, True)
ol.networklayer_1.populate_socket_table()
print(ol.networklayer_1.get_socket_table())
ol.networklayer_1.arp_discovery()
print(ol.networklayer_1.get_arp_table())


ol_tg = ol.traffic_generator_1_0

ol_tg.start(TgMode.LOOPBACK, 0)# Use connection in position 0 to reflect

time.sleep(10) ## to keep program running, until tx loop finish.

pynq.Overlay.free(ol)
