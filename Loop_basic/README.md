## This folder aims to provide a basic+loop function using Opencl.

* Step 1: create a new folder for project building in two FPGAs resepctively. // this step should be executed in headxacc machine, not FPGA board.
```sh
mkdir build && cd build && cmake .. && make
```
* Step 2: copy folder bin/ lib/ to the FPGA machine.
```sh
cp -r bin/ lib/ <FPGA machine>
```
* Step 3: run basic and loop in bin/ folder respectively.
```sh
./basic ../../../basic.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_basic_if1.xclbin 0 // in Server FPGA
./loop ../../../loop.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_loop_if1.xclbin 0 // in Client FPGA
```
Note that when running this code the first time, it may lose the first data packet. Just add a initial script to check the datapath is ok.
For example, I usually run Rtt_tx.py and Rtt_rx.py to check whether the datapath is OK.