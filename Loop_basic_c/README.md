## This folder aims to provide basic and loop functions using Opencl.

* Step 1: create a new folder for project building in two FPGAs resepctively. // this step should be executed in headxacc machine, not FPGA board.
```sh
mkdir build && cd build && cmake .. && make
```
* Step 2: copy folder bin/ lib/ to the FPGA machine.
```sh
cp -r bin/ lib/ <FPGA machine>
```
* Step 3: run (loop kernel) in bin/ folder respectively.
```sh
./round ../../../basic.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_basic_if1.xclbin 1 2 // in FPGA 1, for 1-->2
./loop ../../../loop.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_loop_if1.xclbin 2 1 // in FPGA 2, for 2-->1
```
* Step 3: run (basic kernel) in bin/ folder respectively.
```sh
./tx ../../../basic.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_basic_if1.xclbin 1 2 // in FPGA 1, FPGA 1 works as a Client.
./rx ../../../basic.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_loop_if1.xclbin 2 1 // in FPGA 2, FPGA 2 works as a Server.
```