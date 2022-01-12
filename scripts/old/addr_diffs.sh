
# Takes a trace and the corresponding executable, and produces the offset in the trace.

TRACE=$1
EXE=$2

if [ ! -f $OFFS_FIND ]; then
  gcc $OFFS_FIND.c -o $OFFS_FIND
fi

pv $1 | od -vxL -w8 | awk '(NR-2) % 16 == 0' | sed -nE 's/\s+//p' | uniq
