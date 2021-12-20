
set -e

NUM_MIL_ITERS=$1
TRACE_NAME=$2

$CHAMPSIM_DIR/build_champsim.sh bimodal no no no sisb lru 1 &&  \
$CHAMPSIM_DIR/run_champsim.sh bimodal-no-no-no-sisb-lru-1core   \
  1 $NUM_MIL_ITERS $TRACE_NAME.gz
