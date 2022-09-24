## alveo u250 Server. using basic.intf1.xilinx....  xclbin file.

import pynq
import numpy as np
from _thread import *
import threading 
import socket
from vnx_utils import *


for i in range(len(pynq.Device.devices)):
    print("{}) {}".format(i, pynq.Device.devices[i].name))

currentDevice = pynq.Device.devices[0]
xclbin = '../basic.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_basic_if1.xclbin'
ol = pynq.Overlay(xclbin,device=currentDevice)

##print(ol.ip_dict)

## print("Link interface 0 {}; link interface 1 {}".format(ol.cmac_0.link_status(),ol.cmac_1.link_status()))
print("Link interface 1 {}".format(ol.cmac_1.link_status()))


IpAddrServer = '197.11.27.12'
print(ol.networklayer_1.set_ip_address(IpAddrServer, debug=True))

IpAddrClient = '197.11.27.55'

ol.networklayer_1.sockets[0] = (IpAddrClient, 50446, 60133, True)
ol.networklayer_1.populate_socket_table(debug=True)
ol.networklayer_1.arp_discovery()
ol.networklayer_1.get_arp_table()


s2mm = ol.krnl_s2mm_1
size = 1408 * 100
shape = (size,1)
if hasattr(ol, 'HBM0'):
    s2mm_buf = pynq.allocate(shape, dtype=np.uint8, target=ol.HBM0)
else:
    s2mm_buf = pynq.allocate(shape, dtype=np.uint8, target=ol.bank1)

## move data from Alveo Client to Alveo Server.
s2mm_wh = s2mm.start(s2mm_buf,size)
print("Alveo server waiting....")
s2mm_wh.wait()
s2mm_buf.sync_from_device()
print("Alveo server received")
print(s2mm_buf)

del s2mm_buf
pynq.Overlay.free(ol)
