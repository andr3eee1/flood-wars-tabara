/*
   Nume program   : komodo.c
   Nume concurent : Boeriu Horia Andrei, Vasile Radu
   E-mail         : haboeriu@gmail.com, raduvasile810@gmail.com
*/
// dp penal 3
#include <ctype.h>
#include <stdio.h>
#include <sys/time.h>
#define INFINIT 2000000000
#define MAXTIME 990000
#define P1 (200 * 200)
#define P2 1
#define P3 200
#define MAXID 3500
#define MAX 52
#define NR 5
#define PC 4
#define MAXL 76
#define NIL 0
int next1[MAX * MAX + 1], prev1[MAX * MAX + 1];//aici tin frontiera de jos
int next2[MAX * MAX + 1], prev2[MAX * MAX + 1];//aici tin frontiera de sus
int frloc[MAXID + 1];//1 daca e jos, 2 daca e sus
int prl[MAXID + 1], next[MAX * MAX + 1], frnt[MAX + 2][MAX + 2];//o sa tin frontiera de la fiecare id
int scor[MAXL];
char opp['S' + 1];
int lungj, lungs;


int punctaj[NR];

//la pc tin extremitatea din fiecare parte din fiecare id
int aprj[MAXID + 1], aprs[MAXID + 1];
int jos[PC][2], sus[PC][2];//tin frontierele din toate laturile
int perimetru[MAXID + 1], mr2[MAX + 2][MAX + 2], pc[PC][MAXID + 1];//in mr2 este copie pt id-uri
int frid[MAXID + 1];//frecventa pentru iduri cand calculez aria
char mr[MAX + 2][MAX + 2];
char vp[] = {'.', '#', '+', '*', '@'}, fvp[MAXID];
int fr[MAX + 2][MAX + 2];
int fr1[MAX + 2][MAX + 2];
int cl[(MAX + 2) * (MAX + 2)], cc[(MAX + 2) * (MAX + 2)];
int dl[] = {-1, 0, 0, 1};
int dc[] = {0, -1, 1, 0};
int killer[MAXL];
int mutari[MAXL][NR];
int n, m, ul, pr, arie2, depthorig, maxarie1 = -INFINIT, id, ariej, aries, distj, dists, pas, culj, culs;
char jucorig, rez;//jucatorul original
struct timeval tv;
long long tbase;
int cont;
// verificare timp ( pentru iterative deepening )
static inline long long checktime() {
  gettimeofday(&tv, NULL);
  return ((long long)tv.tv_sec) * 1000000 + tv.tv_usec;
}
// functie de minim
static inline int min(int a, int b) {
  return a < b ? a : b;
}
// modulul lui a
int mod(int a) {
    return a < 0 ? -a : a;
}
// distanta dintre 2 puncte manhattan
int distman(int l3, int c3, int l4, int c4) {
  return mod(l3 - l4) + mod(c3 - c4);
}
// fill pentru id-uri, pentru calculul incremental al ariei
void fillid(int l, int c, char ch) {
  int i, c1, l1;
  pr = 0;
  ul = 1;
  cl[pr] = l;
  cc[pr] = c;
  mr2[l][c] = -id;
  perimetru[id] = 1;
  while (pr != ul) {
    l = cl[pr];
    c = cc[pr];
    pr++;
    for (i = 0; i < PC; i++) {
      l1 = l + dl[i];
      c1 = c + dc[i];
      if (mr2[l1][c1] == ch) {
        mr2[l1][c1] = -id;
        perimetru[id]++;
        cl[ul] = l1;
        cc[ul++] = c1;
      } else if (frnt[l][c] == 0 && mr2[l1][c1] != -id) {//l, c face parte din frontiera acestui id
        next[m * (l - 1) + c] = prl[id];
        prl[id] = m * (l - 1) + c;
        frnt[l][c] = 1;
      }
    }
  }
}

void fill(int i, int l, int c, int next3[], int prev3[], short mrc[NR][MAX * MAX + 1], short vnr[]) {
  //avem lista cu vecinii de fiecare culoare
  //i este numarul culorii in care schimbam
  ///fputc(vp[i], stdout);
  ///fputc(' ', stdout);
  int c1, l1, l2, j;
  l2 = l;
  for (j = 0; j < vnr[i]; j++) {
    l1 = (mrc[i][j] - 1) / m + 1;
    c1 = (mrc[i][j] - 1) % m + 1;
    if (frloc[mr2[l1][c1]] != 1 && frloc[mr2[l1][c1]] != 2) {
      if (l2 == 1) {
        frloc[mr2[l1][c1]] = 2;
        if (aprj[mr2[l1][c1]] < dists) {
          dists = aprj[mr2[l1][c1]];
        }
        aries += perimetru[mr2[l1][c1]];
      } else {
        frloc[mr2[l1][c1]] = 1;
        if (aprs[mr2[l1][c1]] < distj) {
          distj = aprs[mr2[l1][c1]];
        }
        ariej += perimetru[mr2[l1][c1]];
      }
    }
    //adaug l1, c1 la frontiera
    prev3[next3[0]] = mrc[i][j];
    next3[mrc[i][j]] = next3[0];
    prev3[mrc[i][j]] = 0;
    next3[0] = mrc[i][j];
  }
}

void fill2(char ch, char ch1, int l, int c) {
  // facem fill cu lee deoarece e mai eficient
  // schimba toate characterele egale cu ch de langa l, c in ch1 si pune 1 in fr
  int i, c1, l1;
  for (l1 = 1; l1 <= n; l1++) {
    for (c1 = 1; c1 <= m; c1++) {
      fr[l1][c1] = 0;
    }
  }
  pr = 0;
  ul = 1;
  cl[pr] = l;
  cc[pr] = c;
  mr[l][c] = ch1;
  fr[l][c] = 1;
  while (pr != ul) {
    l = cl[pr];
    c = cc[pr++];
    for (i = 0; i < PC; i++) {
      l1 = l + dl[i];
      c1 = c + dc[i];
      if (mr[l1][c1] == ch && fr[l1][c1] == 0) {
        fr[l1][c1] = 1;
        cl[ul] = l1;
        cc[ul++] = c1;
        mr[l1][c1] = ch1;
      }
    }
  }
}

int dist;
int analize(char juc) {
  // o avem deja calculata
  /*int l, c;
  for (l = 1; l <= n; l++) {
    for (c = 1; c <= m; c++) {
      printf("%d ", frloc[mr2[l][c]]);
    }
    printf("\n");
  }
  printf("\n");*/
  if(juc == 'S')
    return (aries - ariej) * P1 + (lungs - lungj) * P2 + (distj - dists) * P3;
  return (ariej - aries) * P1 + (lungj - lungs) * P2 + (dists - distj) * P3;
}

//in frloc[mr2[l][c]] tin 1 daca e jos, 2 daca e sus
int alphabeta(int depth, char juc, int alpha, int beta) {
  short clista[MAX * MAX + 1];
  short mrc[NR][MAX * MAX + 1], vnr[NR];//vecinii de fiecare culoare
  char frecv[NR];
  int arie, maxarie, minarie, i, l, c, cariej, caries, cdistj, cdists, lung, l1, c1, cul, nrv, x, x1, j, cculj, cculs, clungj, clungs;
  if(depthorig - depth == 5)
    cont = (checktime() - tbase) < MAXTIME;
  if(cont && depth == depthorig) {
    //fputc(jucorig, stdout);
    //fputc(' ', stdout);
    //printf("%d %c\n", analize(jucorig), culs);
    return analize(jucorig);
  }
  //in loc de fill o sa tin o lista cu frontiera noastra(adc punctele noastre de pe margine)
  //si o copie la ea, asa cum faceam cu matricea inainte
  //ar trebui sa fie mult mai rapid
  cariej = ariej;
  caries = aries;
  cdistj = distj;
  cdists = dists;
  cculj = culj;
  cculs = culs;
  clungj = lungj;
  clungs = lungs;
  ///printf("%d ", pas);
  if (cont && juc == 'J') {//construiesc lista de vecini si copia frontierei
    lung = 0;
    x = next1[0];//x este o linie si coloana
    for (i = 0; i < NR; i++) {
      vnr[i] = frecv[i] = 0;
    }
    pas++;
    ///printf("\n");
    while (x != NIL) {
      l = (x - 1) / m + 1;
      c = (x - 1) % m + 1;
      ///printf("%d %d\n", l, c);
      //acum l, c face parte din frontiera si ii testez vecinii ca sa ii pun in mrc
      //mrc era vectorul pentru fiecare culoare cu vecinii liberi
      nrv = 0;
      for (i = 0; i < PC; i++) {
        l1 = l + dl[i];
        c1 = c + dc[i];
        if (frloc[mr2[l1][c1]] <= 0 && frloc[mr2[l1][c1]] != -pas) {///daca nu apare in frontiera si nici ca si vecin
          ///printf("%d %d\n", l1, c1);
          frloc[mr2[l1][c1]] = -pas;
          nrv++;
          cul = fvp[mr[l1][c1]];
          //punem ca si vecinii de aceasta culoare toata frontiera acestui bloc
          //si dupa o sa punem ca frontiera noastra
          x1 = prl[mr2[l1][c1]];
          while (x1 != NIL) {
            mrc[cul][vnr[cul]] = x1;
            x1 = next[x1];
            vnr[cul]++;
          }
        } else if (frloc[mr2[l1][c1]] == -pas) {
          nrv++;
        }
      }
      if (nrv == 0) {//nu mai este la frontiera deci o scot din aceasta lista
        prev1[next1[x]] = prev1[x];
        next1[prev1[x]] = next1[x];
      } else {
        clista[lung] = x;
        lung++;
      }
      x = next1[x];
    }
    lungj = lung;
  } else if(cont){
    lung = 0;
    x = next2[0];//x este o linie si coloana
    for (i = 0; i < NR; i++) {
      vnr[i] = frecv[i] = 0;
    }
    pas++;
    while (x != NIL) {
      l = (x - 1) / m + 1;
      c = (x - 1) % m + 1;
      //acum l, c face parte din frontiera si ii testez vecinii ca sa ii pun in mrc
      //mrc era vectorul pentru fiecare culoare cu vecinii liberi
      nrv = 0;
      for (i = 0; i < PC; i++) {
        l1 = l + dl[i];
        c1 = c + dc[i];
        if (frloc[mr2[l1][c1]] <= 0 && frloc[mr2[l1][c1]] != -pas) {///daca nu apare in frontiera si nici ca si vecin
          frloc[mr2[l1][c1]] = -pas;
          nrv++;
          cul = fvp[mr[l1][c1]];
          //punem ca si vecinii de aceasta culoare toata frontiera acestui bloc
          //si dupa o sa punem ca frontiera noastra
          x1 = prl[mr2[l1][c1]];
          while (x1 != NIL) {
            mrc[cul][vnr[cul]] = x1;
            x1 = next[x1];
            vnr[cul]++;
          }
        } else if (frloc[mr2[l1][c1]] == -pas) {
          nrv++;
        }
      }
      if (nrv == 0) {//nu mai este la frontiera deci o scot din aceasta lista
        prev2[next2[x]] = prev2[x];
        next2[prev2[x]] = next2[x];
      } else {
        clista[lung] = x;
        lung++;
      }
      x = next2[x];
    }
    lungs = lung;
  }

  if( cont && juc == jucorig ) { // nu a trecut timpul ?
    ///if (depth == 0 && depthorig == 19) {
      ///printf("alpha = %d beta = %d %d\n", alpha, beta, cont);
    ///}
    maxarie = -INFINIT;
    if (cont && killer[depth] >= 0 && alpha < beta) {
      i = killer[depth];
      if (culj != vp[i] && culs != vp[i]) {
        frecv[i] = 1;
        if (juc == 'J') {
          culj = vp[i];
          fill(i, n, 1, next1, prev1, mrc, vnr); // punem mutare
          arie = alphabeta(depth + 1, 'S', alpha, beta);
        } else {
          culs = vp[i];
          fill(i, 1, m, next2, prev2, mrc, vnr); // punem mutare
          arie = alphabeta(depth + 1, 'J', alpha, beta);
        }
        /*if (depth == 0) {
          punctaj[i] = arie;
          ///printf("lol%c %c\n\n", culj, culs);
        }*/
        if (arie > maxarie) {
          maxarie = arie;
        }
        if(arie > alpha) {
          alpha = arie;
          scor[depth] = arie;
        }
        //printf("%d ", arie);
        //scot din frloc la vecinii de aceasta culoare si acum nu vor mai face parte din frontiera
        for (j = 0; j < vnr[i]; j++) {
          l1 = (mrc[i][j] - 1) / m + 1;
          c1 = (mrc[i][j] - 1) % m + 1;
          frloc[mr2[l1][c1]] = 0;
        }
        //acum reconstituiesc lista cu frontiera veche
        if (juc == 'J') {
          x1 = 0;
          for (j = 0; j < lung; j++) {
            next1[x1] = clista[j];
            prev1[clista[j]] = x1;
            x1 = clista[j];
          }
          next1[x1] = NIL;
        } else {
          x1 = 0;
          for (j = 0; j < lung; j++) {
            next2[x1] = clista[j];
            prev2[clista[j]] = x1;
            x1 = clista[j];
          }
          next2[x1] = NIL;
        }
      }
      ariej = cariej;
      aries = caries;
      distj = cdistj;
      dists = cdists;
      culj = cculj;
      culs = cculs;
      lungj = clungj;
      lungs = clungs;
    }
    /*if (depth == 0 && depthorig == 19) {
      printf("alpha = %d beta = %d %d\n", alpha, beta, cont);
    }*/
    i = 0;
    while(cont && i < NR && alpha < beta) {
      if (culj != vp[i] && culs != vp[i] && killer[depth] != i && frecv[i] == 0) {
        frecv[i] = 1;
        if (juc == 'J') {
          culj = vp[i];
          fill(i, n, 1, next1, prev1, mrc, vnr); // punem mutare
          arie = alphabeta(depth + 1, 'S', alpha, beta);
        } else {
          culs = vp[i];
          fill(i, 1, m, next2, prev2, mrc, vnr); // punem mutare
          arie = alphabeta(depth + 1, 'J', alpha, beta);
        }
        /*if (depth == 0) {
          punctaj[i] = arie;
          ///printf("lol");
          ///printf("%c %c\n\n", culj, culs);
        }*/
        if (arie > maxarie) {
          maxarie = arie;
        }
        if(arie > alpha) {
          alpha = arie;
          //printf("%d ", i);
          killer[depth] = i;
          scor[depth] = arie;
        }
        //printf("%d ", arie);
        //scot din frloc la vecinii de aceasta culoare si acum nu vor mai face parte din frontiera
        for (j = 0; j < vnr[i]; j++) {
          l1 = (mrc[i][j] - 1) / m + 1;
          c1 = (mrc[i][j] - 1) % m + 1;
          frloc[mr2[l1][c1]] = 0;
        }
        //acum reconstituiesc lista cu frontiera veche
        if (juc == 'J') {
          x1 = 0;
          for (j = 0; j < lung; j++) {
            next1[x1] = clista[j];
            prev1[clista[j]] = x1;
            x1 = clista[j];
          }
          next1[x1] = NIL;
        } else {
          x1 = 0;
          for (j = 0; j < lung; j++) {
            next2[x1] = clista[j];
            prev2[clista[j]] = x1;
            x1 = clista[j];
          }
          next2[x1] = NIL;
        }
      }
      i++;
      ariej = cariej;
      aries = caries;
      distj = cdistj;
      dists = cdists;
      culj = cculj;
      culs = cculs;
      lungj = clungj;
      lungs = clungs;
    }
    return maxarie;
  }
  else{
    minarie = INFINIT;
    if(cont && killer[depth] >= 0 && alpha < beta) {
      i = killer[depth];
      if (culj != vp[i] && culs != vp[i]) {
        frecv[i] = 1;
        if (juc == 'J') {
          culj = vp[i];
          fill(i, n, 1, next1, prev1, mrc, vnr); // punem mutare
          arie = alphabeta(depth + 1, 'S', alpha, beta);
        } else {
          culs = vp[i];
          fill(i, 1, m, next2, prev2, mrc, vnr); // punem mutare
          arie = alphabeta(depth + 1, 'J', alpha, beta);
        }
        if (arie < minarie) {
          minarie = arie;
        }
        if (arie < beta) {
          beta = arie;
          scor[depth] = arie;
        }
        //scot din frloc la vecinii de aceasta culoare si acum nu vor mai face parte din frontiera
        for (j = 0; j < vnr[i]; j++) {
          l1 = (mrc[i][j] - 1) / m + 1;
          c1 = (mrc[i][j] - 1) % m + 1;
          frloc[mr2[l1][c1]] = 0;
        }
        //acum reconstituiesc lista cu frontiera veche
        if (juc == 'J') {
          x1 = 0;
          for (j = 0; j < lung; j++) {
            next1[x1] = clista[j];
            prev1[clista[j]] = x1;
            x1 = clista[j];
          }
          next1[x1] = NIL;
        } else {
          x1 = 0;
          for (j = 0; j < lung; j++) {
            next2[x1] = clista[j];
            prev2[clista[j]] = x1;
            x1 = clista[j];
          }
          next2[x1] = NIL;
        }
      }
      ariej = cariej;
      aries = caries;
      distj = cdistj;
      dists = cdists;
      culj = cculj;
      culs = cculs;
      lungj = clungj;
      lungs = clungs;
    }
    i = 0;
    while(cont && i < NR && alpha < beta) {
      if (culj != vp[i] && culs != vp[i] && i != killer[depth] && frecv[i] == 0) {
        frecv[i] = 1;
        if (juc == 'J') {
          culj = vp[i];
          fill(i, n, 1, next1, prev1, mrc, vnr); // punem mutare
          arie = alphabeta(depth + 1, 'S', alpha, beta);

        } else {
          culs = vp[i];
          fill(i, 1, m, next2, prev2, mrc, vnr); // punem mutare
          arie = alphabeta(depth + 1, 'J', alpha, beta);
        }
        if (arie < minarie) {
          minarie = arie;
        }
        if (arie < beta) {
          beta = arie;
          killer[depth] = i;
          scor[depth] = arie;
        }
        //scot din frloc la vecinii de aceasta culoare si acum nu vor mai face parte din frontiera
        for (j = 0; j < vnr[i]; j++) {
          l1 = (mrc[i][j] - 1) / m + 1;
          c1 = (mrc[i][j] - 1) % m + 1;
          frloc[mr2[l1][c1]] = 0;
        }
        //acum reconstituiesc lista cu frontiera veche
        if (juc == 'J') {
          x1 = 0;
          for (j = 0; j < lung; j++) {
            next1[x1] = clista[j];
            prev1[clista[j]] = x1;
            x1 = clista[j];
          }
          next1[x1] = NIL;
        } else {
          x1 = 0;
          for (j = 0; j < lung; j++) {
            next2[x1] = clista[j];
            prev2[clista[j]] = x1;
            x1 = clista[j];
          }
          next2[x1] = NIL;
        }
      }
      i++;
      ariej = cariej;
      aries = caries;
      distj = cdistj;
      dists = cdists;
      culj = cculj;
      culs = cculs;
      lungj = clungj;
      lungs = clungs;
    }
    return minarie;
  }
}
int main() {
  opp['J'] = 'S';
  opp['S'] = 'J';
  tbase = checktime();
  char ch, juc, ch1;
  int l, c, i, x, x1;
  juc = fgetc(stdin);
  jucorig = juc;
  fgetc(stdin);
  n = m = 1;
  ch = fgetc(stdin);
  // citire
  while (ch == '.' || ch == '#' || ch == '+' || ch == '*' || ch == '@') {
    m = 1;
    while (ch != '\n') {
      mr[n][m] = mr2[n][m] = ch;
      m++;
      ch = fgetc(stdin);
    }
    ch = fgetc(stdin);
    n++;
  }
  n--;
  m--;
  //acum calcuez aria fiecarei bucati de egale ca sa aflu aria la analiza in O(1)
  //in mr2 va fi id-ul respectiv si in perimetru[id] va fi aria id-ului
  id = 1;
  for (l = 1; l <= n; l++) {
    for (c = 1; c <= m; c++) {
      if (mr2[l][c] >= 0) {
        fillid(l, c, mr2[l][c]);
        id++;
      }
    }
  }
  id--;
  for (l = 0; l <= id; l++) {
    aprj[l] = aprs[l] = INFINIT;
  }
  for (l = 1; l <= n; l++) {
    for (c = 1; c <= m; c++) {
      mr2[l][c] = -mr2[l][c];
      if (distman(l, c, n, 1) < aprj[mr2[l][c]]) {
        aprj[mr2[l][c]] = distman(l, c, n, 1);
      }
      if (distman(l, c, 1, m) < aprs[mr2[l][c]]) {
        aprs[mr2[l][c]] = distman(l, c, 1, m);
      }
      if (l < pc[0][mr2[l][c]]) {
        pc[0][mr2[l][c]] = l;
      }
      if (c < pc[1][mr2[l][c]]) {
        pc[1][mr2[l][c]] = c;
      }
      if (c > pc[2][mr2[l][c]]) {
        pc[2][mr2[l][c]] = c;
      }
      if (l > pc[3][mr2[l][c]]) {
        pc[3][mr2[l][c]] = l;
      }
    }
  }
  for (i = 0; i < NR; i++) {
    fvp[vp[i]] = i;
  }
  //astea sunt ariile initiale
  //bored matricea mr2 cu MAXID - 1 si  pun in frloc[MAXID - 1] = 3;
  frloc[MAXID - 1] = 3;
  for (l = 0; l <= n + 1; l++) {
    mr2[l][0] = mr2[l][m + 1] = MAXID - 1;
  }
  for (c = 0; c <= m + 1; c++) {
    mr2[0][c] = mr2[n + 1][c] = MAXID - 1;
  }
  //ariej este aria de jos si aries este aria de sus
  ariej = perimetru[mr2[n][1]];
  aries = perimetru[mr2[1][m]];
  distj = aprs[mr2[n][1]];
  dists = aprj[mr2[1][m]];
  culj = mr[n][1];
  culs = mr[1][m];
  frloc[mr2[n][1]] = 1;
  frloc[mr2[1][m]] = 2;
  //pun frontiera de jos

  x1 = prl[mr2[n][1]];
  x = lungj = 0;
  while (x1 != NIL) {
    next1[x] = x1;
    prev1[x1] = x;
    x = x1;
    x1 = next[x1];
    lungj++;
  }
  //pun frontiera de sus
  x1 = prl[mr2[1][m]];
  x = lungs = 0;
  while (x1 != NIL) {
    next2[x] = x1;
    prev2[x1] = x;
    x = x1;
    x1 = next[x1];
    lungs++;
  }
  for( i = 0; i < MAXL; i++ ){
    killer[i] = -1;

  }
  for (i = 0; i < NR; i++) {
    punctaj[i] = -INFINIT;
  }
  cont = depthorig = 1;
  depthorig = 1;
  // iterative deepening
  while(((checktime() - tbase) < MAXTIME) && (depthorig <= MAXL)) {
    /*for (l = 1; l <= n; l++) {
      for (c = 1; c <= m; c++) {
        printf("%d ", frloc[mr2[l][c]]);
      }
      printf("\n");
    }
    printf("\n");
    //afiseaza dupa ceva timp o frontiera gresita, sa o copiez la sf lui alphabeta toata frontiera cu copia aia clista
    x1 = next1[0];
    while (x1 != NIL) {
      printf("%d %d, la prev %d %d\n", (x1 - 1) / m + 1, (x1 - 1) % m + 1, (prev1[x1] - 1) / m + 1, (prev1[x1] - 1) % m + 1);
      x1 = next1[x1];
    }
    printf("\n");*/
    alphabeta(0, juc, -INFINIT, INFINIT);

    if ( cont ) {
      /*fputc(vp[killer[0]], stdout);
      fputc(' ', stdout);
      printf("%d %d\n", depthorig, pas);
      for (i = 0; i < NR; i++) {
        printf("%c %d\n", vp[i], punctaj[i]);
        punctaj[i] = -INFINIT;
      }
      printf("\n");*/
      depthorig++;
      ch1 = vp[killer[0]]; // o retinem daca nu a fost timeout
    }
  }

  //for( i = 0; i < MAXL; i++ )
  //printf("%d ", killer[i]);
  ///printf("%d %d\n", depthorig, pas);
  if (juc == 'J') {
    fill2(mr[n][1], ch1, n, 1);
    fputc('S', stdout);
  } else {
    fill2(mr[1][m], ch1, 1, m);
    fputc('J', stdout);
  }
  fputc('\n', stdout);
  for (l = 1; l <= n; l++) {
    for (c = 1; c <= m; c++) {
      fputc(mr[l][c], stdout);
    }
    fputc('\n', stdout);
  }
  // Komodo dragon pattern
  return 0;
}
