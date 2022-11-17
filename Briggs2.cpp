#include "Briggs2.h"
#include "mrand.h"
#include "fasta_sampler.h"
#include "NGSNGS_misc.h"

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <math.h>

#define LENS 4096
#define MAXBINS 100

extern int refToInt[256];
extern char NtComp[5];
extern const char *bass;

 
int SimBriggsModel2(char *ori, int L, double nv, double lambda, double delta_s, double delta, mrand_t *mr,char **res,int strandR1,int& C_to_T_counter,int& G_to_A_counter,int& C_total,int& G_total) {
  int IsDeam = 0;
  assert(L<1024);

  //fprintf(stderr,"------\nori pre %d \t%s\n",strandR1,ori);
  // The input reference should always be equal to the 5' ---> fwrd ---> 3' orientation similar to the reference genome
  if (strandR1 == 1){
    ReversComplement(ori);
  }
  //fprintf(stderr,"ori post\t%s\n",ori);

  int l = 0;
  int r = L-1;
 
  char *rasmus = res[0];
  char *thorfinn = res[2];
 
  while (l+r > L-2){
    l = 0;
    r = 0;
    double u_l = mrand_pop(mr);
    double u_r = mrand_pop(mr);
   
    if (u_l > 0.5){
      l = (int) Random_geometric_k(lambda,mr);
    }
    if (u_r > 0.5){
      r = (int) Random_geometric_k(lambda,mr);
    }
  }

  //Please do a check in the following part, since it may shift by 1
  
  strncpy(rasmus,ori,L);
  strncpy(thorfinn,ori,L); //Thorfinn equals Rasmus
  //fprintf(stderr,"rasmus orig\t%s\n",thorfinn);
  //fprintf(stderr,"thorfinn orig\t%s\n",thorfinn);
  /*
  RASMUS   = AGACT
  THORFINN = AGACT
  */
  Complement(thorfinn);
  //fprintf(stderr,"thorfinn compl\t%s\n",thorfinn);
  /*
  THORFINN = TCTGA
  */

  //fprintf(stderr,"SEQUNEC %s\n",rasmus);
  //fprintf(stderr,"SEQUNEC %s\n",rasmus)
  //fprintf(stderr,"SEQUNEC %s\n",thorfinn);
  
  // All of the positions are counting from 5' of Rasmus. 

  //fprintf(stderr,"SEQUNEC %s\n",thorfinn);

  // Contain everything strncpy(rasmus,ori,L);

 
  /*for(int i = 0; i < strlen(rasmus);i++){
    fprintf(stderr,"i value %d\n",i);
  }*/
 

  /*
    5' CGTATACATAGGCACTATATCGACCACACT 3'
    3'        TATCCGTGATATAGCTGGTGTGA 5'
  */
  for (int i = 0; i<l; i++){
    // left 5' overhangs, Thorfinn's DMG pattern is fully dependent on that of Rasmus.

    if (rasmus[i] == 'C' || rasmus[i] == 'c' ){
      if (i == 0){C_total++;G_total++;}
      double u = mrand_pop(mr);
      if (u < delta_s){
        IsDeam = 1;
        //fprintf(stderr,"%d i %c %c\n",i,rasmus[i],thorfinn[i]);
        rasmus[i] = 'T'; 
        thorfinn[i] = 'A';
        //fprintf(stderr,"%d i %c %c\n",i,rasmus[i],thorfinn[i]);
        if (i == 0){C_to_T_counter++;G_to_A_counter++;}
      }
    }
  }
  
  /*
    5' CGTATACATAGGCACTATATCGACC       3' 
    3' GCATATGTATCCGTGATATAGCTGGTGTGAC 5' 
  */
  for (int i = 0; i < r; i++){
    // right 5' overhangs, Rasmus's DMG pattern is fully dependent on that of Thorfinn.
    if (thorfinn[L-i-1] == 'C' || thorfinn[L-i-1] == 'c'){
      if (i == 0){C_total++;G_total++;}
      double u = mrand_pop(mr);
      if (u < delta_s){
        IsDeam = 1;
        thorfinn[L-i-1] = 'T';
        rasmus[L-i-1] = 'A';
        if (i == 0){C_to_T_counter++;G_to_A_counter++;}
      }
    }
  }
  
  
  // The nick positions on both strands are denoted as (m,n). m (The nick position on Rasmus) is sampled as the previous way, while n (The nick position on thorfinn) is sampled according to a 
  // conditional probability given m.
  double u_nick_m = mrand_pop(mr);
   
	// the counting starts from 0 rather than one so we shift
  double P_m = nv/((L-l-r-1)*nv+1-nv);
  int p_nick_m = l;
  double CumPm = P_m;
 
  while ((u_nick_m > CumPm) && (p_nick_m < L-r-1)){
    CumPm += P_m;
    p_nick_m +=1;
  }
  

  int p_nick_n;
  double u_nick_n = mrand_pop(mr);
  double CumPn;
 
  // Given m, sampling n
  if (p_nick_m < L-r-1){
      p_nick_n = L-p_nick_m-2; //we shift both n and m
      CumPn = nv;
      while((u_nick_n > CumPn) && (p_nick_n < L-l-1)){
         p_nick_n +=1;
         CumPn += nv*pow(1-nv,p_nick_m+p_nick_n-L+2);
      }
  }else if(p_nick_m == L-r-1){
      p_nick_n = r;
      CumPn = nv;
      while((u_nick_n > CumPn) && (p_nick_n < L-l-1)){
         p_nick_n +=1;
         CumPn += nv*pow(1-nv,p_nick_n-r);
      }
  }
  
  // Way 2 Complicated Way (should be a little bit faster)
  for (int i = l; i < L-r; i++){

    //if(i == l){fprintf(stderr,"Lille l %d and pos nick %d\n",l,p_nick_n);}
    //fprintf(stderr,"p_nick_n %d \t p_nick_m %d \t L %d \n",p_nick_n,p_nick_m,L);
    
    
    if (i<L-p_nick_n-1 && (rasmus[i] == 'C' || rasmus[i] == 'c')){
      if (i == 0){C_total++;G_total++;}
      //left of nick on thorfinn strand we change thorfinn according to rasmus

      /*
        5' CGTATACATAGGCACTATATCGACCACACT 3'
        3' GCATATGTA CCGTGATATAGCTGGTGTGA 5'
                      |
                      v
        5' CGTATACATAGGCACTATATCGACCACACT 3'
        3'           CCGTGATATAGCTGGTGTGA 5'                
      */
    
      double u = mrand_pop(mr);
      if (u < delta){
        IsDeam = 1;
        rasmus[i] = 'T';
        thorfinn[i] = 'A'; //Downstream nick one DMG pattern depends on the other strand
        if (i == 0){C_to_T_counter++;G_to_A_counter++;}
      }
    }
    else if (i>p_nick_m && (thorfinn[i] == 'C' || thorfinn[i] == 'c')){
      if (i == (L-1)){C_total++;G_total++;}
      // right side of rasmus nick we change rasmus according to thorfinn

      /*++
        5' CGTATACAT GGCACTATATCGACCACACT 3'
        3' GCATATGTATCCGTGATATAGCTGGTGTGA 5'
                      |
                      v
        5' CGTATACAT                       3'
        3' GCATATGTATCCGTGATATAGCTGGTGTGA  5'                
      */

      double u = mrand_pop(mr);
      if (u < delta){
        IsDeam = 1;
        rasmus[i] = 'A';
        thorfinn[i] = 'T'; //Downstream nick one DMG pattern depends on the other strand
        if (i == (L-1)){C_to_T_counter++;G_to_A_counter++;}
      }
    }
	
    // between the nick with rasmus showing DMG
    else if(i>=L-p_nick_n-1 && i<=p_nick_m && (rasmus[i] == 'C' || rasmus[i] == 'c')){
      if (i == 0){C_total++;G_total++;}
      double u = mrand_pop(mr);
      if (u < delta){
        IsDeam = 1;
        rasmus[i] = 'T'; //Upstream both nicks, DMG patterns are independent
        if (i == 0){C_to_T_counter++;}
      }
    }
    // between the nick with Thorfinn showing DMG
    else if(i>=L-p_nick_n-1 && i<=p_nick_m && (thorfinn[i] == 'C' || thorfinn[i] == 'c')){
      if (i == (L-1)){C_total++;G_total++;}
      //fprintf(stderr,"i value %d\n",i);
      double u = mrand_pop(mr);
      if (u < delta){
        IsDeam = 1;
        thorfinn[i] = 'T'; //Upstream both nicks, DMG patterns are independent
        if (i == (L-1)){C_to_T_counter++;}
      }
    }
  }
  
  //fprintf(stderr,"thorfinn deam\t%s\n",thorfinn);
  //Change orientation of Thorfinn to reverse strand
  reverseChar(thorfinn,strlen(thorfinn));
  //fprintf(stderr,"thorfinn rev\t%s\n",thorfinn);
  char RasmusTmp[1024] = {0};
  char ThorfinnTmp[1024] = {0};
  memset(RasmusTmp, 0, sizeof RasmusTmp);
  memset(ThorfinnTmp, 0, sizeof ThorfinnTmp);
  strcpy(RasmusTmp,rasmus);
  res[0] = RasmusTmp;
  strcpy(ThorfinnTmp,thorfinn);
  res[1] = ThorfinnTmp;

  //fprintf(stderr,"pre T'\t%s\n",thorfinn);
  ReversComplement(thorfinn);
  //fprintf(stderr,"post T'\t%s\n",thorfinn);
  res[2] = thorfinn; // rev strand
  ReversComplement(rasmus);
  res[3] = rasmus; // rev strand
  /*char RasmusCompRev[1024] = {0};
  char ThorfinnCompRev[1024] = {0};
  memset(RasmusCompRev, 0, sizeof RasmusCompRev);
  memset(ThorfinnCompRev, 0, sizeof ThorfinnCompRev);
  strcpy(RasmusCompRev,rasmus);
  strcpy(ThorfinnCompRev,thorfinn);
  ReversComplement(ThorfinnCompRev);
  res[2] = ThorfinnCompRev; // rev strand
  ReversComplement(RasmusCompRev);
  res[3] = RasmusCompRev; // rev strand*/
  
  //fprintf(stderr,"R and T'\t%s\n\t\t%s\n",res[0],res[2]);
  //fprintf(stderr,"T and R'\t%s\n\t\t%s\n",res[1],res[3]);
  return IsDeam;
}
  
#ifdef __WITH_MAIN__
//g++ Briggs2.cpp -D__WITH_MAIN__ mrand.o
int main(){
  int C_total = 0;int C_to_T_counter = 0;int C_to_T_counter_rev = 0;int C_total_rev=0;
  int G_total = 0;int G_to_A_counter = 0;int G_to_A_counter_rev = 0;int G_total_rev=0;

  int maxfraglength = 100;
  int seed = 300;
  mrand_t *mr = mrand_alloc(2,seed);

  size_t reads = 1000000;
  int modulovalue = 10;
  size_t moduloread = reads/modulovalue;

  for (int i = 0; i < reads; i++){
    if (i%moduloread == 0){
      fprintf(stderr,"SEQUENCE %d \n",i);
    }
    
    char original[1024];
    char **results = new char *[4];
    for(int i=0;i<4;i++){
      results[i] = new char[1024];
      memset(results[i],'\0',1024);
    }

    int flen; 
    flen = mrand_pop_long(mr) % maxfraglength;//mrand_pop_long(mr) % maxfraglength;
    if (flen < 30){
      flen = 35;
    }
    
    for(int i=0;i<flen;i++)
      original[i] = bass[mrand_pop_long(mr) %4];
    
    //fprintf(stderr,"FLEN IS %d\n",flen);
    //fprintf(stderr,"ori:    %s\n",original);
    //fprintf(stderr,"strlen IS %d\n",strlen(original));

    int strand = 0;
    //if (original[0] == 'C'){C_total++;};
    SimBriggsModel2(original,flen,0.024,0.36,0.68,0.0097,mr,results,strand,C_to_T_counter,G_to_A_counter,C_total,G_total);

    int pair = mrand_pop(mr)>0.5?0:1;
    
    /*if (pair == 0 && original[0] == 'C'){
      C_total++;
      if (results[pair][0] == 'T' || results[pair+2][0] == 'T'){
        C_to_T_counter++;
      }
    }
    if (pair == 1 && original[0] == 'G'){
      G_total++;
      if (results[pair][0] == 'A' || results[pair+2][0] == 'A'){
        G_to_A_counter++;
      }
    }*/

  }
  double C_Deam = (double)C_to_T_counter/(double)C_total;
  double G_Deam = (double)G_to_A_counter/(double)G_total;
  fprintf(stderr,"deamination C>T freq %f and G>A freq %f\n",C_Deam,G_Deam);
  //fprintf(stderr,"G total counter %d and G > A counter %d and deamination freq %f\n",G_total,G_to_A_counter,G_Deam);
  return 0;
}

#endif
//g++ Briggs2.cpp NGSNGS_misc.cpp -D__WITH_MAIN__ mrand.o fasta_sampler.o RandSampling.o ../htslib/libhts.a -std=c++11 -lz -lm -lbz2 -llzma -lpthread -lcurl -lcrypto -ggdb