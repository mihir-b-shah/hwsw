
set -e

BENCH_NAME="$1"
EXE_NAME="${BENCH_NAME:4}"
MDIR=$(pwd)

cd $SPEC/benchspec/CPU/$BENCH_NAME/build/build_base_mihirbench-m64.0000/
$SPEC/bin/specmake clean
$SPEC/bin/specmake
cp $EXE_NAME $SPEC/$EXE_NAME.bc
cd $MDIR
