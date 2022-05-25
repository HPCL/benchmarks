'''
run_STREAM.py

This script builds and run STREAM to match the different sizes of 
for x in {32..4096..64}; do python run_STREAM.py -c 48 $x -1 -1; done

Brian J Gravelle
gravelle@cs.uoregon.edu
'''


import os
import argparse
from math import floor

# caliper run file has two inputs. the size and the executable
# caliper_run_file % (64, "stream_c_64kib.exe")
caliper_run_file='''#!/bin/bash
#
declare -a _metrics=("PAPI_TOT_CYC")

declare -a metrics=(\\
                   "PAPI_TOT_CYC" \\
                   "CPU_CYCLES" \\
                   "INST_RETIRED" \\
                   "INST_SPEC" \\
                   "ASE_SPEC" \\
                   "FP_SPEC" \\
                   "VFP_SPEC" \\
                   "SVE_INST_RETIRED" \\
                   "0INST_COMMIT" \\
                   "1INST_COMMIT" \\
                   "2INST_COMMIT" \\
                   "3INST_COMMIT" \\
                   "4INST_COMMIT" \\
                   "FP_DP_SCALE_OPS_SPEC" \\
                   "FP_DP_FIXED_OPS_SPEC" \\
                   "L1_PIPE_ABORT_STLD_INTLK" \\
                   "LD_COMP_WAIT" \\
                   "LDST_SPEC" \\
                   "BR_COMP_WAIT" \\
                   "FL_COMP_WAIT" \\
                   "EU_COMP_WAIT" \\
                   "SINGLE_MOVPRFX_COMMIT" \\
                   "ROB_EMPTY" \\
                   "UOP_ONLY_COMMIT" \\
                   "BR_MIS_PRED" \\
                   "BR_PRED" \\
                   "STALL_BACKEND" \\
                   "STALL_FRONTEND" \\
                   "BR_RETURN_SPEC" \\
                   "BUS_WRITE_TOTAL_MEM" \\
                   "BUS_READ_TOTAL_MEM" \\
                   "BUS_WRITE_TOTAL_PCI" \\
                   "BUS_READ_TOTAL_PCI" \\
                   "BUS_WRITE_TOTAL_TOFU" \\
                   "BUS_READ_TOTAL_TOFU" \\
                   "LDREX_SPEC" \\
                   "STREX_SPEC" \\
                   "PAPI_L1_DCM" \\
                   "L1D_CACHE" \\
                   "PAPI_L2_DCM" \\
                   "PAPI_L2_DCH" \\
                   "L2D_CACHE_REFILL" \\
                   "L2D_CACHE_WB" \\
                   "L2D_CACHE" \\
                   "ASE_SVE_LD_SPEC" \\
                   "ASE_SVE_ST_SPEC" \\
                   "SVE_LDR_REG_SPEC" \\
                   "ASE_SVE_LD_MULTI_SPEC" \\
                   "SVE_LD_GATHER_SPEC" \\
                   "FP_LD_SPEC" \\
                   "FP_ST_SPEC" \\
                   "PRF_SPEC" \\
                   "SVE_PRF_CONTIG_SPEC" \\
                   "SVE_PRF_GATHER_SPEC" \\
                   "LD_SPEC" \\
                   "ST_SPEC" \\
                   )




out_dir=cali_stream_a64_%dkib

mkdir ${out_dir}

export OMP_PROC_BIND=close
export OMP_PLACES=cores

export CALI_SERVICES_ENABLE=trace,event,papi,report

for metric in "${metrics[@]}"
do    

  export CALI_REPORT_CONFIG="SELECT * FORMAT json" 
  export CALI_PAPI_COUNTERS=${metric}
 
  export CALI_LOG_VERBOSITY=1
 
  export CALI_REPORT_FILENAME=${out_dir}/cali_${metric}.json
  ./%s

done

'''




def main(cores, size_L1, size_L2, size_L3, cali):
	'''
	main function builds / runs each stream benchmark then prints the results
	size_L1 - the number of KiB per core in the L1 cache, 01 to skip that cache
	size_L2 - the number of KiB per core in the L1 cache, 01 to skip that cache
	size_L3 - the number of KiB per core in the L1 cache, 01 to skip that cache
	'''


	# improved cascade lake options
	# INC+=-DSTREAM_ARRAY_SIZE=49152     -DNTIMES=100 -DNTIMES_KK=685440 (size = 1179648)
	# INC+=-DSTREAM_ARRAY_SIZE=1048576   -DNTIMES=100 -DNTIMES_KK=32130 (size = 25165824)
	# INC+=-DSTREAM_ARRAY_SIZE=2064384   -DNTIMES=100 -DNTIMES_KK=16320 (size = 49545216)
	# INC+=-DSTREAM_ARRAY_SIZE=935854080 -DNTIMES=100 -DNTIMES_KK=36 (size = 22460497920)

	# a64fx options
	# INC+=-DSTREAM_ARRAY_SIZE=196608     -DNTIMES=10 -DNTIMES_KK=171360 (size = 4718592
	# INC+=-DSTREAM_ARRAY_SIZE=2097152    -DNTIMES=10 -DNTIMES_KK=16065 (size = 50331648)
	# INC+=-DSTREAM_ARRAY_SIZE=1871708160 -DNTIMES=10 -DNTIMES_KK=18 (size = 44920995840)


	print("Running Cache STREAM for Shingles")

	iter_ops = int(33690746880) # ops 
	ntimes = int(50)

	if(size_L1 > 0):
		run_stream(size_L1, iter_ops, ntimes, cores, cali)

	if(size_L2 > 0):
		run_stream(size_L2, iter_ops, ntimes, cores, cali)

	if(size_L3 > 0):
		run_stream(size_L3, iter_ops, ntimes, cores, cali)




def run_stream(size, iter_ops, ntimes, cores, cali=False):

	# 3 arrays, 8 bytes per element, 
	size_bytes  = size*1024
	array_len   = floor(size_bytes / (3.*8.))
	stream_size = array_len*cores
	ntimes_kk   = floor(iter_ops / array_len)
	exe = "%s%d%s" % ("stream_c_",size,"kib.exe")

	print("cache size  = %d" % size)
	print("num cores   = %d" % cores)
	print("planned ops = %d" % iter_ops)
	print("ntimes      = %d" % ntimes)
	print("array size  = %d" % array_len)
	print("total size  = %d" % stream_size)
	print("ntimes_kk   = %d" % ntimes_kk)
	print("total_ops   = %d" % (array_len*ntimes*ntimes_kk))
	print("executable  = %s" % exe)

	# build
	if cali:
		os.system("make SAS=%d NT=%d NKK=%d EXE=%s USE_CALI=1" % (stream_size,ntimes,ntimes_kk,exe))
		f = open('run_cali_a64_%dkib.sh'%size, 'w')
		f.write(caliper_run_file % (size, exe))
		f.close()
		os.system('chmod u+x run_cali_a64_%dkib.sh'%size)
	else:
		os.system("make SAS=%d NT=%d NKK=%d EXE=%s " % (stream_size,ntimes,ntimes_kk,exe))


	# run 

	# print results




if __name__ == '__main__':
	# Initialize parser
	parser = argparse.ArgumentParser()
	 
	# Adding optional argument
	parser.add_argument("-c","--cali", help="build executables with caliper", action="store_true")
	parser.add_argument("Cores", help="Number of logical cores to run on.", type=int)
	parser.add_argument("L1", help="size of L1 cache per core in KiB (-1 to skip)", type=int)
	parser.add_argument("L2", help="size of L2 cache per core in KiB (-1 to skip)", type=int)
	parser.add_argument("L3", help="size of L3 cache per core in KiB (-1 to skip)", type=int)
	 
	# Read arguments from command line
	args = parser.parse_args()

	main(args.Cores, args.L1, args.L2, args.L3, args.cali)



