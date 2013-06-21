
#include <stdlib.h>
#include <iostream>
#include "Online.h"

#define VERBOSITY 5
#define THRESHOLD 2
#define MAXLENGTH 6
#define GAPLIMIT 0

using namespace std;

int main(int argc, char * argv[]) {

  if ( argc != 6 ) {
    cerr << "Usage: " << argv[0] << " [option1_for_phrase] [option2_for_root] SynMap QueryFile OutputFile" << endl;
    cerr << "Option1:\n" ;
    cerr << "  1 			include unigram and multi word phrases\n";
    cerr << "  0			include only unigram\n"; 
    cerr << "Option2:\n" ;
    cerr << "  1 			include root for lattice \n";
    cerr << "  0			include no root\n"; 
    cerr << "For example: " << argv[0] << " 1 0 SynMap.txt topics.txt output.txt" << endl;
    return -1;     }

  char * pEnd;
  int phrase_in;
  phrase_in = strtol( argv[1], &pEnd, 10);
  if ( phrase_in > 1 || phrase_in < 0 ) {
    cerr << "Invalid option in argument 2: " << argv[1] << endl;
    exit(-1);
  }  

  int root_in;
  root_in = strtol( argv[2], &pEnd, 10);
  if ( phrase_in > 1 || phrase_in < 0 ) {
    cerr << "Invalid option in argument 3: " << argv[2] << endl;
    return -1; }  

  ifstream fin( argv[3] );
  if( !fin ) {
    cerr << "Can't open SynMap file " << argv[3] << endl;
    return -1; }

  ifstream fin2( argv[4] );
  if( !fin2 ) {
    cerr << "Can't open Query file " << argv[4] << endl;
    return -1; }

  ofstream fout( argv[5] );
  if( !fout ) {
    cerr << "Can't open output file " << argv[5] << endl;
    return -1; }

  Online aOnline( MAXLENGTH, VERBOSITY, phrase_in, root_in );
  aOnline.init( fin );
  aOnline.onlineQuerying( fin2, fout );
  return 0;
}
