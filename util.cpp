/*
 *  util.cpp
 *  
 *
 *  Created by Xin Li and Fei Song on 11-08-11.
 *  Copyright 2011 University of Guelph. All rights reserved.
 *
 */

#include "util.h"
/*
bool onString( char ch, string word ) {
  for( unsigned int i = 0; i < word.size(); i++ )
    if( ch == word[i] ) return true;
  return false;
}

vector<string> tokenize( string words, string delimiters ) {
  vector<string> tokens;
  unsigned int i = 0;
  while( i < words.size() ) {
    while( i < words.size() && onString(words[i], delimiters) )
      i++;
    unsigned int start = i;
    if( i < words.size() )
      i++;
    while( i < words.size() && !onString(words[i], delimiters) )
      i++;
    if( i > start )
      tokens.push_back( words.substr(start, i - start) );
  }
  return tokens;
}*/

vector<string> tokenize( string words, string delimiters ) {
  vector<string> tokens;
  char *running = strdup(words.c_str());
  char *separators = strdup(delimiters.c_str());
  char *token = strtok(running, separators);
  while( token != NULL ) {
    if( token[0] != '.' )
      tokens.push_back( token );
    token = strtok(NULL, separators);
  }
  free(running); free(separators);
  return tokens;
}

string toUpper( string str ){
  for( int i=0; i< strlen( str.c_str() ); i++ ){
    if ( str[i] >= 'a' && str[i] <= 'z' )
      str[i] = str[i] - 32;
  }
  return str;
}
