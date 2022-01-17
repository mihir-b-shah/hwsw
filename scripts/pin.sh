
set -e

EXE_PATH=$1
NUM_SKIP=$2
NUM_TRACE=$3
INPUT_FILE=$4

TRACE_NAME=$(basename ${EXE_PATH%.*})

INIT_OFFS_EXE=$(objdump -d $EXE_PATH            \
  | sed -nE 's/^([0-9a-f]+)\s+<main>:$/\1/p'    \
  | awk '{ print strtonum("0x" $1) }')

cd $CHAMPSIM_DIR/tracer && $CHAMPSIM_DIR/tracer/make_tracer.sh && cd -

INIT_OFFS_TRACE=$($PIN_ROOT/pin                           \
  -t $CHAMPSIM_DIR/tracer/obj-intel64/champsim_tracer.so  \
  -o $TRACE_NAME                                          \
  -s $NUM_SKIP -t $NUM_TRACE                              \
  -- $EXE_PATH $INPUT_FILE                                \
  | sed -nE 's/^\*\*\* MIHIR_ADDR:\s([0-9]+)\s*$/\1/p')

# put the offset in the file.
printf "%#x\n" $((INIT_OFFS_TRACE - INIT_OFFS_EXE))  \
  >$CHAMPSIM_DIR/info/$TRACE_NAME.offset

# compress the trace.
pv $TRACE_NAME | gzip >$CHAMPSIM_DIR/dpc3_traces/$TRACE_NAME.gz
rm $TRACE_NAME

