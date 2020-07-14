#!/bin/bash
#
#SBATCH --job-name=stencil_cali
#SBATCH --output=test_cali.txt
#SBATCH --time=8:00:00



declare -a metrics=("PAPI_TOT_CYC")

declare -a metrics=( \
                    "PAPI_TOT_CYC" \
                    "IDQ_UOPS_NOT_DELIVERED:CORE"\
                    "UOPS_ISSUED:ANY"\
                    "UOPS_RETIRED:RETIRE_SLOTS"\
                    "INT_MISC:RECOVERY_CYCLES"\
                    "CYCLE_ACTIVITY:STALLS_TOTAL"\
                    "CYCLE_ACTIVITY:STALLS_MEM_ANY"\
                    "RESOURCE_STALLS:SB"\
                    "UOPS_EXECUTED:CORE_CYCLES_GE_1" \
                    "UOPS_EXECUTED:CORE_CYCLES_GE_2" \
                    "UOPS_EXECUTED:CORE_CYCLES_GE_3" \
                    "UOPS_EXECUTED:CORE_CYCLES_GE_4" \
                    "EXE_ACTIVITY:BOUND_ON_STORES" \
                    "EXE_ACTIVITY:EXE_BOUND_0_PORTS" \
                    "RS_EVENTS:EMPTY_CYCLES" \
                    "CYCLE_ACTIVITY:STALLS_L1D_MISS" \
                    "CYCLE_ACTIVITY:STALLS_L2_MISS" \
                    "CYCLE_ACTIVITY:STALLS_L3_MISS" \
                    "PAPI_STL_CCY" \
                    "CYCLE_ACTIVITY:CYCLES_MEM_ANY" \
                    "RESOURCE_STALLS:ALL" \
                    "RESOURCE_STALLS:ANY" \
                    "IDQ_UOPS_NOT_DELIVERED:CORE"\

                    "INST_RETIRED:PREC_DIST"\
                    "IDQ:MS_UOPS"\
                    "FP_ARITH_INST_RETIRED:SCALAR_DOUBLE"\
                    "FP_ARITH_INST_RETIRED:128B_PACKED_DOUBLE"\
                    "FP_ARITH_INST_RETIRED:256B_PACKED_DOUBLE"\
                    "FP_ARITH_INST_RETIRED:512B_PACKED_DOUBLE"\
                    "FP_ARITH_INST_RETIRED:SCALAR_SINGLE"\
                    "FP_ARITH_INST_RETIRED:128B_PACKED_SINGLE"\
                    "FP_ARITH_INST_RETIRED:256B_PACKED_SINGLE"\
                    "FP_ARITH_INST_RETIRED:512B_PACKED_SINGLE"\

                    "UOPS_EXECUTED:CORE" \
                    "ARITH:DIVIDER_ACTIVE" \
                    "UOPS_DISPATCHED_PORT:PORT_0" \
                    "UOPS_DISPATCHED_PORT:PORT_1" \
                    "UOPS_DISPATCHED_PORT:PORT_2" \
                    "UOPS_DISPATCHED_PORT:PORT_3" \
                    "UOPS_DISPATCHED_PORT:PORT_4" \
                    "UOPS_DISPATCHED_PORT:PORT_5" \
                    "UOPS_DISPATCHED_PORT:PORT_6" \
                    "UOPS_DISPATCHED_PORT:PORT_7" \
                    "PARTIAL_RAT_STALLS:SCOREBOARD" \
                   )

#declare -a threads=("1" "4" "8" "16")
declare -a threads=("4")
out_dir=cali_stencil_simd_aos_t
#out_dir=cali_stencil_simd_aos
#out_dir=cali_stencil_simd_soa
#out_dir=cali_stencil_aos
#out_dir=cali_stencil_soa
#exe="./run_vpic_int_sky_28_scaling_strong_srun_mpi"

module load caliper/x86-ompi3.1.4-intel19.0.5

#mkdir ${out_dir}

export OMP_PROC_BIND=close
export OMP_PLACES=cores

export CALI_SERVICES_ENABLE=trace,event,papi,report

for t in "${threads[@]}"
do
  export OMP_NUM_THREADS=${t}
  mkdir ${out_dir}${t}
  for metric in "${metrics[@]}"
  do    
    export CALI_REPORT_CONFIG="SELECT * FORMAT json"
    # export CALI_REPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested FORMAT json"
    export CALI_PAPI_COUNTERS=${metric}
  
    export CALI_REPORT_FILENAME=${out_dir}${t}/cali_${metric}.json
    ./test_stencil

  done
done



