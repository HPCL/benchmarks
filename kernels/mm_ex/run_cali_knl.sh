#!/bin/bash
#

# declare -a metrics=("PAPI_TOT_CYC")

declare -a metrics=( \
                    "PAPI_TOT_CYC" \
                    "INST_RETIRED:ANY" \
                    "NO_ALLOC_CYCLES:MISPREDICTS" \
                    "NO_ALLOC_CYCLES:NOT_DELIVERED" \
                    "UOPS_RETIRED:ALL" \
                    "UOPS_RETIRED:SCALAR_SIMD" \
                    "UOPS_RETIRED:PACKED_SIMD" \
                    "CYCLES_DIV_BUSY:ALL" \
                   )


#out_dir=cali_mm_knl_5000_avx512_base
out_dir=cali_mm_knl_5000_base
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


#out_dir=cali_mm_knl_5000_avx512_transpose
#out_dir=cali_mm_knl_5000_transpose
mkdir ${out_dir}

for metric in "${metrics[@]}"
do    
  export CALI_MPIREPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested,mpi.rank FORMAT json"
  export CALI_PAPI_COUNTERS=${metric}
  
  export CALI_MPIREPORT_FILENAME=${out_dir}/cali_${metric}.json
  #mpirun -n 40 ./mm_mpi -t

done


