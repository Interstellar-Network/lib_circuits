`timescale 1ns/1ns

module my_aes128_coprocessor_tb;
  // Declare signals
  reg [127:0] ptext;
  reg [127:0] key;
  reg [127:0] ctext_aes_expected;
  wire [127:0] ctext_aes_actual;

  // Instantiate AES coprocessor module
  my_aes128_coprocessor dut (
    .ptext(ptext),
    .key(key),
    .ctext_aes(ctext_aes_actual)
  );

  // Initialize input vectors
  initial begin
    // Plaintext input
    ptext = 128'h3243f6a8885a308d313198a2e0370734;
    $display("Plaintext: %h", ptext);

    // Cipher key
    key = 128'h2b7e151628aed2a6abf7158809cf4f3c;

    // Expected ciphertext result
    ctext_aes_expected = 128'h3925841d02dc09fbdc118597196a0b32;

    // Apply a reset signal if necessary

    // Apply inputs and simulate
    #1;  // Adjust the delay if necessary
    // Continue your simulation here

    // Compare actual output and expected output
    if (ctext_aes_actual == ctext_aes_expected)
      $display("Test Passed: Ciphertext is correct");
    else
      $display("Test Failed: Ciphertext is incorrect");

    $display("Ciphertext: %h", ctext_aes_actual);

     // To use EPWave
    $dumpfile("dump.vcd"); $dumpvars;

    // End the simulation
    $finish;
  end
endmodule

/* Result on https://www.edaplayground.com/
[2023-08-12 10:33:21 UTC] vlib work && vlog '-timescale' '1ns/1ns' +incdir+$RIVIERA_HOME/vlib/uvm-1.2/src -l uvm_1_2 -err VCP2947 W9 -err VCP2974 W9 -err VCP3003 W9 -err VCP5417 W9 -err VCP6120 W9 -err VCP7862 W9 -err VCP2129 W9 design.sv testbench.sv  && vsim -c -do "vsim +access+r ; run -all; exit"  
VSIMSA: Configuration file changed: `/home/runner/library.cfg'
ALIB: Library "work" attached.
work = /home/runner/work/work.lib
MESSAGE "Pass 1. Scanning modules hierarchy."
MESSAGE "Pass 2. Processing instantiations."
MESSAGE "Pass 3. Processing behavioral statements."
MESSAGE "Running Optimizer."
MESSAGE "ELB/DAG code generating."
MESSAGE "Unit top modules: my_aes128_coprocessor_tb."
MESSAGE "$root top modules: my_aes128_coprocessor_tb."
SUCCESS "Compile success 0 Errors 0 Warnings  Analysis time: 0[s]."
ALOG: Warning: The source is compiled without the -dbg switch. Line breakpoints and assertion debug will not be available.
done
# Aldec, Inc. Riviera-PRO version 2022.04.117.8517 built for Linux64 on May 04, 2022.
# HDL, SystemC, and Assertions simulator, debugger, and design environment.
# (c) 1999-2022 Aldec, Inc. All rights reserved.
# ELBREAD: Elaboration process.
# ELBREAD: Elaboration time 0.0 [s].
# KERNEL: Main thread initiated.
# KERNEL: Kernel process initialization phase.
# ELAB2: Elaboration final pass...
# ELAB2: Create instances ...
# KERNEL: Time resolution set to 1ns.
# ELAB2: Create instances complete.
# SLP: Started
# SLP: Elaboration phase ...
# SLP: Elaboration phase ... done : 0.1 [s]
# SLP: Generation phase ...
# SLP: Generation phase ... done : 0.3 [s]
# SLP: Finished : 0.4 [s]
# SLP: 14152 (72.85%) primitives and 5274 (27.15%) other processes in SLP
# SLP: 20345 (100.00%) signals in SLP and 0 interface signals
# ELAB2: Elaboration final pass complete - time: 0.5 [s].
# KERNEL: SLP loading done - time: 0.0 [s].
# KERNEL: Warning: You are using the Riviera-PRO EDU Edition. The performance of simulation is reduced.
# KERNEL: Warning: Contact Aldec for available upgrade options - sales@aldec.com.
# KERNEL: SLP simulation initialization done - time: 0.0 [s].
# KERNEL: Kernel process initialization done.
# Allocation: Simulator allocated 5279 kB (elbread=660 elab2=4484 kernel=134 sdf=0)
# KERNEL: ASDB file was created in location /home/runner/dataset.asdb
# KERNEL: Plaintext: 3243f6a8885a308d313198a2e0370734
# KERNEL: Test Passed: Ciphertext is correct
# KERNEL: Ciphertext: 3925841d02dc09fbdc118597196a0b32
# RUNTIME: Info: RUNTIME_0068 testbench.sv (44): $finish called.
# KERNEL: Time: 1 ns,  Iteration: 0,  Instance: /my_aes128_coprocessor_tb,  Process: @INITIAL#18_0@.
# KERNEL: stopped at time: 1 ns
# VSIM: Simulation has finished. There are no more test vectors to simulate.
# VSIM: Simulation has finished.
