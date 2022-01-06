#ifndef NGSNGSFUNC_H
#define NGSNGSFUNC_H

void DNA_complement(char seq[]);

void reverseChar(char* str);

int Random_geometric_k(unsigned int  seed, const double p);

double myrand(unsigned int persistent);

void SimBriggsModel(char* reffrag, char* frag, int L, double nv, double lambda, double delta_s, double delta, unsigned int seed);

const char* Error_lookup(double a,double err[6000],int nt_offset, int read_pos,int outputoffset);

double* Qual_array(double* freqval,const char* filename);

void Read_Qual_new(char *seq,char *qual,unsigned int seed,double* freqval,int outputoffset);

int BinarySearch_fraglength(double* SearchArray,int low, int high, double key);

void FragArray(int& number,int*& Length, double*& Frequency,const char* filename);

void printTime(FILE *fp);

void Header_func(htsFormat *fmt_hts,const char *outfile_nam,samFile *outfile,sam_hdr_t *header,faidx_t *seq_ref,int chr_total, size_t genome_len);

char* full_genome_create(faidx_t *seq_ref,int chr_total,int chr_sizes[],const char *chr_names[],int chr_size_cumm[]);

typedef struct{
    int n;
    int* alias;
    double* prob;
} ransampl_ws;

ransampl_ws* ransampl_alloc(int n);

void ransampl_set( ransampl_ws *ws, const double *p );

int ransampl_draw2( ransampl_ws *ws,double r1, double r2); //added below function to make it threadsafe tsk 23dec 2021

void ransampl_free( ransampl_ws *ws );

ransampl_ws ***ReadQuality(char *ntqual, int ntcharoffset,const char *freqfile);

#endif /* NGSNGSFUNC_H */