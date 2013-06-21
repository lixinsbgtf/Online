
#ifndef _DEFS_H_
#define _DEFS_H_

#include <string>

using namespace std;

#include <string>

/*
struct sequence { 
  string seq;
  int size;
};*/

struct entry {
  string docid;
  //long offset;
  double magnitude;
  long size;
  entry(){};

  entry( string t_doc, /*long t_offset,*/ double t_mag, long t_size )
	:docid( t_doc ),
	 //offset( t_offset ),
	 magnitude( t_mag ),
	 size( t_size ){};
};

struct posting {
  long did;
  long fre;
};

struct ngramInfo {
  string ngram;
  long offset;
  ngramInfo( string t_ngram, long t_offset )
	:ngram( t_ngram ),
	 offset( t_offset ){};
  ngramInfo(){};
};

struct phrase_root {
  string phrase;
  long root;
  phrase_root( const string& t_ph, const long& t_root)
        :phrase( t_ph ),
         root( t_root ){};
};

#endif
