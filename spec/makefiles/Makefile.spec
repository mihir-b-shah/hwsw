TUNE=base
LABEL=mihirbench-m64
NUMBER=520
NAME=omnetpp_r
SOURCES= simulator/fileoutscalarmgr.cc simulator/xmlgenerator.cc \
	 simulator/cdisplaystring.cc simulator/expression.tab.cc \
	 simulator/patternmatcher.cc simulator/neddtdvalidator.cc \
	 simulator/cmessage.cc simulator/cenvir.cc simulator/matchableobject.cc \
	 simulator/cstringpool.cc simulator/nedyylib.cc simulator/cgate.cc \
	 simulator/cconfiguration.cc simulator/cdynamicchanneltype.cc \
	 simulator/cxmlparimpl.cc simulator/nedexception.cc \
	 simulator/csimulation.cc simulator/msg2.tab.cc simulator/spec_qsort.cc \
	 simulator/nedelements.cc simulator/cdataratechannel.cc \
	 simulator/clcg32.cc simulator/envirbase.cc simulator/scenario.cc \
	 simulator/cstringtokenizer.cc simulator/appreg.cc \
	 simulator/nedsupport.cc simulator/cvisitor.cc simulator/valueiterator.cc \
	 simulator/cmodule.cc simulator/matchablefield.cc simulator/nederror.cc \
	 simulator/chasher.cc simulator/cwatch.cc simulator/cparimpl.cc \
	 simulator/globals.cc simulator/cstringparimpl.cc \
	 simulator/speedometer.cc simulator/fnamelisttokenizer.cc \
	 simulator/nedvalidator.cc simulator/cpacketqueue.cc \
	 simulator/ned1generator.cc simulator/nedparser.cc simulator/ned1.tab.cc \
	 simulator/ccompoundmodule.cc simulator/eventlogwriter.cc \
	 simulator/cexpressionbuilder.cc simulator/cconfigoption.cc \
	 simulator/cvarhist.cc simulator/objectprinter.cc simulator/main.cc \
	 simulator/lex.expryy.cc simulator/startup.cc simulator/nedutil.cc \
	 simulator/cmathfunction.cc simulator/eventlogfilemgr.cc \
	 simulator/nedtypeinfo.cc simulator/cdoubleparimpl.cc \
	 simulator/chistogram.cc simulator/akaroarng.cc simulator/cmessageheap.cc \
	 simulator/simtime.cc simulator/lex.msg2yy.cc \
	 simulator/cregistrationlist.cc simulator/cdetect.cc \
	 simulator/csimplemodule.cc simulator/expression.cc \
	 simulator/lcgrandom.cc simulator/inifilereader.cc simulator/errmsg.cc \
	 simulator/carray.cc simulator/runattributes.cc simulator/expr.tab.cc \
	 simulator/sectionbasedconfig.cc simulator/ccommbuffer.cc \
	 simulator/clongparimpl.cc simulator/cmsgpar.cc simulator/cnamedobject.cc \
	 simulator/cksplit.cc simulator/cxmlelement.cc simulator/ned2generator.cc \
	 simulator/cboolparimpl.cc simulator/cdelaychannel.cc simulator/cenum.cc \
	 simulator/onstartup.cc simulator/cfsm.cc simulator/lex.expressionyy.cc \
	 simulator/fsutils.cc simulator/nedfilebuffer.cc \
	 simulator/cdynamicmoduletype.cc simulator/displaystring.cc \
	 simulator/enumstr.cc simulator/cdensityestbase.cc simulator/ctopology.cc \
	 simulator/cnednetworkbuilder.cc simulator/fileglobber.cc \
	 simulator/cproperties.cc simulator/cexception.cc \
	 simulator/cxmldoccache.cc simulator/saxparser_none.cc \
	 simulator/cparsimcomm.cc simulator/stringutil.cc \
	 simulator/cownedobject.cc simulator/linetokenizer.cc \
	 simulator/nedelement.cc simulator/cneddeclaration.cc \
	 simulator/cdynamicexpression.cc simulator/cnedfunction.cc \
	 simulator/cproperty.cc simulator/lex.ned1yy.cc simulator/bigdecimal.cc \
	 simulator/ccomponent.cc simulator/clinkedlist.cc simulator/nedtools.cc \
	 simulator/opp_run.cc simulator/nedsaxhandler.cc \
	 simulator/matchexpression.tab.cc simulator/cnedloader.cc \
	 simulator/cpar.cc simulator/nedresourcecache.cc \
	 simulator/nedxmlparser.cc simulator/task.cc \
	 simulator/cclassdescriptor.cc simulator/minixpath.cc \
	 simulator/cstlwatch.cc simulator/coutvector.cc simulator/args.cc \
	 simulator/cqueue.cc simulator/ned2.tab.cc \
	 simulator/nedsyntaxvalidator.cc simulator/matchexpression.cc \
	 simulator/cclassfactory.cc simulator/exception.cc simulator/fileutil.cc \
	 simulator/distrib.cc simulator/cstddev.cc simulator/stringtokenizer.cc \
	 simulator/matchexpressionlexer.cc simulator/cstatistic.cc \
	 simulator/neddtdvalidatorbase.cc simulator/nedcrossvalidator.cc \
	 simulator/ccomponenttype.cc simulator/cobject.cc simulator/cchannel.cc \
	 simulator/akoutvectormgr.cc simulator/filesnapshotmgr.cc \
	 simulator/lex.ned2yy.cc simulator/ccoroutine.cc \
	 simulator/unitconversion.cc simulator/cdefaultlist.cc simulator/util.cc \
	 simulator/commonutil.cc simulator/cscheduler.cc \
	 simulator/nedfunctions.cc simulator/cmersennetwister.cc \
	 simulator/sim_std_m.cc simulator/filereader.cc simulator/stringpool.cc \
	 simulator/cmdenv.cc simulator/fileoutvectormgr.cc simulator/cpsquare.cc \
	 simulator/indexedfileoutvectormgr.cc simulator/intervals.cc \
	 model/MACRelayUnitBase.cc model/Ieee802Ctrl_m.cc model/EtherEncap.cc \
	 model/MACAddress.cc model/EtherLLC.cc model/EtherFrame_m.cc \
	 model/EtherMAC.cc model/MACRelayUnitNP.cc model/EtherApp_m.cc \
	 model/EtherMACBase.cc model/EtherAppSrv.cc model/MACRelayUnitPP.cc \
	 model/EtherHub.cc model/EtherAppCli.cc
EXEBASE=omnetpp_r
NEED_MATH=yes
BENCHLANG=CXX

BENCH_FLAGS      = -Isimulator/platdep -Isimulator -Imodel -DWITH_NETBUILDER -DSPEC_AUTO_SUPPRESS_OPENMP
CC               = $(LLVM_BIN_PATH)/clang
CC_VERSION_OPTION = -v
CLD              = $(LLVM_BIN_PATH)/llvm-link
COPTIMIZE        = -O3 -mavx
CXX              = $(LLVM_BIN_PATH)/clang++
CXXLD            = $(LLVM_BIN_PATH)/llvm-link
CXXOPTIMIZE      = -O3 -mavx -emit-llvm -fPIE
CXX_VERSION_OPTION = -v
EXTRA_FLIBS      = -lgfortran -lm
EXTRA_PORTABILITY = -DSPEC_LP64
FC_VERSION_OPTION = -v
FOPTIMIZE        = -O3 -mavx -funroll-loops
LDCFLAGS         =
LDOPTIMIZE       =
LLVM_BIN_PATH    = /usr/bin
LLVM_INCLUDE_PATH = /usr/include
OS               = unix
absolutely_no_locking = 0
abstol           = 1e-06
action           = validate
allow_label_override = 0
backup_config    = 1
baseexe          = omnetpp_r
basepeak         = 0
benchdir         = benchspec
benchmark        = 520.omnetpp_r
binary           = 
bindir           = exe
builddir         = build
bundleaction     = 
bundlename       = 
calctol          = 1
changedhash      = 0
check_version    = 0
clean_between_builds = no
command_add_redirect = 0
commanderrfile   = speccmds.err
commandexe       = omnetpp_r_base.mihirbench-m64
commandfile      = speccmds.cmd
commandoutfile   = speccmds.out
commandstdoutfile = speccmds.stdout
comparedir       = compare
compareerrfile   = compare.err
comparefile      = compare.cmd
compareoutfile   = compare.out
comparestdoutfile = compare.stdout
compile_error    = 0
compwhite        = 
configdir        = config
configfile       = mihir
configpath       = /u/mihirs/research/omnetpp/config/mihir.cfg
copies           = 4
current_range    = 
datadir          = data
default_size     = ref
default_submit   = $command
delay            = 0
deletebinaries   = 0
deletework       = 0
dependent_workloads = 0
device           = 
difflines        = 10
dirprot          = 511
discard_power_samples = 0
enable_monitor   = 1
endian           = 12345678
env_vars         = 0
expand_notes     = 0
expid            = 
exthash_bits     = 256
failflags        = 0
fake             = 1
feedback         = 1
flag_url_base    = https://www.spec.org/auto/cpu2017/Docs/benchmarks/flags/
floatcompare     = 
force_monitor    = 0
fw_bios          = 
hostname         = jalad
http_proxy       = 
http_timeout     = 30
hw_avail         = 
hw_cpu_max_mhz   = 
hw_cpu_name      = Intel Xeon E5-2603 v3
hw_cpu_nominal_mhz = 
hw_disk          = 6.0 TB  add more disk info here
hw_memory001     = 31.197 GB fixme: If using DDR3, format is:
hw_memory002     = 'N GB (M x N GB nRxn PCn-nnnnnR-n, ECC)'
hw_model         = 
hw_nchips        = 2
hw_ncores        = 
hw_ncpuorder     = 
hw_nthreadspercore = 
hw_ocache        = 
hw_other         = 
hw_pcache        = 
hw_scache        = 
hw_tcache        = 
hw_vendor        = My Corporation
idle_current_range = 
idledelay        = 10
idleduration     = 60
ignore_errors    = 1
ignore_sigint    = 0
ignorecase       = 
info_wrap_columns = 50
inputdir         = input
inputgenerrfile  = inputgen.err
inputgenfile     = inputgen.cmd
inputgenoutfile  = inputgen.out
inputgenstdoutfile = inputgen.stdout
iteration        = -1
iterations       = 1
keeptmp          = 0
label            = mihirbench-m64
license_num      = nnn (Your SPEC license number)
line_width       = 1020
link_input_files = 1
locking          = 1
log              = CPU2017
log_line_width   = 1020
log_timestamp    = 0
logname          = /u/mihirs/research/omnetpp/result/CPU2017.066.log
lognum           = 066
mail_reports     = all
mailcompress     = 0
mailmethod       = smtp
mailport         = 25
mailserver       = 127.0.0.1
mailto           = 
make             = specmake
make_no_clobber  = 0
makefile_template = Makefile.YYYtArGeTYYYspec
makeflags        = --jobs=8
max_average_uncertainty = 1
max_hum_limit    = 0
max_report_runs  = 3
max_unknown_uncertainty = 1
mean_anyway      = 1
meter_connect_timeout = 30
meter_errors_default = 5
meter_errors_percentage = 5
min_report_runs  = 2
min_temp_limit   = 20
minimize_builddirs = 0
minimize_rundirs = 0
name             = omnetpp_r
nansupport       = 
need_math        = yes
no_input_handler = close
no_monitor       = 
noratios         = 0
note_preenv      = 0
notes_plat_sysinfo_000 =  Sysinfo program /u/mihirs/research/omnetpp/bin/sysinfo
notes_plat_sysinfo_005 =  Rev: r5797 of 2017-06-14 96c45e4568ad54c135fd618bcc091c0f
notes_plat_sysinfo_010 =  running on jalad Fri Dec 17 00:16:14 2021
notes_plat_sysinfo_015 = 
notes_plat_sysinfo_020 =  SUT (System Under Test) info as seen by some common utilities.
notes_plat_sysinfo_025 =  For more information on this section, see
notes_plat_sysinfo_030 =     https://www.spec.org/cpu2017/Docs/config.html\#sysinfo
notes_plat_sysinfo_035 = 
notes_plat_sysinfo_040 =  From /proc/cpuinfo
notes_plat_sysinfo_045 =     model name : Intel(R) Xeon(R) CPU E5-2603 v3 @ 1.60GHz
notes_plat_sysinfo_050 =        2  "physical id"s (chips)
notes_plat_sysinfo_055 =        12 "processors"
notes_plat_sysinfo_060 =     cores, siblings (Caution: counting these is hw and system dependent. The following
notes_plat_sysinfo_065 =     excerpts from /proc/cpuinfo might not be reliable.  Use with caution.)
notes_plat_sysinfo_070 =        cpu cores : 6
notes_plat_sysinfo_075 =        siblings  : 6
notes_plat_sysinfo_080 =        physical 0: cores 0 1 2 3 4 5
notes_plat_sysinfo_085 =        physical 1: cores 0 1 2 3 4 5
notes_plat_sysinfo_090 = 
notes_plat_sysinfo_095 =  From lscpu:
notes_plat_sysinfo_100 =       Architecture:        x86_64
notes_plat_sysinfo_105 =       CPU op-mode(s):      32-bit, 64-bit
notes_plat_sysinfo_110 =       Byte Order:          Little Endian
notes_plat_sysinfo_115 =       CPU(s):              12
notes_plat_sysinfo_120 =       On-line CPU(s) list: 0-11
notes_plat_sysinfo_125 =       Thread(s) per core:  1
notes_plat_sysinfo_130 =       Core(s) per socket:  6
notes_plat_sysinfo_135 =       Socket(s):           2
notes_plat_sysinfo_140 =       NUMA node(s):        2
notes_plat_sysinfo_145 =       Vendor ID:           GenuineIntel
notes_plat_sysinfo_150 =       CPU family:          6
notes_plat_sysinfo_155 =       Model:               63
notes_plat_sysinfo_160 =       Model name:          Intel(R) Xeon(R) CPU E5-2603 v3 @ 1.60GHz
notes_plat_sysinfo_165 =       Stepping:            2
notes_plat_sysinfo_170 =       CPU MHz:             1198.585
notes_plat_sysinfo_175 =       CPU max MHz:         1600.0000
notes_plat_sysinfo_180 =       CPU min MHz:         1200.0000
notes_plat_sysinfo_185 =       BogoMIPS:            3196.08
notes_plat_sysinfo_190 =       Virtualization:      VT-x
notes_plat_sysinfo_195 =       L1d cache:           32K
notes_plat_sysinfo_200 =       L1i cache:           32K
notes_plat_sysinfo_205 =       L2 cache:            256K
notes_plat_sysinfo_210 =       L3 cache:            15360K
notes_plat_sysinfo_215 =       NUMA node0 CPU(s):   0,2,4,6,8,10
notes_plat_sysinfo_220 =       NUMA node1 CPU(s):   1,3,5,7,9,11
notes_plat_sysinfo_225 =       Flags:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov
notes_plat_sysinfo_230 =       pat pse36 clflush dts acpi mmx fxsr sse sse2 ss ht tm pbe syscall nx pdpe1gb rdtscp
notes_plat_sysinfo_235 =       lm constant_tsc arch_perfmon pebs bts rep_good nopl xtopology nonstop_tsc cpuid
notes_plat_sysinfo_240 =       aperfmperf pni pclmulqdq dtes64 monitor ds_cpl vmx smx est tm2 ssse3 sdbg fma cx16
notes_plat_sysinfo_245 =       xtpr pdcm pcid dca sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave
notes_plat_sysinfo_250 =       avx f16c rdrand lahf_lm abm cpuid_fault epb invpcid_single pti ssbd ibrs ibpb stibp
notes_plat_sysinfo_255 =       tpr_shadow vnmi flexpriority ept vpid fsgsbase tsc_adjust bmi1 avx2 smep bmi2 erms
notes_plat_sysinfo_260 =       invpcid cqm xsaveopt cqm_llc cqm_occup_llc dtherm arat pln pts md_clear flush_l1d
notes_plat_sysinfo_265 = 
notes_plat_sysinfo_270 =  /proc/cpuinfo cache data
notes_plat_sysinfo_275 =     cache size : 15360 KB
notes_plat_sysinfo_280 = 
notes_plat_sysinfo_285 =  From numactl --hardware  WARNING: a numactl 'node' might or might not correspond to a
notes_plat_sysinfo_290 =  physical chip.
notes_plat_sysinfo_295 = 
notes_plat_sysinfo_300 =  From /proc/meminfo
notes_plat_sysinfo_305 =     MemTotal:       32712524 kB
notes_plat_sysinfo_310 =     HugePages_Total:       0
notes_plat_sysinfo_315 =     Hugepagesize:       2048 kB
notes_plat_sysinfo_320 = 
notes_plat_sysinfo_325 =  /usr/bin/lsb_release -d
notes_plat_sysinfo_330 =     Ubuntu 18.04.5 LTS
notes_plat_sysinfo_335 = 
notes_plat_sysinfo_340 =  From /etc/*release* /etc/*version*
notes_plat_sysinfo_345 =     debian_version: buster/sid
notes_plat_sysinfo_350 =     os-release:
notes_plat_sysinfo_355 =        NAME="Ubuntu"
notes_plat_sysinfo_360 =        VERSION="18.04.5 LTS (Bionic Beaver)"
notes_plat_sysinfo_365 =        ID=ubuntu
notes_plat_sysinfo_370 =        ID_LIKE=debian
notes_plat_sysinfo_375 =        PRETTY_NAME="Ubuntu 18.04.5 LTS"
notes_plat_sysinfo_380 =        VERSION_ID="18.04"
notes_plat_sysinfo_385 =        HOME_URL="https://www.ubuntu.com/"
notes_plat_sysinfo_390 =        SUPPORT_URL="https://help.ubuntu.com/"
notes_plat_sysinfo_395 = 
notes_plat_sysinfo_400 =  uname -a:
notes_plat_sysinfo_405 =     Linux jalad 4.15.0-151-generic \#157-Ubuntu SMP Fri Jul 9 23:07:57 UTC 2021 x86_64
notes_plat_sysinfo_410 =     x86_64 x86_64 GNU/Linux
notes_plat_sysinfo_415 = 
notes_plat_sysinfo_420 =  run-level 5 2021-08-31 11:57
notes_plat_sysinfo_425 = 
notes_plat_sysinfo_430 =  SPEC is set to: /u/mihirs/research/omnetpp
notes_plat_sysinfo_435 =     Filesystem                    Type  Size  Used Avail Use% Mounted on
notes_plat_sysinfo_440 =     filer5b:/vol38/v38q001/mihirs nfs   6.0T  5.0T  959G  85% /u/mihirs
notes_plat_sysinfo_445 = 
notes_plat_sysinfo_450 =  Additional information from dmidecode follows.  WARNING: Use caution when you interpret
notes_plat_sysinfo_455 =  this section. The 'dmidecode' program reads system data which is "intended to allow
notes_plat_sysinfo_460 =  hardware to be accurately determined", but the intent may not be met, as there are
notes_plat_sysinfo_465 =  frequent changes to hardware, firmware, and the "DMTF SMBIOS" standard.
notes_plat_sysinfo_470 = 
notes_plat_sysinfo_475 =  (End of data from sysinfo program)
notes_wrap_columns = 0
notes_wrap_indent =   
num              = 520
obiwan           = 
os_exe_ext       = 
output_format    = txt,html,cfg,pdf,csv
output_root      = 
outputdir        = output
parallel_test    = 4
parallel_test_submit = 0
parallel_test_workloads = 
path             = /u/mihirs/research/omnetpp/benchspec/CPU/520.omnetpp_r
plain_train      = 1
platform         = 
power            = 0
preenv           = 1
prefix           = 
prepared_by      = mihirs  (is never output, only tags rawfile)
ranks            = 1
rawhash_bits     = 256
rebuild          = 1
reftime          = reftime
reltol           = 1e-05
reportable       = 0
resultdir        = result
review           = 0
run              = all
runcpu           = /u/mihirs/research/omnetpp/bin/harness/runcpu --fake --loose --size test --tune base --config mihir 520.omnetpp_r
rundir           = run
runmode          = rate
safe_eval        = 1
save_build_files = 
section_specifier_fatal = 1
setprocgroup     = 1
setup_error      = 0
sigint           = 2
size             = test
size_class       = test
skipabstol       = 
skipobiwan       = 
skipreltol       = 
skiptol          = 
smarttune        = base
specdiff         = specdiff
specrun          = specinvoke
srcalt           = 
srcdir           = src
srcsource        = /u/mihirs/research/omnetpp/benchspec/CPU/520.omnetpp_r/src
stagger          = 10
strict_rundir_verify = 1
submit_default   = echo "$command" > run.sh ; $BIND bash run.sh
sw_avail         = 
sw_base_ptrsize  = 64-bit
sw_compiler001   = C/C++: Version 3.9.0 of Clang, the
sw_compiler002   = LLVM Compiler Infrastructure
sw_compiler003   = Fortran: Version 4.8.2 of GCC, the
sw_compiler004   = GNU Compiler Collection
sw_compiler005   = DragonEgg: Version 3.5.2, the
sw_compiler006   = LLVM Compiler Infrastructure
sw_file          = nfs
sw_os001         = Ubuntu 18.04.5 LTS
sw_os002         = 4.15.0-151-generic
sw_other         = 
sw_peak_ptrsize  = 64-bit
sw_state         = Run level 5 (add definition here)
sysinfo_hash_bits = 256
sysinfo_program  = specperl /u/mihirs/research/omnetpp/bin/sysinfo
sysinfo_program_hash = e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855
table            = 1
teeout           = 1
test_date        = Dec-2021
test_sponsor     = My Corporation
tester           = My Corporation
threads          = 1
top              = /u/mihirs/research/omnetpp
train_single_thread = 0
train_with       = train
tune             = base
uid              = 40202
unbuffer         = 1
uncertainty_exception = 5
update           = 0
update_url       = http://www.spec.org/auto/cpu2017/updates/
use_submit_for_compare = 0
use_submit_for_speed = 0
username         = mihirs
verbose          = 5
verify_binaries  = 1
version          = 0.905000
version_url      = http://www.spec.org/auto/cpu2017/devel_version
voltage_range    = 
worklist         = list
OUTPUT_RMFILES   = General-0.sca
