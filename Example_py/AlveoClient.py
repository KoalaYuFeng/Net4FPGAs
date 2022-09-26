## alveo u250 Client. using basic.intf1......xclbin file.

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

## print(ol.ip_dict)

## print("Link interface 0 {}; link interface 1 {}".format(ol.cmac_0.link_status(),ol.cmac_1.link_status()))
print("Link interface 1 {}".format(ol.cmac_1.link_status()))

IpAddrServer = '197.11.27.12'
IpAddrClient = '197.11.27.55'
print(ol.networklayer_1.set_ip_address(IpAddrClient, debug=True))

ol.networklayer_1.sockets[0] = (IpAddrServer, 60133, 50446, True)
ol.networklayer_1.populate_socket_table(debug=True)
ol.networklayer_1.arp_discovery()
print(ol.networklayer_1.get_arp_table())


mm2s = ol.krnl_mm2s_1

size = 1408 * 100
shape = (size,1)

if hasattr(ol, 'HBM0'):
    mm2s_buf = pynq.allocate(shape, dtype=np.uint8, target=ol.HBM0)
else:
    mm2s_buf = pynq.allocate(shape, dtype=np.uint8, target=ol.bank1)

mm2s_buf[:] = np.random.randint(low=0, high=((2**8)-1), size=shape, dtype=np.uint8)
print(mm2s_buf)

## move data from Alveo Client to Alveo Server.
mm2s_buf.sync_to_device()
mm2s_wh = mm2s.start(mm2s_buf,size, 0)

print("Alveo client send data")

del mm2s_buf
pynq.Overlay.free(ol)
