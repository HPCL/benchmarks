#!/bin/bash
#

#declare -a metrics=("PAPI_TOT_CYC")

declare -a metrics=(\
#                    "PAPI_TOT_CYC" \
#                    "PAPI_STL_ICY" \
#                    "DISPATCH_RESOURCE_STALL_CYCLES_1:INT_PHY_REG_FILE_RSRC_STALL" \
#                    "DISPATCH_RESOURCE_STALL_CYCLES_1:LOAD_QUEUE_RSRC_STALL" \
#                    "DISPATCH_RESOURCE_STALL_CYCLES_1:STORE_QUEUE_RSRC_STALL" \
#                    "DISPATCH_RESOURCE_STALL_CYCLES_1:INT_SCHEDULER_MISC_RSRC_STALL" \
#                    "DISPATCH_RESOURCE_STALL_CYCLES_1:TAKEN_BRANCH_BUFFER_RSRC_STALL" \
#                    "DISPATCH_RESOURCE_STALL_CYCLES_1:FP_REG_FILE_RSRC_STALL" \
#                    "DISPATCH_RESOURCE_STALL_CYCLES_1:FP_SCHEDULER_FILE_RSRC_STALL" \
#                    "DISPATCH_RESOURCE_STALL_CYCLES_1:FP_MISC_FILE_RSRC_STALL" \
#                    "DISPATCH_RESOURCE_STALL_CYCLES_0:ALU_TOKEN_STALL" \
#                    "RETIRED_UOPS" \
#                    "RETIRED_INSTRUCTIONS" \
#                    "STALLED-CYCLES-BACKEND" \
#                    "STALLED-CYCLES-FRONTEND" \
#                    "FPU_PIPE_ASSIGNMENT:TOTAL0" \
#                    "FPU_PIPE_ASSIGNMENT:TOTAL1" \
#                    "FPU_PIPE_ASSIGNMENT:TOTAL2" \
#                    "FPU_PIPE_ASSIGNMENT:TOTAL3" \
#                    "RETIRED_MMX_FP_INSTRUCTIONS" \
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
#                    "RETIRED_BRANCH_INSTRUCTIONS" \
#                    "RETIRED_FAR_CONTROL_TRANSFERS" \
#                    "RETIRED_INDIRECT_BRANCH_INSTRUCTIONS_MISPREDICTED" \
#                    "RETIRED_BRANCH_INSTRUCTIONS_MISPREDICTED" \
#                    "RETIRED_TAKEN_BRANCH_INSTRUCTIONS" \
#                    "RETIRED_TAKEN_BRANCH_INSTRUCTIONS_MISPREDICTED" \
#                    "RETIRED_CONDITIONAL_BRANCH_INSTRUCTIONS" \
#                    "RETIRED_FUSED_INSTRUCTIONS" \
#                    "RETIRED_NEAR_RETURNS" \
#                    "RETIRED_NEAR_RETURNS_MISPREDICTED" \
#                    "RETIRED_LOCK_INSTRUCTIONS:CACHEABLE_LOCKS" \
#                    "RETIRED_CLFLUSH_INSTRUCTIONS" \
#                    "RETIRED_CPUID_INSTRUCTIONS" \
#                    "RETIRED_SERIALIZING_OPS:X87_CTRL_RET" \
#                    "RETIRED_SERIALIZING_OPS:X87_BOT_RET" \
#                    "RETIRED_SERIALIZING_OPS:SSE_CTRL_RET" \
#                    "RETIRED_SERIALIZING_OPS:SSE_BOT_RET" \
#                    "L1-DCACHE-LOADS" \
#                    "LS_DISPATCH:LD_ST_DISPATCH" \
#                    "FP_SCHEDULER_EMPTY" \
#                    "CYCLES_WITH_FILL_PENDING_FROM_L2" \
#                    "DIV_CYCLES_BUSY_COUNT" \
#                    "DIV_OP_COUNT" \
#                    "INSTRUCTION_PIPE_STALL:IC_STALL_DQ_EMPTY" \
#                    "UOPS_DISPATCHED_FROM_DECODER:DECODER_DISPATCHED" \
                    "r00AA" \
#                    "UOPS_QUEUE_EMPTY" \
#                    "RETIRED_LOCK_INSTRUCTIONS" \
#                    "PERF_COUNT_HW_STALLED_CYCLES_BACKEND" \
                   )

module load caliper/rome-ompi3.1.4-gcc9.2

#out_dir=cali_mm_5000_avx2_base
out_dir=cali_mm_5000
mkdir ${out_dir}_base
mkdir ${out_dir}_transpose

export OMP_PROC_BIND=close
export OMP_PLACES=cores

export OMPI_MCA_pml=^ucx
export OMPI_MCA_osc=^ucx

export CALI_SERVICES_ENABLE=trace,event,papi,mpi,mpireport

for metric in "${metrics[@]}"
do    
  export CALI_MPIREPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested,mpi.rank FORMAT json"
  export CALI_PAPI_COUNTERS=${metric}
  
  export CALI_MPIREPORT_FILENAME=${out_dir}_base/cali_${metric}.json
  mpirun -n 40 ./mm_mpi

  export CALI_MPIREPORT_FILENAME=${out_dir}_transpose/cali_${metric}.json
  mpirun -n 40 ./mm_mpi -t

done



