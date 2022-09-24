// This kernel is used for create a loop_back function in FPGA user side.
// FPGA0 --> tx | ----> | rx --> FPGA1 -> loop_back --> tx | ----> | rx --> FPGA0 

// need to modify

#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define DWIDTH 512
#define TDWIDTH 16

typedef ap_axiu<DWIDTH, 1, 1, TDWIDTH> pkt;


extern "C" {
void krnl_loop(ap_uint<DWIDTH>  *Out,  // Write-Only Vector
               hls::stream<pkt> &k2n, // Internal Stream
               hls::stream<pkt> &n2k, // Internal Stream
               unsigned int     dest  // destination ID
               ) {
#pragma HLS INTERFACE m_axi port = Out offset = slave bundle = gmem
#pragma HLS INTERFACE axis port = k2n
#pragma HLS INTERFACE axis port = n2k
#pragma HLS INTERFACE s_axilite port = Out bundle = control
#pragma HLS INTERFACE s_axilite port = dest bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    pkt v;
    Out[0] = dest; // write a constant value.
    while (1) {
        n2k.read(v);
        v.dest = dest;
        v.keep = -1;
        k2n.write(v);
    }
}
}
