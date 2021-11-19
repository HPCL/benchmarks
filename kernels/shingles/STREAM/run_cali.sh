#!/bin/bash
#

# declare -a metrics=("PAPI_TOT_CYC")

declare -a metrics=("PAPI_TOT_CYC" \
                    "IDQ_UOPS_NOT_DELIVERED:CORE"\
                    "UOPS_ISSUED:ANY"\
                    "UOPS_RETIRED:RETIRE_SLOTS"\
                    "INT_MISC:RECOVERY_CYCLES"\
                    "CYCLE_ACTIVITY:STALLS_TOTAL"\
                    "CYCLE_ACTIVITY:STALLS_MEM_ANY"\
                    "RESOURCE_STALLS:SB"\
                  # )

# declare -a metrics=( \
                    "UOPS_EXECUTED:CORE_CYCLES_GE_1" \
                    "UOPS_EXECUTED:CORE_CYCLES_GE_2" \
                    "RS_EVENTS:EMPTY_CYCLES"
                   # )

# declare -a metrics=( \
                    "CYCLE_ACTIVITY:STALLS_L1D_MISS" \
                    "CYCLE_ACTIVITY:STALLS_L2_MISS" \
                    "CYCLE_ACTIVITY:STALLS_L3_MISS"
                   # )

# declare -a metrics=(
                    "PAPI_STL_CCY" \
                    "CYCLE_ACTIVITY:CYCLES_MEM_ANY" \
                    "RESOURCE_STALLS:ALL" \
                    "RESOURCE_STALLS:ANY" \
                   )


out_dir=cali_stream
#exe="./run_vpic_int_sky_28_scaling_strong_srun_mpi"


mkdir ${out_dir}

export OMP_PROC_BIND=close
export OMP_PLACES=cores

export CALI_SERVICES_ENABLE=trace,event,papi,report

for metric in "${metrics[@]}"
do    
  # export CALI_REPORT_CONFIG="SELECT * FORMAT json"
  export CALI_REPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested,thread_id FORMAT json"
  export CALI_PAPI_COUNTERS=${metric}
  
  export CALI_REPORT_FILENAME=${out_dir}/cali_${metric}.json
  ./stream_c.exe

done




