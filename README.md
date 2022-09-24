# Net4FPGAs

### This project aims to achieve network communication between multi FPGAs. At current stage, two U250 are used for data transcation, one for tx, another for rx. Both U250 board are configured using interface 1. The project contains python version and C++ version.

## Generate XCLBIN

To implement any of the examples run:

```sh
make all DEVICE=<full platform path> INTERFACE=<interface number> DESIGN=<design name>
```

* `DEVICE` Alveo development target platform.
* `INTERFACE` defines which physical interface is going to be use. 0, 1 or 3 are supported. When `INTERFACE=3` the design will be replicated for each interface. Note that Alveo U50 only has one interface available (`INTERFACE=0`)
* `DESIGN` only support the following strings `basic` and `benchmark`. If you use something different, an error will be reported
* The basic configuration file is pulled from [config_files](config_files) folder and complete with `userPostSysLinkOverlayTcl` parameter before calling `v++`.
* [Ethernet/post_sys_link.tcl](Ethernet/post_sys_link.tcl) is automatically called from `v++` after system link. It is used to connect the GT capable pins to the cmac kernel(s)
* The `XCLBIN` will be generated in the folder \<DESIGN\>.intf_\<INTERFACE\>.\<(short)DEVICE\>

For Our platform, I use command below in hacchead machine:
```sh
make all DEVICE=/opt/xilinx/platforms/xilinx_u250_gen3x16_xdma_3_1_202020_1/xilinx_u250_gen3x16_xdma_3_1_202020_1.xpfm INTERFACE=1 DESIGN=basic
```

## Run Network basic function
* For python version, go to folder Example_py/
* For C++ version, go to folder Example_c/

## Reference
https://github.com/Xilinx/xup_vitis_network_example