* Step 1: create a new folder for project building in two FPGAs resepctively. // this step should be executed in headxacc machine, not FPGA board.
```sh
mkdir build && cd build && cmake .. && make
```
* Step 2: copy folder bin/ lib/ to the FPGA machine.
```sh
cp -r bin/ lib/ <FPGA machine>
```
* Step 3: run rx and tx in bin/ folder respectively.
```sh
./tx ../../../basic.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_basic_if1.xclbin 0 m.data // in Client FPGA
./rx ../../../basic.intf1.xilinx_u250_gen3x16_xdma_3_1_202020_1/vnx_basic_if1.xclbin 0 // in Server FPGA
```
Note that the size of m.data file in Client side must be multiple of 64 byte.

