/*************************************************************************
	> File Name: read_seeding.h
	> Author: 
	> Mail: 
	> Created Time: 2017年04月11日 星期二 15时07分43秒
 ************************************************************************/

#ifndef _READ_SEEDING_H
#define _READ_SEEDING_H

#include <stdio.h>
#include <stdint.h>
#include <getopt.h> 
#include <zlib.h>

#define INDEX_KMER 22
#define SEEDING_KMER 15
#define LOCAL_HASH_KMER 8
#define SEED_STEP 5
#define MAX_UNI_POS 50
#define TOP_NUM_ALN 4
#define MAX_EXON 500 
#define GAP_OPEN 2
#define GAP_OPEN2 32
#define GAP_EXT 1
#define GAP_EXT2 0
#define MATCH_SCORE 1
#define MISMATCH_SCORE 2
#define ZDROP_SCORE 400
#define BANDWIDTH 500
#define MIN_FRAG_DIS 20
#define SECONDARY_TO_PRIMARY 0.9
#define E_SHIFT 5
#define STRAND_DIFF 20
#define MIN_CHAIN_SCORE 20
#define MAX_READ_JOIN_GAP 2000
#define BATCH_SIZE 100000
#define TEMP_FILE_PERFIRX "1pass_anchor"
#define OUTPUT "./aln.sam"


#define UNPIPATH_OFF_K20
#define UNI_SEQ64
#define HASH_KMER_READ_J

#define MAX_PTHREAD 48
#define MAX_READLEN 1000000 //2^15+1 > readlen_max = 30000
#define SPLICDISTANCE 200000   //200000 shuold be better
//#define Annoation

pthread_rwlock_t rwlock;

typedef struct { uint64_t x, y; } mm128_t;

typedef struct READ
{
	char* read_seq;
	char* name;
	uint32_t read_length;
} READ_t;

typedef struct TARGET
{
	uint32_t ts;
	uint32_t te;
	uint16_t cov; //cal the coverage of this exon;
	uint8_t strand;
} TARGET_t;

typedef struct QUERY
{
	uint32_t qs;
	uint32_t qe;
} QUERY_t;

typedef struct PATH
{
	float dist;
	int32_t pre_node;
} PATH_t;

typedef struct ISOFORM
{
	uint32_t pos;
	uint32_t length;
	uint32_t* cigar;
	int32_t cigarLen;
	uint8_t flag;
	char* ref_name;
}iso_form;

typedef struct vertex_MEM
{
	// uint32_t id;
	uint64_t uid;
	uint32_t seed_id;
	uint32_t read_pos;
	uint32_t uni_pos_off;
	uint32_t length;
	uint32_t pos_n;
}vertex_m;

typedef struct vertex_U
{
	uint64_t uid;
	uint32_t read_pos;
	uint32_t uni_pos_off;
	uint32_t length1; //length in read
	uint32_t length2;// length in reference
	uint32_t pos_n;
	uint32_t cov;
}vertex_u;

typedef struct UNI_SEED
{
	uint32_t read_begin;
	uint32_t read_end;
	uint32_t seed_id; //record the first seed_id of those mems which can be merged
	uint32_t ref_begin;
	uint32_t ref_end;
	uint32_t cov;

    //added
    uint64_t uid;
}uni_seed;

typedef struct{
	int8_t match_D;
	int8_t mismatch_D;
	int8_t gap_open_D;
    int8_t gap_ex_D;
    int8_t gap_open2_D;
    int8_t gap_ex2_D;
    int8_t match_R;
	int8_t mismatch_R;
	int8_t gap_open_R;
    int8_t gap_ex_R;
    int8_t gap_open2_R;
    int8_t gap_ex2_R;
    int16_t max_extend_gap; //the max allowed left extension gap in read;
    int16_t bw;
    uint16_t zdrop_D; //for DNA zdrop = 400, 200 for RNA
    uint16_t zdrop_R;
    int noncan; //cost of non-canonical splicing sites
    int end_bonus;
    int batch_size;
	
	float secondary_ratio;
	uint8_t simulated;
	uint8_t read_type;
	uint8_t with_qual; //print SAM with qual or not
	uint16_t Eindel;
	uint8_t thread_n;
	uint8_t top_n;
	uint8_t seed_step;
	uint8_t hash_kmer;
	uint8_t e_shift;
	uint8_t k_t; //index kmer
	uint8_t seed_k_t; //alignment kmer
    uint8_t with_gtf;
    int strand_diff;

	float error_overall;
	float error_ins;
	float error_del;

	uint16_t pos_n_max;
	int readlen_max;
	uint32_t max_intron_length;
	uint32_t max_exon_num_per_read;
	int64_t max_sw_mat;
	int min_chain_score;
    int max_read_join_gap;
	int max_extend_left_right;
	char *temp_file_perfix;
	char *sam_path;
	char *gtf_path;

}param_map;


typedef struct
{
	int32_t dp_score, dp_max, dp_max2;  // DP score; score of the max-scoring segment; score of the best alternate mappings
	uint32_t n_ambi; // number of ambiguous bases; transcript strand: 0 for unknown, 1 for +, 2 for -
	// int32_t qs, qe, rs, re;         // query start and end; reference start and end
	int32_t mlen, blen;             // seeded exact match length; seeded alignment block length
	int chr_n;
	uint16_t flag:10;
	uint32_t _1_based_pos;
	// uint16_t mapq:10;

	uint32_t n_cigar;                   // number of cigar operations in cigar[]
	uint32_t *cigar;
} _aln_t;


typedef struct SEQIOIN
{
	char* read_seq;
	char* qual;
	char* name;
	uint32_t read_length;

	uint8_t mapable;
	uint8_t multi_n;  //multiple alignment count
	uint16_t mapq;
	_aln_t *aln; //point 
}seq_io;

//DP result
typedef struct ANCHOR
{
	uint32_t strand;
	uint32_t anchor_n;
	int primary;
	uint32_t** anchor_pos; //[anchor_n][4]
} anchor_t;

typedef struct DP_RESULT
{
	// uint32_t read_i;
	uint8_t multi_n;
	anchor_t *point;
} dpSkeleton_t;

//pthread
typedef struct THREAD_DATA
{
	uint8_t tid;
	uint32_t seqn;
	dpSkeleton_t *dp_skeleton;
} thread_aln_t;

vertex_m*** vertexm;
vertex_u*** vertexu;
READ_t* query_info;

//variable in this file
uint32_t seed_num;
uint8_t k_r;
uint8_t re_b;
uint8_t re_bt;
uint8_t re_2bt;
uint8_t seed_step;
uint8_t seed_offset;
uint16_t pos_n_max;
uint16_t uni_pos_n_max;
int batch_size;
uint8_t top_n;
float secondary_ratio;
int min_chain_score;
int max_read_join_gap;
char temp_anchor_dir[1024];
char temp_binary_pos[1024];
FILE *fp_tff;
FILE *fp_temp;

//global variable
extern uni_seed*** uniseed;
extern int8_t* mata_D;
extern int8_t* mata_R;
extern uint8_t k_t;
extern uint8_t seed_k_t;
extern uint8_t BASE_true;
extern uint32_t new_seed_cnt;
extern char *sam_path;
extern FILE *fp_sam;
extern uint32_t max_exon_num_per_read;
extern int readlen_max;
extern uint16_t Eindel;
extern uint8_t thread_n;
extern uint32_t max_intron_length;
extern int waitingLen;
extern int TEMP_INDEX;

uint64_t read_bit1[MAX_PTHREAD][2][((MAX_READLEN - 1) >> 5) + 1]; //  MAX_READLEN=2048  ((MAX_READLEN - 1) >> 5) + 1 = 64

int help_usage();
int tgs_aln(int argc, char *argv[], const char *version);

#endif
