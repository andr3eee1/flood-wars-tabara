#include <stdio.h>
#include "hash.h"

// program demonstrativ, citeste trei tipuri de comenzi:
// g <sir>       [afiseaza valoarea asociata sirului <sir>]
// s <sir> <val> [insereaza valoarea <val> asociata cu sirul <sir>]
// q             [iesire din program]
int main() {
  FILE *fin = stdin, *fout = stderr;
  char ch, s[1000];
  int val;

  init_cval();
  
  while ( (ch = fgetc( fin )) != 'q' ) {
    switch ( ch ) {
    case 's':
      fscanf( fin, "%s %d", s, &val );
      setVal( s, val );
      fprintf( fout, "Am asociat %d cu [%s]\n", val, s );
      break;
    case 'g':
      fscanf( fin, "%s", s );
      val = getVal( s );
      if ( val < 0 )
        fprintf( fout, "[%s] nu are asociere in tabela\n", s );
      else
        fprintf( fout, "%d este asociat cu [%s]\n", val, s );
      break;
    default:
      fprintf( fout, "Comanda necunoscuta: [%c]\n", ch );
    }
    fgetc( fin ); // finalul de linie
  }

  return 0;
}

/* 
   Exemplu de executie

g 123baa
[123baa] nu are asociere in tabela
s 123baa 100
Am asociat 100 cu [123baa]
g 123baa
100 este asociat cu [123baa]
g aba
[aba] nu are asociere in tabela
s aba 23
Am asociat 23 cu [aba]
g aba
23 este asociat cu [aba]
g 123baa
100 este asociat cu [123baa]
q

*/