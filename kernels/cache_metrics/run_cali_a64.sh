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
                   "FP_SP_SCALE_OPS_SPEC" \
                   "FP_SP_FIXED_OPS_SPEC" \
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
                   "EA_MEMORY" \
                   "EA_L2" \
                   "EA_CORE" \
                   )

t=48
b=256
module load use.own
module load caliper/a64-ompi3.1.6-gcc11.0.0c

export OMP_PROC_BIND=close
export OMP_PLACES=cores
export OMP_NUM_THREADS=${t}
export CALI_EVENT_ENABLE_SNAPSHOT_INFO=false

export CALI_SERVICES_ENABLE=trace,event,papi,report
export CALI_REPORT_CONFIG="SELECT * FORMAT json" 

# out_dir=a64_block_mm/block_mm_a64_scalar

for offset in {896..2112..32}
do

# out_dir=a64_cache_conflict/conflict_a64_gcc11_offset_${offset}
out_dir=a64_cache_conflict/conflict_a64_clang_offset_${offset}
mkdir -p ${out_dir}

for metric in "${metrics[@]}"
do 

  export CALI_PAPI_COUNTERS=${metric}
  export CALI_REPORT_FILENAME=${out_dir}/cali_${metric}.json
  ./cache_conflict -o ${offset}

done

done



