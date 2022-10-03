* step 0: install pynq in FPGA machine.
```sh
pip install pynq // for XRT = 2.13.466, using pip install pynq==2.8.0.dev0
```
* Step 1: copy python files to server FPGA respectively.
* Step 2: run Xilinx runtime in two FPGAs.
```sh
source /opt/xilinx/xrt/setup.sh
```
* Step 3: run python script respectively.
```sh
python rtt_test.py --type master --our 1 --the 2 // for rtt test, in FPGA 1 node.
python rtt_test.py --type loop --our 2 --the 1 // in FPGA 2 node, loop_back.
```
```sh
python throu_test.py --type tx --our 1 --the 2 // for thsoughput test, in FPGA 1 node.
python throu_test.py --type rx --our 2 --the 1 // in FPGA 2 node.
```
```sh
python basic_test.py --type tx --our 1 --the 2 // for thsoughput test, in FPGA 1 node.
python basic_test.py --type rx --our 2 --the 1 // in FPGA 2 node.
```

* Result
Rtt point-to-point about 320 cycles, Rtt switch about 515 cycles.
Throughput point-to-point, can up to 90+ Gbps when ayload_size = 1472 bytes.
