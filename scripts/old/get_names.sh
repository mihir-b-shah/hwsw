
EXE_PATH=$1
EXE_NAME=$(basename $EXE_PATH)

objdump -d $EXE_PATH                              \
  | sed -nE 's/^([0-9a-f]+)\s+<(\S+)>:$/\1 \2/p'  \
  >$CHAMPSIM_DIR/info/$EXE_NAME.names
