#!/bin/bash
#

declare -a _metrics=("PAPI_TOT_CYC")

declare -a _metrics=( \
                    "PAPI_L2_STM" \
                    )

declare -a _metrics=( \
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

                   "skx_unc_imc0::UNC_M_CAS_COUNT:RD:cpu=23" \
                   "skx_unc_imc1::UNC_M_CAS_COUNT:RD:cpu=23" \
                   "skx_unc_imc2::UNC_M_CAS_COUNT:RD:cpu=23" \
                   "skx_unc_imc3::UNC_M_CAS_COUNT:RD:cpu=23" \
                   "skx_unc_imc4::UNC_M_CAS_COUNT:RD:cpu=23" \
                   "skx_unc_imc5::UNC_M_CAS_COUNT:RD:cpu=23" \

                   "skx_unc_imc0::UNC_M_CAS_COUNT:WR:cpu=23" \
                   "skx_unc_imc1::UNC_M_CAS_COUNT:WR:cpu=23" \
                   "skx_unc_imc2::UNC_M_CAS_COUNT:WR:cpu=23" \
                   "skx_unc_imc3::UNC_M_CAS_COUNT:WR:cpu=23" \
                   "skx_unc_imc4::UNC_M_CAS_COUNT:WR:cpu=23" \
                   "skx_unc_imc5::UNC_M_CAS_COUNT:WR:cpu=23" \
                   )


declare -a metrics=( \
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
                    
                    "rapl::RAPL_ENERGY_PKG:cpu=0" \
                    "rapl::RAPL_ENERGY_DRAM:cpu=0" \

                    "rapl::RAPL_ENERGY_PKG:cpu=1" \
                    "rapl::RAPL_ENERGY_DRAM:cpu=1" \

                   )

# out_dir=cali_mm_flipped_icc_simd
# out_dir=cali_mm_default_icc_scalar
out_dir=cali_mm_block
# out_dir=temp
#out_dir=cali_mm_5000_scalar
#out_dir=cali_mm_unc_tests
mkdir ${out_dir}
# mkdir ${out_dir}_base
# mkdir ${out_dir}_transpose

export OMP_PROC_BIND=close
# export OMP_PLACES="{22:22:1}"
export OMP_PLACES=cores
export OMP_NUM_THREADS=44
# export OMP_NUM_THREADS=1

export CALI_SERVICES_ENABLE=trace,event,papi,report

for metric in "${metrics[@]}"
do   

  export CALI_REPORT_CONFIG="SELECT * FORMAT json" 
  export CALI_PAPI_COUNTERS=${metric}
 
  export CALI_LOG_VERBOSITY=1
 
  export CALI_REPORT_FILENAME=${out_dir}/cali_${metric}.json
  ./test_linear.out

done



