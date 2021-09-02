#!/bin/bash
#

declare -a _metrics=("PAPI_TOT_CYC")

declare -a metrics=(\
                   "PAPI_TOT_CYC" \
                   "CPU_CYCLES" \
                   "INST_RETIRED" \
                   "INST_SPEC" \
                   "ASE_SPEC" \
                   "FP_SPEC" \
                   "VFP_SPEC" \
                   "SVE_INST_RETIRED" \
                   "0INST_COMMIT" \
                   "1INST_COMMIT" \
                   "2INST_COMMIT" \
                   "3INST_COMMIT" \
                   "4INST_COMMIT" \
                   "FP_DP_SCALE_OPS_SPEC" \
                   "FP_DP_FIXED_OPS_SPEC" \
                   "L1_PIPE_ABORT_STLD_INTLK" \
                   "LD_COMP_WAIT" \
                   "LDST_SPEC" \
                   "BR_COMP_WAIT" \
                   "FL_COMP_WAIT" \
                   "EU_COMP_WAIT" \
                   "SINGLE_MOVPRFX_COMMIT" \
                   "ROB_EMPTY" \
                   "UOP_ONLY_COMMIT"\
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
                   "PAPI_L1_DCM" \
                   "L1D_CACHE" \
                   "PAPI_L2_DCM" \
                   "PAPI_L2_DCH" \
                   "L2D_CACHE_REFILL" \
                   "L2D_CACHE_WB" \
                   "L2D_CACHE" \
                   "ASE_SVE_LD_SPEC" \
                   "ASE_SVE_ST_SPEC" \
                   "FP_LD_SPEC" \
                   "FP_ST_SPEC" \
                   "LD_SPEC" \
                   "ST_SPEC" \
                   )

t=48
t=1
b=256
# module load use.own
# module load caliper/a64-ompi3.1.6-gcc11.0.0c

export OMP_PROC_BIND=close
export OMP_PLACES=cores
export OMP_NUM_THREADS=${t}
export CALI_EVENT_ENABLE_SNAPSHOT_INFO=false

export CALI_SERVICES_ENABLE=trace,event,papi,report
export CALI_REPORT_CONFIG="SELECT * FORMAT json"
export CALI_LOG_VERBOSITY=1 

# out_dir=a64_block_mm/block_mm_a64_scalar
out_dir=cali_mm_a64/cali_mm_block


for metric in "${metrics[@]}"
do 
  mkdir -p ${out_dir}

  export CALI_PAPI_COUNTERS=${metric}
  export CALI_REPORT_FILENAME=${out_dir}/cali_${metric}.json
  ./test_linear.out

done