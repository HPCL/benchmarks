#!/bin/bash
#

declare -a _metrics=("PAPI_TOT_CYC")

declare -a _metrics=( \
                    "PAPI_L2_STM" \
                    )

declare -a metrics=( \
                   "skx_unc_imc0::UNC_M_CAS_COUNT:RD:cpu=0" \
                   "skx_unc_imc1::UNC_M_CAS_COUNT:RD:cpu=0" \
                   "skx_unc_imc2::UNC_M_CAS_COUNT:RD:cpu=0" \
                   "skx_unc_imc3::UNC_M_CAS_COUNT:RD:cpu=0" \
                   "skx_unc_imc4::UNC_M_CAS_COUNT:RD:cpu=0" \
                   "skx_unc_imc5::UNC_M_CAS_COUNT:RD:cpu=0" \

                   "skx_unc_imc0::UNC_M_CAS_COUNT:WR:cpu=0" \
                   "skx_unc_imc1::UNC_M_CAS_COUNT:WR:cpu=0" \
                   "skx_unc_imc2::UNC_M_CAS_COUNT:WR:cpu=0" \
                   "skx_unc_imc3::UNC_M_CAS_COUNT:WR:cpu=0" \
                   "skx_unc_imc4::UNC_M_CAS_COUNT:WR:cpu=0" \
                   "skx_unc_imc5::UNC_M_CAS_COUNT:WR:cpu=0" \

                   "skx_unc_imc0::UNC_M_CAS_COUNT:RD:cpu=25" \
                   "skx_unc_imc1::UNC_M_CAS_COUNT:RD:cpu=25" \
                   "skx_unc_imc2::UNC_M_CAS_COUNT:RD:cpu=25" \
                   "skx_unc_imc3::UNC_M_CAS_COUNT:RD:cpu=25" \
                   "skx_unc_imc4::UNC_M_CAS_COUNT:RD:cpu=25" \
                   "skx_unc_imc5::UNC_M_CAS_COUNT:RD:cpu=25" \

                   "skx_unc_imc0::UNC_M_CAS_COUNT:WR:cpu=25" \
                   "skx_unc_imc1::UNC_M_CAS_COUNT:WR:cpu=25" \
                   "skx_unc_imc2::UNC_M_CAS_COUNT:WR:cpu=25" \
                   "skx_unc_imc3::UNC_M_CAS_COUNT:WR:cpu=25" \
                   "skx_unc_imc4::UNC_M_CAS_COUNT:WR:cpu=25" \
                   "skx_unc_imc5::UNC_M_CAS_COUNT:WR:cpu=25" \

                    "rapl::RAPL_ENERGY_PKG:cpu=0" \
                    "rapl::RAPL_ENERGY_DRAM:cpu=0" \
                    "rapl::RAPL_ENERGY_PKG:cpu=25" \
                    "rapl::RAPL_ENERGY_DRAM:cpu=25" \
                   )


declare -a _metrics=( \
                    "PAPI_TOT_CYC" \
                    "UNHALTED_REFERENCE_CYCLES" \
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

                    "PAPI_LD_INS" \
                    "PAPI_SR_INS" \

                    "PAPI_SP_OPS" \
                    "PAPI_DP_OPS" \

                    "MEM_LOAD_UOPS_RETIRED:L3_MISS" \
                    "MEM_UOPS_RETIRED:ALL_STORES" \
                    
                    "MEM_LOAD_UOPS_LLC_MISS_RETIRED:LOCAL_DRAM" \
                    "MEM_LOAD_UOPS_LLC_MISS_RETIRED:REMOTE_DRAM" \
                    "PAPI_PRF_DM" \
                    "PAPI_L1_TCM" \
                    "PAPI_L1_DCM" \
                    "PAPI_L2_TCM" \
                    "PAPI_L2_STM" \
                    "PAPI_L3_TCM" \
                    "PAPI_L3_TCW" \
                    "PAPI_L3_LDM" \
                    
                    "OFFCORE_REQUESTS:L3_MISS_DEMAND_DATA_RD" \
                    "OFFCORE_REQUESTS:ALL_DATA_RD" \
                    "OFFCORE_RESPONSE_0:ANY_REQUEST" \
                    "OFFCORE_RESPONSE_0:ANY_DATA" \
                    "OFFCORE_RESPONSE_0:ANY_RESPONSE" \
                    "OFFCORE_RESPONSE_1:ANY_REQUEST" \
                    "OFFCORE_RESPONSE_1:ANY_DATA" \
                    "OFFCORE_RESPONSE_1:ANY_RESPONSE" \
                    "OFFCORE_RESPONSE_1:L3_MISS" \
                    "OFFCORE_RESPONSE_1:L3_MISS_LOCAL" \


                   )

t=48
b=256
# module load use.own
# module load caliper/a64-ompi3.1.6-gcc11.0.0c

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
out_dir=cas_cache_conflict/conflict_cas_icc_offset_${offset}
mkdir -p ${out_dir}

for metric in "${metrics[@]}"
do 

  export CALI_PAPI_COUNTERS=${metric}
  export CALI_REPORT_FILENAME=${out_dir}/cali_${metric}.json
  ./cache_conflict -o ${offset}

done

done


