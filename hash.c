/*
  Exemplu ilustrativ tabele hash:
  - Presupunem ca avem doar litere mici si cifre
  - Inlantuire perechi pe acelasi cod hash
  - Rule of thumb: nu umplem tabela cu mai mult de 80% din numarul de hashes
  - Exemplu doar cu insertii
  - Stergerile pot fi implementate cu pierderea spatiului ocupat de sir
*/ 

#define MAXN 100000 // 100k =~ 80% din 128k, nu umplem tabela mai mult
#define MAX_HASH (128 * 1024) // 128000 de liste
#define SUM_MAX_STR_LEN 1000000 // lungimea tuturor sirurilor inserate
#define NIL 0
#define NCHAR 128
#define NALPHA 26
#define SIGMA 36

char cval[NCHAR];

int first_free_str;
char string[SUM_MAX_STR_LEN]; // sirurile din tabela, una dupa alta

// exemplu de folosire gresita a lui struct - ocupa 4 intregi inloc de 3
// il vom folosi pentru claritatea codului
struct { 
  int first;  // indicele primului caracter in vectorul string[]
  int val;    // valoarea asociata sirului
  int next;   // urmatoarea pereche cu acelasi cod hash
} cell[MAXN]; // vector perechi (sir, valoare) + link la urmatoarea celula
int first_free_cell = 1;

int hash_index[MAX_HASH];

void init_cval() {
  int i;
  
  for ( i = 'a'; i <= 'z'; i++ )
    cval[i] = i - 'a';
  for ( i = '0'; i <= '9'; i++ )
    cval[i] = i - '0' + NALPHA;
}

// compara sirurile terminate cu zero, returneaza -1, 0 sau 1
int str_cmp( char s1[], char s2[] ) {
  int i = 0;

  while ( s1[i] && s2[i] && s1[i] == s2[i] )
    i++;

  return s1[i] < s2[i] ? -1 : s1[i] == s2[i] ? 0 : 1;
}

// adauga s[] la coada lui string[]
void str_add( char s[] ) {
  int i = 0;

  while ( s[i] )
    string[first_free_str++] = s[i++];

  string[first_free_str++] = '\0'; // copiem si finalul de sir
}

// calculam codul hash al lui s[]
// tratam s[] ca pe un numar mare in baza 256 si calculam s[] % MAX_HASH
int computeHash( char s[] ) {
  int i = 0;
  unsigned hash = 0;

  while ( s[i] ) // sirul se termina cu '\0'
    hash = hash * SIGMA + cval[s[i++]]; // adaugam la coada

  // modulo putere a lui 2
  return hash & (MAX_HASH - 1);
}

// returneaza celula asociata cu sirul s[] avand hash
// sau NIL daca s[] nu e gasit
int get_cell( char s[], int hash ) {
  int p = hash_index[hash];

  while ( p != NIL && str_cmp( &string[cell[p].first], s ) )
    p = cell[p].next;

  return p;
}

// returneaza valoarea asociata cu sirul s[] sau -1 daca s[] nu e gasit
int getVal( char s[] ) {
  int p = get_cell( s, computeHash( s ) );

  return p == NIL ? -1 : cell[p].val;
}

// asociaza valoarea 'val' cu sirul 's[]'
void setVal( char s[], int val ) {
  int hash, p = get_cell( s, hash = computeHash( s ) );

  if ( p == NIL ) { // valoare negasita in tabela, adaugam o celula
    p = first_free_cell++;           // alocam o noua celula
    cell[p].next = hash_index[hash]; // legam prima celula
    hash_index[hash] = p;            // memoram noul cap de lista

    // memoram sirul in celula
    cell[p].first = first_free_str; // inceputul sirului
    str_add( s ); // copiem s[]
  }

  // in acest moment avem o celula alocata in tabela ce contine s[]
  cell[p].val = val;
}