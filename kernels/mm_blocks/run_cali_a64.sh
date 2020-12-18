#!/bin/bash
#

declare -a _metrics=("PAPI_TOT_CYC")

declare -a metrics=(\
                   "PAPI_TOT_CYC" \
                   "INST_RETIRED" \
                   "INST_SPEC" \
                   "ASE_SPEC" \
                   "VFP_SPEC" \
                   "SVE_INST_RETIRED" \
                   "FP_DP_SCALE_OPS_SPEC" \
                   "FP_DP_FIXED_OPS_SPEC" \
                   "L1_PIPE_ABORT_STLD_INTLK" \
                   "LD_COMP_WAIT" \
                   "LDST_SPEC" \
                   "BR_COMP_WAIT" \
                   "FL_COMP_WAIT" \
                   "BR_MIS_PRED" \
                   "BR_PRED" \
                   "STALL_BACKEND" \
                   "STALL_FRONTEND" \
                   "BR_RETURN_SPEC" \
                   "BUS_WRITE_TOTAL_MEM" \
                   "BUS_READ_TOTAL_MEM" \
                   "BUS_WRITE_TOTAL_PCI" \
                   "BUS_READ_TOTAL_PCI" \
                   "BUS_WRITE_TOTAL_TOFU" \
                   "BUS_READ_TOTAL_TOFU" \
                   "LDREX_SPEC" \
                   "STREX_SPEC" \
                   "L2D_CACHE_REFILL" \
                   "L2D_CACHE_WB" \
                   "ASE_SVE_LD_SPEC" \
                   "ASE_SVE_ST_SPEC" \
                   "FP_LD_SPEC" \
                   "FP_ST_SPEC" \
                   "LD_SPEC" \
                   "ST_SPEC")


declare -a bsize=("4" "8" "16" "32" "64" "128" "256" "512")


t=48

# module load use.own
# module load caliper/a64-ompi3.1.6-gcc11.0.0c

export OMP_PROC_BIND=close
export OMP_PLACES=cores
export OMP_NUM_THREADS=48

export CALI_SERVICES_ENABLE=trace,event,papi,report

out_dir=a64_block_mm/block_mm_a64


for metric in "${metrics[@]}"
do   
for b in "${bsize[@]}"
do   
  mkdir -p ${out_dir}_b${b}_t${t}

  export CALI_REPORT_CONFIG="SELECT * FORMAT json" 
  export CALI_PAPI_COUNTERS=${metric}
  export CALI_REPORT_FILENAME=${out_dir}_b${b}_t${t}/cali_${metric}.json
  ./test_linear.out ${b} 

done
done



