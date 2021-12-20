
EXE_PATH=$1
EXE_NAME=$(basename $EXE_PATH)

objdump -d --no-show-raw-insn $EXE_PATH                                       \
  | sed -nE 's/^\s*([0-9a-f]+):\s+(call|ret)\w*\s+.*$/\2 \1/p'                \
  | tee                                                                       \
      >(grep call | awk '{ print $2 }' >$CHAMPSIM_DIR/info/$EXE_NAME.calls)   \
      >(grep ret | awk '{ print $2 }' >$CHAMPSIM_DIR/info/$EXE_NAME.rets)     \
  >/dev/null
