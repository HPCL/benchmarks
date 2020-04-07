#!/bin/bash
#

# declare -a metrics=("PAPI_TOT_CYC")

declare -a metrics=(\
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

                   )


# out_dir=cali_mm_2000_avx512_base
out_dir=cali_mm_2000_base
mkdir ${out_dir}

export OMP_PROC_BIND=close
export OMP_PLACES=cores

export CALI_SERVICES_ENABLE=trace,event,papi,mpi,mpireport

for metric in "${metrics[@]}"
do    
  export CALI_MPIREPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested,mpi.rank FORMAT json"
  export CALI_PAPI_COUNTERS=${metric}
  
  export CALI_MPIREPORT_FILENAME=${out_dir}/cali_${metric}.json
  mpirun -n 40 ./mm_mpi

done


# out_dir=cali_mm_2000_avx512_transpose
out_dir=cali_mm_2000_transpose
mkdir ${out_dir}

for metric in "${metrics[@]}"
do    
  export CALI_MPIREPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested,mpi.rank FORMAT json"
  export CALI_PAPI_COUNTERS=${metric}
  
  export CALI_MPIREPORT_FILENAME=${out_dir}/cali_${metric}.json
  mpirun -n 40 ./mm_mpi -t

done


