# cf lib_server/src/circuits_builder/verilog_compiler.cpp

YOSYS="./build/_deps/yosys_dl-src/yosys"
YOSYS_MAIN_CMDS="techmap"

echo $PWD/data/verilog/xorexpand.v

$YOSYS -s $PWD/data/blif/yosys_cmd_verilog_compiler_cpp
# below DOES NOT work; .blif is empty(or rather improper)
# $YOSYS -o $PWD/data/blif/rndswitch.blif -p $YOSYS_MAIN_CMDS $PWD/data/blif/define.v $PWD/data/verilog/rndswitch.v
# $YOSYS -o $PWD/data/blif/xorexpand.blif -p $YOSYS_MAIN_CMDS $PWD/data/blif/define.v $PWD/data/verilog/xorexpand.v
# $YOSYS -o $PWD/data/blif/display-main.blif -p $YOSYS_MAIN_CMDS $PWD/data/blif/define.v $PWD/data/verilog/display-main.v
# $YOSYS tcl $PWD/data/blif/yosys_cmd.tcl $PWD/data