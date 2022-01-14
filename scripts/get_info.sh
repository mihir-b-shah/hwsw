
set -e

OPT=$LLVM_ROOT/bin/opt
LLC=$LLVM_ROOT/bin/llc

INFGEN_DIR=$CHAMPSIM_DIR/infgen

run_pass(){
  LIB_NAME=$1
  LIB_PATH=$INFGEN_DIR/build/lib/lib$LIB_NAME.so
  $OPT -load $LIB_PATH -$(echo $LIB_NAME | tr [:upper:] [:lower:]) -enable-new-pm=0 $2 -o $3 -S
}

IR_FILE=$CHAMPSIM_DIR/spec/$1
NAME_BASE=$(basename ${IR_FILE%.*})

DBG_IR_FILE=$CHAMPSIM_DIR/info/$NAME_BASE.ll

CURR_DIR=$(pwd)
cd $INFGEN_DIR/build/ && cmake -DLT_LLVM_INSTALL_DIR=$LLVM_ROOT .. && make && cd $CURR_DIR
run_pass AddDbg $IR_FILE $DBG_IR_FILE

OBJ_FILE=$NAME_BASE.o
EXE_FILE=$CHAMPSIM_DIR/info/$NAME_BASE

$LLC $DBG_IR_FILE -O0 -filetype=obj   \
  -relocation-model=pic               \
  --force-dwarf-frame-section         \
  --frame-pointer=all -o $OBJ_FILE    \
  --debug-entry-values

g++ -lm $OBJ_FILE -o $EXE_FILE

RESULTS_FILE=$CHAMPSIM_DIR/info/$NAME_BASE.crl
python3 $INFGEN_DIR/dwproc/main.py $EXE_FILE > $RESULTS_FILE

rm $OBJ_FILE
