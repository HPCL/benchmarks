#!/bin/bash
#

# declare -a metrics=("PAPI_TOT_CYC")

declare -a metrics=(\
                   "PAPI_TOT_CYC" \
                   "CPU_CYCLES" \
                   "INST_RETIRED" \
                   "INST_SPEC" \
                   "STALL_BACKEND" \
                   "STALL_FRONTEND" \ 
                   "ISSQ_RECYCLE" \
                   "GPR_RECYCLE" \
                   "FPR_RECYCLE" \
                   "LRQ_RECYCLE" \
                   "SRQ_RECYCLE" \
                   "BSR_RECYCLE" \
                   "ROB_RECYCLE" \
                   "LD_RETIRED" \
                   "ST_RETIRED" \
                   "BR_RETIRED" \
                   "UNALIGNED_LDST_RETIRED" \
                   )

module load caliper/arm-ompi3.1.4-gcc9.2
#out_dir=cali_mm_5000_simd_base
out_dir=cali_mm_5000_base
mkdir ${out_dir}

export OMP_PROC_BIND=close
export OMP_PLACES=cores

export CALI_SERVICES_ENABLE=trace,event,papi,mpi,mpireport

for metric in "${metrics[@]}"
do    
  export CALI_MPIREPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested,mpi.rank FORMAT json"
  export CALI_PAPI_COUNTERS=${metric}
  
  export CALI_MPIREPORT_FILENAME=${out_dir}/cali_${metric}.json
  #mpirun -n 56 ./mm_mpi

done


#out_dir=cali_mm_5000_simd_transpose
out_dir=cali_mm_5000_transpose
mkdir ${out_dir}

for metric in "${metrics[@]}"
do    
  export CALI_MPIREPORT_CONFIG="SELECT *,sum(papi.${metric}) GROUP BY prop:nested,mpi.rank FORMAT json"
  export CALI_PAPI_COUNTERS=${metric}
  
  export CALI_MPIREPORT_FILENAME=${out_dir}/cali_${metric}.json

  mpirun -n 56 ./mm_mpi -t

done


