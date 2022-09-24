* step 0: install pynq in FPGA machine.
```sh
pip install pynq // for XRT = 2.13.466, using pip install pynq==2.8.0.dev0
```
* Step 1: copy AlveoClient.py to the client FPGA and AlveoServer.py to server FPGA respectively.
* Step 2: run Xilinx runtime in two FPGAs.
```sh
source /opt/xilinx/xrt/setup.sh
```
* Step 3: run AlveoClient.py and AlveoServer.py respectively.
```sh
python AlveoClient.py
python AlveoServer.py
```
* Step 4: For benchmark rtt and throughput function. Put the python file into two FPGAs respectively, then run following command.
```sh
python Rtt_rx.py // Loop Back mode
python Rtt_tx.py // Measure latency mode
```
```sh
python Throuput_rx.py // CONSUMER packet
python Throuput_tx.py // PRODUCER packet
```

* Result
Rtt point-to-point about 1us,
Throughput point-to-point, can up to 90+ Gbps when ayload_size = 1472 bytes.



