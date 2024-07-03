/*
   Nume program   : infinity-matrix.c
   Nume concurent : Pana Andrei, Danciu Traian, Teodorescu Cosmin, Hanganu Darius
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
//#include "hash.h"

#define MAXN 50
#define MAXM 50
#define INFINIT 2147483647
#define MAXDEPTH 100
#define MAXTIME 980000 // 0.98 sec
#define NCOADA 256
#define NDIR 4

// ponderii (pentru scor)
#define PONDER_MATERIAL 2500 
#define PONDER_DISTANTA 1
#define PONDER_FRONTIERA 1

struct timeval tv;
int cont;
long long tbase;
int maxdepth;

int killer[MAXDEPTH+1]; // killer moves

// timpul curent in microsecunde (milionime de secunda)
static inline long long checktime() {
  gettimeofday(&tv, NULL);
  return ((long long)tv.tv_sec) * 1000000 + tv.tv_usec;
}

char mat[MAXN+2][MAXM+2];
int n,m,scorej,scores;
char juc,mutch;
char viz[MAXN + 2][MAXN + 2];

char mut[]={'@','#','+','*','.'};

char atj,ats;

char int2char[5];
int char2int[127];

static inline int max(int a, int b){
  return a>b?a:b;
}

static inline int min(int a, int b){
  return a<b?a:b;
}

static inline int abs_(int x){
  return (x<0?-x:x);
}

struct Coada {
  int prim, ultim;
  char coadal[NCOADA], coadac[NCOADA];
};

struct Scor {
  int arie, dist, frontl, frontc;
  struct Coada frontiera;
} scor[2];

int dlin[NDIR] = {-1, 0, 1, 0}, dcol[NDIR] = {0, 1, 0, -1};

// facem mutare in color pentru jucatorul juc
void makeMove(int color, int juc) {
  int ramase, i, l, c, dir, lnou, cnou;

  ramase = 0; // numaram numarul de celule noi neprocesate
  for(i = scor[(int)juc].frontiera.prim; i != scor[(int)juc].frontiera.ultim; i = (i + 1) % NCOADA) {
    if(mat[(int)scor[(int)juc].frontiera.coadal[i]][(int)scor[(int)juc].frontiera.coadac[i]] == color) {
      ramase++;
    }
  }

  if(juc == 0) {
    while(ramase > 0) { // cat timp mai avem celule noi neprocesate
      // luam prima pozitie din coada
      l = scor[juc].frontiera.coadal[scor[juc].frontiera.prim];
      c = scor[juc].frontiera.coadac[scor[juc].frontiera.prim];
      scor[juc].frontiera.prim = (scor[juc].frontiera.prim + 1) % NCOADA; // avansam in coada

      //printf("%d %d: %c %c %d\n",l,c,mut[color], mut[(int)mat[l][c]], ramase);
      //printf("%d %d\n",scor[juc].frontiera.prim,scor[juc].frontiera.ultim);
      if(mat[l][c] == color) {
        ramase--; // am mai procesat o culoare

        // actualizam scorurile
        scor[0].arie++;
        scor[0].dist = min(scor[0].dist, l - 1 + m - c);
        scor[0].frontl = max(scor[0].frontl, n + 1 - l);
        scor[0].frontc = max(scor[0].frontc, c);

        for(dir = 0; dir < NDIR; dir++) {
          lnou = l + dlin[dir];
          cnou = c + dcol[dir];
          if(viz[lnou][cnou] == 0) {
            viz[lnou][cnou] = 1;

            // adaugam noua pozitie in coada
            scor[juc].frontiera.coadal[scor[juc].frontiera.ultim] = lnou;
            scor[juc].frontiera.coadac[scor[juc].frontiera.ultim] = cnou;
            scor[juc].frontiera.ultim = (scor[juc].frontiera.ultim + 1) % NCOADA;

            if(mat[lnou][cnou] == color) {
              ramase++; // avem o noua pozitie de procesat
        //      printf("lol\n");
            }
          }
        }
      } else { // nu este buna asa ca o adaugam inapoi in coada
        scor[juc].frontiera.coadal[scor[juc].frontiera.ultim] = l;
        scor[juc].frontiera.coadac[scor[juc].frontiera.ultim] = c;
        scor[juc].frontiera.ultim = (scor[juc].frontiera.ultim + 1) % NCOADA;
      }
    }
  } else {
    while(ramase > 0) { // cat timp mai avem celule noi neprocesate
      // luam prima pozitie din coada
      l = scor[juc].frontiera.coadal[scor[juc].frontiera.prim];
      c = scor[juc].frontiera.coadac[scor[juc].frontiera.prim];
      scor[juc].frontiera.prim = (scor[juc].frontiera.prim + 1) % NCOADA; // avansam in coada

  //    printf("%d %d: %c %c %d\n",l,c,mut[color], mut[(int)mat[l][c]], ramase);
    //  printf("%d %d\n",scor[juc].frontiera.prim,scor[juc].frontiera.ultim);
      if(mat[l][c] == color) {
        ramase--; // am mai procesat o culoare

        // actualizam scorurile
        scor[1].arie++;
        scor[1].dist = min(scor[1].dist, n - l + c - 1);
        scor[0].frontl = max(scor[0].frontl, l);
        scor[0].frontc = max(scor[0].frontc, m + 1 - c);

        for(dir = 0; dir < NDIR; dir++) {
          lnou = l + dlin[dir];
          cnou = c + dcol[dir];
          if(viz[lnou][cnou] == 0) {
            viz[lnou][cnou] = 1;

            // adaugam noua pozitie in coada
            scor[juc].frontiera.coadal[scor[juc].frontiera.ultim] = lnou;
            scor[juc].frontiera.coadac[scor[juc].frontiera.ultim] = cnou;
            scor[juc].frontiera.ultim = (scor[juc].frontiera.ultim + 1) % NCOADA;

            if(mat[lnou][cnou] == color) {
              ramase++; // avem o noua pozitie de procesat
      //        printf("lol\n");
            }
          }
        }
      } else { // nu este buna asa ca o adaugam inapoi in coada
        scor[juc].frontiera.coadal[scor[juc].frontiera.ultim] = l;
        scor[juc].frontiera.coadac[scor[juc].frontiera.ultim] = c;
        scor[juc].frontiera.ultim = (scor[juc].frontiera.ultim + 1) % NCOADA;
      }
    }
  }
  //printf("\n");
}

// evaluarea statica a tablei
int evalStatic(int depth) {
  return PONDER_MATERIAL * (scor[0].arie - scor[1].arie) + // scorul material
         PONDER_DISTANTA * (scor[0].dist - scor[1].dist) + // distanta pana la adversar
         PONDER_FRONTIERA * ((n - scor[0].frontl + 1) * scor[0].frontc -
                             scor[1].frontl * (m - scor[1].frontc + 1)); // si frontiera de incadrare
}

int negamax(int depth,int alpha,int beta){
  int icolor,score, startKiller, i;
  struct Scor oldscor;

  if(maxdepth-depth==5){
    cont=((checktime()-tbase)<MAXTIME);
  }

  if(cont&&depth==maxdepth){
    return (1 - 2 * ((depth % 2) ^ juc)) * evalStatic(depth);
  }

  oldscor = scor[(int)juc];

  startKiller = killer[depth];
  if(cont&&killer[depth]>=0){
    icolor=killer[depth];
    if(icolor!=mat[n][1]&&icolor!=mat[1][m]){
      makeMove(icolor, juc ^ (depth % 2)); // juc pentru par si juc^1 pentru impar
      
      score=-negamax(depth+1,-beta,-alpha);

      if(score>alpha){
        alpha=score;
      }

      for(i = scor[(int)juc].frontiera.prim; i != scor[(int)juc].frontiera.ultim; i = (i + 1) % NCOADA) {
        viz[(int)scor[(int)juc].frontiera.coadal[i]][(int)scor[(int)juc].frontiera.coadac[i]] = 0;
      }
      scor[(int)juc] = oldscor;
    }
  }

  icolor=0;
  while(cont&&alpha<beta&&icolor<5){
    if(icolor!=startKiller&&icolor!=mat[n][1]&&icolor!=mat[1][m]){
      makeMove(icolor, juc ^ (depth % 2)); // juc pentru par si juc^1 pentru impar

      score=-negamax(depth+1,-beta,-alpha);

      if(score>alpha){
        alpha=score;
        killer[depth]=icolor;
      }

      // avand in vedere ca lenea ma domina, nu mai creez o variabila noua
      for(i = scor[(int)juc].frontiera.prim; i != scor[(int)juc].frontiera.ultim; i = (i + 1) % NCOADA) {
        viz[(int)scor[(int)juc].frontiera.coadal[i]][(int)scor[(int)juc].frontiera.coadac[i]] = 0;
      }
      scor[(int)juc] = oldscor;
    }
    icolor++;
  }

  return alpha<beta?alpha:beta;
}

// fill care schimba toate pozitiile cu culoarea vechi in nou
void fillMutare(int l, int c, int vechi, int nou) {
  int dir;

  mat[l][c] = nou;
  for(dir = 0; dir < NDIR; dir++) {
    if(mat[l + dlin[dir]][c + dcol[dir]] == vechi) { // daca e de schimbat 
      fillMutare(l + dlin[dir], c + dcol[dir], vechi, nou);
    }
  }
}

int main(){
  int l,c,maxicolor;
  char ch;

  tbase = checktime();

  for(l=0;l<5;l++){
    char2int[(int)mut[l]]=l;
    int2char[l]=mut[l];
  }

  juc = (fgetc(stdin) == 'J' ? 0 : 1);
  fgetc(stdin);//'\n'

  // citire matrice
  n=1;
  ch=fgetc(stdin);
  while(!feof(stdin)){
    m=1;
    while(ch!='\n'){
      mat[n][m++]=char2int[(int)ch];
      ch=fgetc(stdin);
    }
    ch=fgetc(stdin);
    n++;
  }
  n--;
  m--;

  // bordare matrice
  for(l=0;l<=n+1;l++){
    mat[l][0]=mat[l][m+1]=-1;
    viz[l][0] = viz[l][m + 1] = 1;
  }
  for(c=0;c<=m+1;c++){
    mat[0][c]=mat[n+1][c]=-1;
    viz[0][c] = viz[n + 1][c] = 1;
  }

  // initializari frontiera si scoruri
  scor[0].dist = scor[1].dist = n + m - 2;
  scor[0].frontl = scor[0].frontc = scor[1].frontl = scor[1].frontc = 1;
  scor[0].frontiera.prim = scor[1].frontiera.prim = 0;
  scor[0].frontiera.ultim = scor[1].frontiera.ultim = 1;
  scor[0].frontiera.coadal[0] = n;
  scor[0].frontiera.coadac[0] = 1;
  scor[1].frontiera.coadal[0] = 1;
  scor[1].frontiera.coadac[0] = m;
  makeMove(mat[n][1], 0);
  makeMove(mat[1][m], 1);
  viz[n][1] = viz[1][m] = 1;

  // resetare killermove
  for(l=0;l<MAXDEPTH;l++){
    killer[l]=-1;
  }

  maxicolor=100;//ceva fictional, pentru debug
  cont=maxdepth=1;
  while((checktime()-tbase)<MAXTIME&&maxdepth<=MAXDEPTH){
    negamax(0,-INFINIT,INFINIT);
    maxdepth++;
    if(cont){
      maxicolor=killer[0];
    }
  }

  // printf("%d\n", maxdepth);

  // mutarea finala
  if(juc == 0) {
    fillMutare(n, 1, mat[n][1], maxicolor);
  } else {
    fillMutare(1, m, mat[1][m], maxicolor);
  }

  // afisare tabla
  fputc(juc == 0 ? 'S' : 'J', stdout);
  fputc('\n',stdout);
  for(l=1;l<=n;l++){
    for(c=1;c<=m;c++){
      fputc(int2char[(int)mat[l][c]],stdout);
    }
    fputc('\n',stdout);
  }

  return 0;
}
/*
J
.#+#.*.@.@
@.+*@.+*#+
**#++@**#@
#@#@.@@+@#
++@++@#.@.
**##*@*#++
#@@*.+*.*+
*/
