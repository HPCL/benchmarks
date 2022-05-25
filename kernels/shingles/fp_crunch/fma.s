	.section .text
.LNDBG_TX:
# mark_description "Intel(R) C Intel(R) 64 Compiler for applications running on Intel(R) 64, Version 19.0.5.281 Build 20190815";
# mark_description "-O3 -g -DVLEN=8 -march=skylake-avx512 -qopt-zmm-usage=high -DUSE_SIMD -g -S -std=c11 -DDATA_SIZE=8 -DDP";
	.file "fma.c"
	.text
..TXTST0:
.L_2__routine_start_fma_0:
# -- Begin  fma
	.text
# mark_begin;
       .align    16,0x90
	.globl fma
# --- fma(double *__restrict__, double *__restrict__, double *__restrict__)
fma:
# parameter 1(mat_a): %rdi
# parameter 2(mat_b): %rsi
# parameter 3(mat_c): %rdx
..B1.1:                         # Preds ..B1.0
                                # Execution count [1.00e+00]
..L5:
                # optimization report
                # LOOP WAS COMPLETELY UNROLLED
                # LOOP WAS VECTORIZED
                # SIMD LOOP
                # VECTORIZATION HAS UNALIGNED MEMORY REFERENCES
                # VECTORIZATION SPEEDUP COEFFECIENT 3.144531
                # LOOP HAS ONE VECTOR ITERATION
                # VECTOR LENGTH 8
                # NORMALIZED VECTORIZATION OVERHEAD 0.750000
                # MAIN VECTOR TYPE: 64-bits floating point
                # DEPENDENCY ANALYSIS WAS IGNORED
                # COST MODEL DECISION WAS IGNORED
..LN0:
	.file   1 "fma.c"
	.loc    1  15  is_stmt 1
	.cfi_startproc
..___tag_value_fma.2:
..L3:
                                                          #9.5
..LN1:
	.loc    1  9  prologue_end  is_stmt 1
..LN2:
	.loc    1  16  is_stmt 1
        vmovups   (%rdx), %zmm1                                 #16.17
..LN3:
        vmovups   (%rsi), %zmm0                                 #16.28
..LN4:
        vfmadd213pd (%rdi), %zmm0, %zmm1                        #16.5
..LN5:
        vmovupd   %zmm1, (%rdi)                                 #16.5
..LN6:
	.loc    1  19  is_stmt 1
        vzeroupper                                              #19.1
..LN7:
	.loc    1  19  epilogue_begin  is_stmt 1
        ret                                                     #19.1
        .align    16,0x90
..LN8:
                                # LOE
..LN9:
	.cfi_endproc
# mark_end;
	.type	fma,@function
	.size	fma,.-fma
..LNfma.10:
.LNfma:
	.data
# -- End  fma
	.data
	.section .debug_opt_report, ""
..L9:
	.ascii ".itt_notify_tab\0"
	.word	258
	.word	48
	.long	2
	.long	..L10 - ..L9
	.long	48
	.long	..L11 - ..L9
	.long	24
	.long	0x00000008,0x00000000
	.long	0
	.long	0
	.long	0
	.long	1
	.quad	..L5
	.long	28
	.long	5
..L10:
	.long	1769238639
	.long	1635412333
	.long	1852795252
	.long	1885696607
	.long	1601466991
	.long	1936876918
	.long	7237481
	.long	1769238639
	.long	1635412333
	.long	1852795252
	.long	1885696607
	.long	7631471
..L11:
	.long	-2062548224
	.long	-2146430462
	.long	-1065320320
	.long	-2139062144
	.long	-1529838958
	.long	450725832
	.section .note.GNU-stack, ""
// -- Begin DWARF2 SEGMENT .debug_info
	.section .debug_info
.debug_info_seg:
	.align 1
	.4byte 0x00000095
	.2byte 0x0004
	.4byte .debug_abbrev_seg
	.byte 0x08
//	DW_TAG_compile_unit:
	.byte 0x01
//	DW_AT_comp_dir:
	.4byte .debug_str
//	DW_AT_name:
	.4byte .debug_str+0x3a
//	DW_AT_producer:
	.4byte .debug_str+0x40
	.4byte .debug_str+0xac
//	DW_AT_language:
	.byte 0x01
//	DW_AT_use_UTF8:
	.byte 0x01
//	DW_AT_low_pc:
	.8byte .L_2__routine_start_fma_0
//	DW_AT_high_pc:
	.8byte ..LNfma.10-.L_2__routine_start_fma_0
//	DW_AT_stmt_list:
	.4byte .debug_line_seg
//	DW_TAG_subprogram:
	.byte 0x02
//	DW_AT_decl_line:
	.byte 0x05
//	DW_AT_decl_file:
	.byte 0x01
//	DW_AT_prototyped:
	.byte 0x01
//	DW_AT_name:
	.4byte 0x00616d66
	.4byte 0x00616d66
//	DW_AT_low_pc:
	.8byte .L_2__routine_start_fma_0
//	DW_AT_high_pc:
	.8byte ..LNfma.10-.L_2__routine_start_fma_0
//	DW_AT_external:
	.byte 0x01
//	DW_TAG_formal_parameter:
	.byte 0x03
//	DW_AT_decl_line:
	.byte 0x06
//	DW_AT_decl_file:
	.byte 0x01
//	DW_AT_type:
	.4byte 0x00000080
//	DW_AT_name:
	.4byte .debug_str+0x11b
//	DW_AT_location:
	.2byte 0x5501
//	DW_TAG_formal_parameter:
	.byte 0x03
//	DW_AT_decl_line:
	.byte 0x07
//	DW_AT_decl_file:
	.byte 0x01
//	DW_AT_type:
	.4byte 0x00000080
//	DW_AT_name:
	.4byte .debug_str+0x121
//	DW_AT_location:
	.2byte 0x5401
//	DW_TAG_formal_parameter:
	.byte 0x03
//	DW_AT_decl_line:
	.byte 0x08
//	DW_AT_decl_file:
	.byte 0x01
//	DW_AT_type:
	.4byte 0x00000080
//	DW_AT_name:
	.4byte .debug_str+0x127
//	DW_AT_location:
	.2byte 0x5101
//	DW_TAG_variable:
	.byte 0x04
//	DW_AT_decl_line:
	.byte 0x0b
//	DW_AT_decl_file:
	.byte 0x01
//	DW_AT_name:
	.2byte 0x0069
//	DW_AT_type:
	.4byte 0x00000091
	.byte 0x00
//	DW_TAG_restrict_type:
	.byte 0x05
//	DW_AT_type:
	.4byte 0x00000085
//	DW_TAG_pointer_type:
	.byte 0x06
//	DW_AT_type:
	.4byte 0x0000008a
//	DW_TAG_base_type:
	.byte 0x07
//	DW_AT_byte_size:
	.byte 0x08
//	DW_AT_encoding:
	.byte 0x04
//	DW_AT_name:
	.4byte .debug_str+0x114
//	DW_TAG_base_type:
	.byte 0x08
//	DW_AT_byte_size:
	.byte 0x04
//	DW_AT_encoding:
	.byte 0x05
//	DW_AT_name:
	.4byte 0x00746e69
	.byte 0x00
// -- Begin DWARF2 SEGMENT .debug_line
	.section .debug_line
.debug_line_seg:
	.align 1
// -- Begin DWARF2 SEGMENT .debug_abbrev
	.section .debug_abbrev
.debug_abbrev_seg:
	.align 1
	.byte 0x01
	.byte 0x11
	.byte 0x01
	.byte 0x1b
	.byte 0x0e
	.byte 0x03
	.byte 0x0e
	.byte 0x25
	.byte 0x0e
	.2byte 0x7681
	.byte 0x0e
	.byte 0x13
	.byte 0x0b
	.byte 0x53
	.byte 0x0c
	.byte 0x11
	.byte 0x01
	.byte 0x12
	.byte 0x07
	.byte 0x10
	.byte 0x17
	.2byte 0x0000
	.byte 0x02
	.byte 0x2e
	.byte 0x01
	.byte 0x3b
	.byte 0x0b
	.byte 0x3a
	.byte 0x0b
	.byte 0x27
	.byte 0x0c
	.byte 0x03
	.byte 0x08
	.2byte 0x4087
	.byte 0x08
	.byte 0x11
	.byte 0x01
	.byte 0x12
	.byte 0x07
	.byte 0x3f
	.byte 0x0c
	.2byte 0x0000
	.byte 0x03
	.byte 0x05
	.byte 0x00
	.byte 0x3b
	.byte 0x0b
	.byte 0x3a
	.byte 0x0b
	.byte 0x49
	.byte 0x13
	.byte 0x03
	.byte 0x0e
	.byte 0x02
	.byte 0x18
	.2byte 0x0000
	.byte 0x04
	.byte 0x34
	.byte 0x00
	.byte 0x3b
	.byte 0x0b
	.byte 0x3a
	.byte 0x0b
	.byte 0x03
	.byte 0x08
	.byte 0x49
	.byte 0x13
	.2byte 0x0000
	.byte 0x05
	.byte 0x37
	.byte 0x00
	.byte 0x49
	.byte 0x13
	.2byte 0x0000
	.byte 0x06
	.byte 0x0f
	.byte 0x00
	.byte 0x49
	.byte 0x13
	.2byte 0x0000
	.byte 0x07
	.byte 0x24
	.byte 0x00
	.byte 0x0b
	.byte 0x0b
	.byte 0x3e
	.byte 0x0b
	.byte 0x03
	.byte 0x0e
	.2byte 0x0000
	.byte 0x08
	.byte 0x24
	.byte 0x00
	.byte 0x0b
	.byte 0x0b
	.byte 0x3e
	.byte 0x0b
	.byte 0x03
	.byte 0x08
	.2byte 0x0000
	.byte 0x00
// -- Begin DWARF2 SEGMENT .debug_str
	.section .debug_str,"MS",@progbits,1
.debug_str_seg:
	.align 1
	.8byte 0x6f682f747361762f
	.8byte 0x65766172672f656d
	.8byte 0x636e65622f656c6c
	.8byte 0x6b2f736b72616d68
	.8byte 0x732f736c656e7265
	.8byte 0x2f73656c676e6968
	.8byte 0x636e7572635f7066
	.2byte 0x0068
	.4byte 0x2e616d66
	.2byte 0x0063
	.8byte 0x2952286c65746e49
	.8byte 0x6c65746e49204320
	.8byte 0x4320343620295228
	.8byte 0x2072656c69706d6f
	.8byte 0x6c70706120726f66
	.8byte 0x736e6f6974616369
	.8byte 0x676e696e6e757220
	.8byte 0x65746e49206e6f20
	.8byte 0x2c3436202952286c
	.8byte 0x6e6f697372655620
	.8byte 0x2e352e302e393120
	.8byte 0x6c69754220313832
	.8byte 0x3830393130322064
	.4byte 0x000a3531
	.8byte 0x2d20672d20334f2d
	.8byte 0x20383d4e454c5644
	.8byte 0x733d686372616d2d
	.8byte 0x612d656b616c796b
	.8byte 0x712d203231357876
	.8byte 0x2d6d6d7a2d74706f
	.8byte 0x69683d6567617375
	.8byte 0x455355442d206867
	.8byte 0x672d20444d49535f
	.8byte 0x6474732d20532d20
	.8byte 0x44442d203131633d
	.8byte 0x455a49535f415441
	.8byte 0x005044442d20383d
	.4byte 0x62756f64
	.2byte 0x656c
	.byte 0x00
	.4byte 0x5f74616d
	.2byte 0x0061
	.4byte 0x5f74616d
	.2byte 0x0062
	.4byte 0x5f74616d
	.2byte 0x0063
	.section .text
.LNDBG_TXe:
# End
