#include <algorithm>
#include <cstdio>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>

#include <htslib/faidx.h>
#include <htslib/sam.h>
#include <htslib/vcf.h>
#include <htslib/kstring.h>
#include <zlib.h>

#include <cstdlib>
#include <ctime>

#include <cstdio>
#include <cassert>
#include <cstdint>

#include <random>
#include <iterator>
#include <cmath>

#include <thread>         // std::thread
#include <mutex>          // std::mutex mtx;
#include <bits/stdc++.h>

#include "SimulAncient_func.h"

pthread_mutex_t data_mutex;

struct Parsarg_for_Fafa_thread{
  int chr_idx;
  kstring_t *fqresult;
  faidx_t *seq_ref;
  //const char* Output_type;
};

//-------------- FUNCTIONS ----------------//

void coverage(float cov){
  const char *fastafile = "/willerslev/users-shared/science-snm-willerslev-wql443/scratch/reference_files/Human/chr1.fa";
  faidx_t *seq_ref = NULL;
  seq_ref  = fai_load(fastafile);
  assert(seq_ref!=NULL);
  int chr_no = 0;
  const char *chr_name = faidx_iseq(seq_ref,chr_no);
  int chr_len = 1000; //faidx_seq_len(seq_ref,chr_name);
  int n= 1000; //chr_len;//chr_len ;// chr_len/3000000;
  
  int* chrlencov = (int *) calloc(chr_len,sizeof(int));
  //int* chrlencov = new int[n];
  //memset(chrlencov, 0, n*sizeof(int)); 

  float cov_current = 0;
  srand(time(NULL));
  int start;
  int length;
  int nread = 0;
  //int sum = 0;
  while (cov_current < cov) {
    int sum = 0;
    int count = chr_len;
    //length = 500000 + rand() % 1000000;
    length = 50 + rand() % 100;
    start = lrand48() % (chr_len-length-1);
    fprintf(stderr,"start %i and length %i \n",start,length);
    for (int j = start; j < start+length; j++){chrlencov[j] += 1;}
    for (size_t i = 0; i < chr_len; i++){std::cout << chrlencov[i] << ' ';}
    std::cout << std::endl;
    for(int i = 0; i<chr_len ; i++){
      if (chrlencov[i] == 0){count--;}
      else{sum+=chrlencov[i];}
    }
    nread++;
    fprintf(stderr,"count %i and sum %i \n",count,sum);
    cov_current = (float) sum / (float) count;
    std::cout << "number of reads "<< nread << std::endl;
    std::cout << "current cov"<< cov_current << std::endl;
    std::cout << "-----------" << std::endl;
  }
  // free(chrlencov);
  // delete[] chrlencov; //Hvorfor får jeg core aborted hvis jeg bruger delete
}

// --------------------- //
int main(int argc,char **argv){
  coverage(2);
}

// g++ SimulAncient_func.cpp FaFa_thread_cov.cpp -std=c++11 -I /home/wql443/scratch/htslib/ /home/wql443/scratch/htslib/libhts.a -lpthread -lz -lbz2 -llzma -lcurl
// ./a.out fa 5


/*


void coverage(int cov){
  const char *fastafile = "/willerslev/users-shared/science-snm-willerslev-wql443/scratch/reference_files/Human/chr1.fa";
  faidx_t *seq_ref = NULL;
  seq_ref  = fai_load(fastafile);
  assert(seq_ref!=NULL);
  int chr_no = 0;
  const char *chr_name = faidx_iseq(seq_ref,chr_no);
  int chr_len = faidx_seq_len(seq_ref,chr_name);
  int n= chr_len;//chr_len ;// chr_len/3000000;

  //auto chrlencov = new int[n]();
  int* chrlencov = new int[n];
  memset(chrlencov, 0, n*sizeof(int)); 

  //for (size_t i = 0; i < n; i++){std::cout << chrlencov[i] << ' ';}

  float cov_current = 0;
  srand(time(NULL));
  int start;
  int length;
  int nread = 0;
  //int sum = 0;
  while (cov_current < cov) {
    int sum = 0;
    int count = n;
    length = 500000 + rand() % 1000000;
    start = lrand48() % (n-length-1);
    fprintf(stderr,"start %i and length %i \n",start,length);
    for (int j = start; j < start+length; j++){chrlencov[j] += 1;}
    //for (size_t i = 0; i < n; i++){std::cout << chrlencov[i] << ' ';}
    std::cout << std::endl;
    for(int i = 0; i<n ; i++){
      if (chrlencov[i] == 0){count--;}
      else{sum+=chrlencov[i];}
    }
    nread++;
    fprintf(stderr,"count %i and sum %i \n",count,sum);
    cov_current = (float) sum / (float) count;
    std::cout << "number of reads "<< nread << std::endl;
    std::cout << "current cov"<< cov_current << std::endl;
    std::cout << "-----------" << std::endl;
  }
  // free(chrlencov);
  // delete[] chrlencov; //Hvorfor får jeg core aborted hvis jeg bruger delete
}

*/