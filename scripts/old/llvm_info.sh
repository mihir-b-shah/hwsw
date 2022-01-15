
set -e

TRACE_NAME=$1
LLVM_BIN_PATH=/usr/lib/llvm-13/bin
MAPPER_DIR=$CHAMPSIM_DIR/scripts/mapper

BC_PATH=$CHAMPSIM_DIR/spec/$TRACE_NAME.bc
EXE_PATH=$CHAMPSIM_DIR/spec/$TRACE_NAME

MIR_PATH="$CHAMPSIM_DIR/info/$TRACE_NAME.mir"
TMP_MIR_FILE="tmp_mir"

$LLVM_BIN_PATH/llc --stop-after=x86-codegen -O2 $BC_PATH -o $MIR_PATH
python3 $MAPPER_DIR/fix_mir.py < $MIR_PATH > $TMP_MIR_FILE
mv $TMP_MIR_FILE $MIR_PATH

TMP_OBJDUMP_FILE="tmp_objdump"
objdump -d --prefix-addresses $EXE_PATH                                           \
  | sed -nE 's/^([0-9a-f]+)\s+<(\w+)(\+0x[0-9a-f]+){0,1}>\s+(\w+).*$/\1 \2 \4/p'  \
  > $TMP_OBJDUMP_FILE

ADDR_INSTR_FILE=$CHAMPSIM_DIR/info/$TRACE_NAME.ai
make -C $MAPPER_DIR
$MAPPER_DIR/mfnum $MIR_PATH $TMP_OBJDUMP_FILE &> $ADDR_INSTR_FILE

rm $TMP_OBJDUMP_FILE
