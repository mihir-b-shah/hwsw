
# tail the output of pin.sh to get the offset.
# by design, so you can see the errors on stderr and offset on stdout

set -e

EXE_NAME=$1
TRACE_NAME=$2
NUM_SKIP=$3
NUM_TRACE=$4
INPUT_FILE=$5

INIT_OFFS_EXE=$(objdump -d $EXE_NAME            \
  | sed -nE 's/^([0-9a-f]+)\s+<_init>:$/\1/p'   \
  | awk '{ printf("%d\n", "0x" $1) }')

INIT_OFFS_TRACE=$(cat $INPUT_FILE                         \
  | $PIN_ROOT/pin                                         \
  -t $CHAMPSIM_DIR/tracer/obj-intel64/champsim_tracer.so  \
  -o $TRACE_NAME                                          \
  -s $NUM_SKIP -t $NUM_TRACE                              \
  -- $EXE_NAME                                            \
  | sed -nE 's/^\*\*\* MIHIR_ADDR:\s([0-9]+)\s*$/\1/p')

echo "Finished tracing."

# put the offset in the file.
printf "%#x\n" $((INIT_OFFS_TRACE - INIT_OFFS_EXE))  \
  >$CHAMPSIM_DIR/info/$TRACE_NAME.offset

# compress the trace.
pv $TRACE_NAME | gzip >$CHAMPSIM_DIR/dpc3_traces/$TRACE_NAME.gz

