// use https://www.edaplayground.com/x/9

module BitCounter_tb;

  // Parameters
  parameter BITSIZE = 10;
  parameter COUNTERSIZE = 4;

  // Inputs
  reg [BITSIZE-1:0] bitslice;

  // Outputs
  wire [COUNTERSIZE-1:0] count;

  // Instantiate the BitCounter module
  BitCounter #(.BITSIZE(BITSIZE), .COUNTERSIZE(COUNTERSIZE)) dut (
    .bitslice(bitslice),
    .count(count)
  );

  // Initialize inputs
  initial begin
    bitslice = 10'b1010101010; // Example input
  end

  // Display input and output values
  always @(count) begin
    $display("bitslice: %b", bitslice);
    $display("count: %b", count);
  end

  // Simulate for some time
  initial begin
    #10; // Give some time for initialization
    
    // Change the bitslice value
    bitslice = 10'b1100110011; // Example input

    // Add more test cases if needed

    $finish;
  end

endmodule

//# Allocation: Simulator allocated 4672 kB (elbread=427 elab2=4111 kernel=134 sdf=0)
//# KERNEL: ASDB file was created in location /home/runner/dataset.asdb
//# KERNEL: bitslice: 1010101010
//# KERNEL: count: 1000 error -> TO DO finetune design
//# RUNTIME: Info: RUNTIME_0068 testbench.sv (39): $finish called.
//# KERNEL: Time: 10 ns,  Iteration: 0,  Instance: /BitCounter_tb,  Process: @INITIAL#31_2@.
//# KERNEL: stopped at time: 10 ns