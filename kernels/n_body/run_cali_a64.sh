#!/bin/bash
#

declare -a _metrics=("CPU_CYCLES")

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
                   "SVE_LDR_REG_SPEC" \
                   "ASE_SVE_LD_MULTI_SPEC" \
                   "SVE_LD_GATHER_SPEC" \
                   "FP_LD_SPEC" \
                   "FP_ST_SPEC" \
                   "PRF_SPEC" \
                   "SVE_PRF_CONTIG_SPEC" \
                   "SVE_PRF_GATHER_SPEC" \
                   "LD_SPEC" \
                   "ST_SPEC" \
                   )


declare -a particles=(131072)

export OMP_PROC_BIND=spread
export OMP_PLACES=cores
export OMP_NUM_THREADS=48
export CALI_EVENT_ENABLE_SNAPSHOT_INFO=false

export CALI_SERVICES_ENABLE=trace,event,papi,report
export CALI_REPORT_CONFIG="SELECT * FORMAT json" 


export CALI_SERVICES_ENABLE=trace,event,papi,report

out_dir=cali_n_body

for metric in "${metrics[@]}"
do   
for p in "${particles[@]}"
do 

  export CALI_PAPI_COUNTERS=${metric}
  export CALI_REPORT_FILENAME=${out_dir}/n_body_${p}/cali_${metric}.json
  ./n_body -n ${p} -t 48

done
done


