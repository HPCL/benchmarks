#!/bin/bash
#

#declare -a metrics=("PAPI_TOT_CYC")

declare -a metrics=(\
#                    "PAPI_TOT_CYC" \
#                    "RETIRED_BRANCH_INSTRUCTIONS_MISPREDICTED" \
#                    "RETIRED_NEAR_RETURNS_MISPREDICTED" \
#                    "PAPI_STL_ICY" \
#                    "DYNAMIC_TOKENS_DISPATCH_STALLS_CYCLES_0:RETIRE_TOKEN_STALL" \
#                    "DYNAMIC_TOKENS_DISPATCH_STALLS_CYCLES_0:AGSQ_TOKEN_STALL" \
#                    "DYNAMIC_TOKENS_DISPATCH_STALLS_CYCLES_0:ALU_TOKEN_STALL" \
#                    "DYNAMIC_TOKENS_DISPATCH_STALLS_CYCLES_0:ALSQ3_0_TOKEN_STALL" \
#                    "DYNAMIC_TOKENS_DISPATCH_STALLS_CYCLES_0:ALSQ3_TOKEN_STALL" \
#                    "DYNAMIC_TOKENS_DISPATCH_STALLS_CYCLES_0:ALSQ2_TOKEN_STALL" \
#                    "DYNAMIC_TOKENS_DISPATCH_STALLS_CYCLES_0:ALSQ1_TOKEN_STALL" \
#                    "RETIRED_UOPS" \
#                    "RETIRED_INSTRUCTIONS" \
#                    "STALLED-CYCLES-BACKEND" \
#                    "FPU_PIPE_ASSIGNMENT:TOTAL0" \
#                    "FPU_PIPE_ASSIGNMENT:TOTAL1" \
#                    "FPU_PIPE_ASSIGNMENT:TOTAL2" \
#                    "FPU_PIPE_ASSIGNMENT:TOTAL3" \
#                    "RETIRED_MMX_FP_INSTRUCTIONS:SSE_INSTR" \
#                    "RETIRED_MMX_FP_INSTRUCTIONS:X87_INSTR" \
#                    "RETIRED_MMX_FP_INSTRUCTIONS:MMX_INSTR" \
#                    "RETIRED_SSE_AVX_OPERATIONS:DP_MULT_FLOPS" \
#                    "RETIRED_SSE_AVX_OPERATIONS:DP_DIV_FLOPS" \
#                    "RETIRED_SSE_AVX_OPERATIONS:DP_MULT_ADD_FLOPS" \
#                    "RETIRED_SSE_AVX_OPERATIONS:DP_ADD_SUB_FLOPS" \
#                    "RETIRED_SSE_AVX_OPERATIONS:SP_MULT_FLOPS" \
#                    "RETIRED_SSE_AVX_OPERATIONS:SP_DIV_FLOPS" \
#                    "RETIRED_SSE_AVX_OPERATIONS:SP_MULT_ADD_FLOPS" \
#                    "RETIRED_SSE_AVX_OPERATIONS:SP_ADD_SUB_FLOPS" \
#                    "LS_DISPATCH:LD_ST_DISPATCH" \
#                    "FP_SCHEDULER_EMPTY" \
#                    "CYCLES_WITH_FILL_PENDING_FROM_L2" \
#                    "DIV_CYCLES_BUSY_COUNT" \
#                    "DIV_OP_COUNT" \
                    "INSTRUCTION_PIPE_STALL:IC_STALL_DQ_EMPTY"\
                   )

module load caliper/amd-ompi3.1.4-gcc9.2

#out_dir=cali_mm_5000_avx2_base
out_dir=cali_mm_5000_base
mkdir ${out_dir}

export OMP_PROC_BIND=close
export OMP_PLACES=cores

export OMPI_MCA_pml=^ucx
export OMPI_MCA_osc=^ucx

export CALI_SERVICES_ENABLE=trace,event,papi,mpi,mpireport

for metric in "${metrics[@]}"
do    
  export CALI_MPIREPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested,mpi.rank FORMAT json"
  export CALI_PAPI_COUNTERS=${metric}
  
  export CALI_MPIREPORT_FILENAME=${out_dir}/cali_${metric}.json
  mpirun -n 40 ./mm_mpi

done


#out_dir=cali_mm_5000_avx2_transpose
out_dir=cali_mm_5000_transpose
mkdir ${out_dir}

for metric in "${metrics[@]}"
do    
  export CALI_MPIREPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested,mpi.rank FORMAT json"
  export CALI_PAPI_COUNTERS=${metric}
  
  export CALI_MPIREPORT_FILENAME=${out_dir}/cali_${metric}.json
  mpirun -n 40 ./mm_mpi -t

done


