/*
 * Online.cpp
 * Online codes for Information Retrieval search
 *
 * Created by Xin Li and Fei Song on 12-07-28.
 * Copyright 2012 University of Guelph
 * 	All right reserved.
 *
 */

#include <stdlib.h>
#include <iostream>
#include <math.h>
#include "Online.h"

Online::Online( int maxlength, int verbosity, 
		int phrase_in, int root_in )
{
  mMaxLength = maxlength;
  mVerbosity = verbosity;
  mIncludePhrase = phrase_in;
  mIncludeSyn = root_in;
}

Online::~Online()
{ }

void
Online::init( ifstream & fin )
{
  loadSynMap( fin );
  loadDocOffset();
  loadPosting();
  loadDictionary();
  calculateDocumentMagnitude();

  if( mVerbosity > 1 )
    cerr << "Files load done! " << endl;
}

void
Online::loadSynMap(ifstream & ifs )
{
  // 1. loadSynMap
  long t_root = 0;
  string line; 
  while( getline(ifs, line) ){
    vector<string> tokens = tokenize(line, " \t\n");
    if( tokens.size() > 1 ) {
      // reform the n-gram phrase after tokenization 
      string t_str = tokens[0] ;
      for( unsigned int i = 1; i < tokens.size()-2; i++ )
        t_str += " " + tokens[i] ;

      t_root = atol ( tokens.back().c_str() ) ;
      mSynMap.push_back( phrase_root( t_str, t_root ) );
    }
  }
  ifs.close();
  
  // for debugging 
  if ( mVerbosity > 10 ) { 
    for(long i = 0 ; i < mSynMap.size(); i++ )   cout << mSynMap[i].phrase << "\t" << mSynMap[i].root << endl;  
  }
}

void Online::loadDocOffset () {
  // 2. read docOffset file
  /*	here we do modify the concept offset and replace to "size". 
	Then, hereby we load doc size.		*/
  ifstream fin;
  fin.open("docOffset.txt");
  if( !fin )  {
    cerr << "docOffset file loading failed." << endl;
    exit(1); }

  string line; 
  while( getline(fin, line) ) {
    vector<string> tokens = tokenize( line, " \t\n");
    if( tokens.size() != 2 ) { 
      cout << "ERROR with docOffset document..." << endl; 
      exit(1);  
    }

    entry aEntry;
    aEntry.docid = tokens[0];
    aEntry.size = atoi( tokens[1].c_str() );
    aEntry.magnitude = 0.0;
    mDocOffset.push_back(aEntry);
  }
  fin.close();
 
  // for debugging
  if ( mVerbosity > 10 ){ 
    cout << "\n mDocOffsets: " << mDocOffset.size() << endl;
    for ( long i = 0 ; i < mDocOffset.size(); i ++ )
      cout << mDocOffset[i].docid << "\t" << mDocOffset[i].magnitude << "\t" 
	   << mDocOffset[i].size << endl;
  } // for debugging END
}

void
Online::loadPosting()
{
  // 3. read posting.txt 
  ifstream fin;
  fin.open("posting.txt");
  posting aPosting;
  string line; 
  while( getline(fin, line) ) {
    vector<string> tokens = tokenize( line, " \t\n");
    if( tokens.size() != 2 ) {
      cout << "ERROR: posting file has problem with line: " << line << endl;
      exit(1);
    }
    
    aPosting.did = atol( tokens[0].c_str() );
    aPosting.fre = atol( tokens[1].c_str() );
    mPosting.push_back( aPosting );
  } 
  cout << "Posting size: " << mPosting.size() << endl;
  fin.close();
  
  // for debugging
  if ( mVerbosity > 10 ){ 
    cout << "================================" << endl;
    for(long i = 0; i < mPosting.size(); i++ )
      cout << mPosting[i].did << "\t" << mPosting[i].fre << endl;
  }
}

void Online::loadDictionary(){
  // 4. read dictionary.txt
  // 	Store them to a temporarily vector<string> 

  string line; 
  ifstream fin;
  fin.open("dictionary.txt");
  while( getline(fin, line) ) {
    vector<string> tokens = tokenize(line, " \t\n");
    if( tokens.size() < 2 ) {
      cout << "dictionary: \" " << line << " \" doesn't have enough information!\n";
      exit(1);  
    }

    long offset; int length;
    offset = atol( tokens.back().c_str() );
    length = tokens.size() - 1;

    string ngram = tokens[0];
    for( int i = 1; i < length; i++ )
      ngram += " " + tokens[i];
    
    ngramInfo tNgramInfo;
    tNgramInfo.ngram = ngram;
    tNgramInfo.offset = offset;
    mNgram.push_back( tNgramInfo );
  }

  ngramInfo tNgramInfo( "ZZZZZ", mPosting.size() );
  mNgram.push_back( tNgramInfo );
  fin.close();
 
  cout << "mNgram size: " << mNgram.size() << endl;
   
  // create mDict, which is a map.
  //for( long i = 0; i < mNgram.size()-1; i++ )
    //mDict.insert( pair<string, long >( mNgram[i].ngram, i) );

  // for debugging END
  if ( mVerbosity > 10 ){
      cout << "================================" << endl;
      cout << "mNgram: " << endl;
      for ( long i = 0 ; i < mNgram.size(); i ++ )
        cout << mNgram[i].ngram << "\t" << mNgram[i].offset << endl;
  } // for debugging END
}

void
Online::calculateDocumentMagnitude ()
{
  unsigned long i = 0;  

  // Accumulate the tfidfs' value to each document
  for ( i = 0; i < mNgram.size()-1; i++ ) {
    // dictionary level
    for ( long j = mNgram[i].offset ; j < mNgram[i+1].offset ; j++ ) {
      // posting level
      double tfidf = 0.0;
      tfidf = (double)mPosting[j].fre /*/ mDocOffset[ mPosting[j].did ].size*/;
      tfidf *= log( (double)mDocOffset.size() / (mNgram[i+1].offset-mNgram[i].offset) );
      mDocOffset[ mPosting[j].did ].magnitude += pow( tfidf, 2);
    }
  }

  // Calculate the square root of temp magnitude of each document, 
  //	which is the what we need.
  for( i = 0 ; i < mDocOffset.size(); i++ ) {
    mDocOffset[i].magnitude = sqrt( mDocOffset[i].magnitude );
    if( mVerbosity > 1 )
      cout << mDocOffset[i].docid << "\t" << mDocOffset[i].magnitude << endl;
  }
  cout << "calculate doc magni finished ... " << endl;
}

void Online::onlineQuerying( ifstream & fin, ofstream & fout ) {
  string tline;
  string queryId;
  int qid = -1;
  vector< vector<string> > tquery;
  
  while ( getline( fin , tline ) ) {
    if( mVerbosity > 1 )    cout << tline << endl;

    vector<string> tokens = tokenize( tline , " \t\n");
    if ( !tokens.empty() ) {
      if ( tokens[0] == "$query" || tokens[0] == "$queri" ) {
        if ( qid == -1 ){
          queryId = tokens[1] ;
          qid++ ;
        } else {
	  if ( ! tquery.empty() )
	    onlineQuerying( queryId, tquery, fout );
	  tquery.clear();
	  queryId = tokens[1] ;
	  qid++ ;
        }
      } else {
	vector<string> tstr ;
        for( int i = 0; i < tokens.size(); i++ )
	  tstr.push_back( tokens[i] );
	tquery.push_back( tstr );
      }
    }
  }
  if ( ! tquery.empty() ) {
    onlineQuerying( queryId, tquery, fout );
    tquery.clear();
  }
}

void Online::onlineQuerying( string queryId, vector< vector<string> > & tokens, ofstream & fout ) {
  if ( mVerbosity > 1 )	{ 
    cout << "online Query test1 \n" << "Query Id: " << queryId << endl;
    for( unsigned int e = 0; e < tokens.size() ; e++ ) { 
      for ( int i = 0 ; i < tokens[e].size(); i ++ )
        cout << tokens[e][i] << " ";
      cout << endl;
    }cout << endl;	}

  //map<string, long>::iterator dict_it; // for mDict
  long pos = 0;

  // Define a container, tfidf_list to store the tfidf value for 
  //				each known ngram in the query ).
  // The second part of tfidf_list will be used to store tf firstly,
  //  afterward, the value will represent tfidf calculated by using tf.

  map<long, double> tfidf_list;
  map<long, double>::iterator tfidf_it;
  
  long querySize = 0;

  // define a multimap to store the final result and will be sent to output()
  multimap<double, long, greater<double> > result;  

  for( unsigned int e = 0; e < tokens.size() ; e++ ) {
    int phrase_restart_point = 0;

    // whole query
    for( unsigned int i = 0; i < tokens[e].size(); i++ ) {
      // each sequence in a query(may has more than one sequence/sentence)
  
      if(mVerbosity > 1) cout << "phrase_restart_point= " << phrase_restart_point << " \t sentence_no= " << e << endl;
      querySize++;

      /*
      if( i >= phrase_restart_point ){		// for less unigram
        // Word only
        if ( mIncludeSyn == 0 )
          //dict_it = mDict.find( tokens[e][i] );
	  pos = binary_search( mNgram, tokens[e][i], 0, mNgram.size()-1 );
        else if ( mIncludeSyn == 1 ) {
	  long position = binary_search( mSynMap, tokens[e][i], 0, mSynMap.size()-1 );
	  if ( position > -1 ) {
	    long t_root = findRoot( position );
	    //dict_it = mDict.find( mSynMap[t_root].phrase ); 
	    pos = binary_search( mNgram, mSynMap[t_root].phrase, 0, mNgram.size()-1 );
          } else
	    continue;
        }

        //if ( dict_it != mDict.end() ) {
        if ( pos > -1 ) {
          //tfidf_it = tfidf_list.find( dict_it->second );
          tfidf_it = tfidf_list.find( pos );
      	  if ( tfidf_it != tfidf_list.end() )
            tfidf_it->second += 1.0 ;
      	  else 
            tfidf_list[ pos ] = 1.0;
        } else {
	  if ( mVerbosity > 1 ) cout << "SAY NO!     No such word was found " <<  tokens[e][i]  << endl;
	  //if ( mIncludeSyn == 0 )
	    //continue;
        }
      }						// for less unigram
      else
  	continue;	*/

      // Phrase only
      if ( mIncludePhrase == 1 && i >= phrase_restart_point) {
	string str;
	str = tokens[e][i];
	for ( unsigned int n = 1; n < mMaxLength && i+n < tokens[e].size() ; n ++ ) 
	  str += " " + tokens[e][ i+n ];
        vector<string> t_tokens = tokenize( str, " " );
	bool first = true;
	while( t_tokens.size() > 1 ){
	  string _str = t_tokens[0];
	  if ( first ) {
	    _str = str;
	    first = false;
	  } else 
	    for( int j = 1; j < t_tokens.size(); j++ )
	      _str += " " + t_tokens[j];	 

	  if ( mIncludeSyn == 0 ) {
	    //dict_it = mDict.find( _str );
	    pos = binary_search( mNgram, _str, 0, mNgram.size()-1 );
	    //if ( dict_it != mDict.end() ) {
	    if ( pos > -1 ) {
	      tfidf_it = tfidf_list.find( pos );
	      if ( tfidf_it != tfidf_list.end() )
	        tfidf_it->second += 1.0;
	      else
	        tfidf_list[ pos ] = 1.0;

	      phrase_restart_point = i + t_tokens.size();
	      break;
	    }
	  } else {
	    // check if _str is exist in mSynMap which has all n-grams above threshold
	    long position = binary_search( mSynMap, _str, 0, mSynMap.size()-1 );
	    // if yes ( position == -1 means there is "no" _str exist in mSynMap )
	    if ( position > -1 ) {
	      long t_root = findRoot( position );
	      if ( t_root == position ) {
		//dict_it = mDict.find( _str );
	        pos = binary_search( mNgram, _str, 0, mNgram.size()-1 );
	      } else {
		//dict_it = mDict.find( mSynMap[t_root].phrase );
	        pos = binary_search( mNgram, mSynMap[t_root].phrase, 0, mNgram.size()-1 );
	      }
	      //if ( dict_it == mDict.end() ) {
	      if ( pos == -1 ) {
	        cerr << "1ERROR in query: " << queryId << " for " << _str << endl;
	        exit(1);
	      }

	      tfidf_it = tfidf_list.find( pos ); 
	      if ( tfidf_it != tfidf_list.end() )
	        tfidf_it->second += 1.0;
	      else
	        tfidf_list[ pos ] = 1.0;

	      phrase_restart_point = i + t_tokens.size();
	      break;
	    }
	  }
	  t_tokens.pop_back();
  	}
      }

      if( i >= phrase_restart_point ){		// for less unigram
        // Word only
        if ( mIncludeSyn == 0 )
          //dict_it = mDict.find( tokens[e][i] );
	  pos = binary_search( mNgram, tokens[e][i], 0, mNgram.size()-1 );
        else if ( mIncludeSyn == 1 ) {
	  long position = binary_search( mSynMap, tokens[e][i], 0, mSynMap.size()-1 );
	  if ( position > -1 ) {
	    long t_root = findRoot( position );
	    //dict_it = mDict.find( mSynMap[t_root].phrase ); 
	    pos = binary_search( mNgram, mSynMap[t_root].phrase, 0, mNgram.size()-1 );
          } else
	    continue;
        }

        //if ( dict_it != mDict.end() ) {
        if ( pos > -1 ) {
          //tfidf_it = tfidf_list.find( dict_it->second );
          tfidf_it = tfidf_list.find( pos );
      	  if ( tfidf_it != tfidf_list.end() )
            tfidf_it->second += 1.0 ;
      	  else 
            tfidf_list[ pos ] = 1.0;
        } else {
	  if ( mVerbosity > 1 ) cout << "SAY NO!     No such word was found " <<  tokens[e][i]  << endl;
	  //if ( mIncludeSyn == 0 )
	    //continue;
        }
      }						// for less unigram
      else
  	continue;	
    }
  }

  result = judgeSimilarity( querySize, queryId, tfidf_list );

  //output
  output( queryId, result, fout );
}

multimap<double, long, greater<double> >
Online::judgeSimilarity( long querySize, string queryId, map<long, double> & tfidf_list ) 
{
  if( mVerbosity > 1 )     cout << "judgeSimilarity" << endl;
  /* 	Judge the similarity:   if only one element in tfidf_list, use "tfidf order";
	  			if more than one, use "cosine measure".  	*/

  entry query( queryId, 0.0, querySize);

  map<long, double>::iterator it;
  multimap<double, long, greater<double> > result;  

  // NEW CALL
  if( mVerbosity > 1 ) cout << "new call" << endl;
  if( tfidf_list.size() == 0 )	{
    cout << "No match in query \"" << queryId << "\" exist in dictionary."<< endl;  
    return result;  		}
 
  // only using tfidf order 
  /*if( tfidf_list.size() == 1 ) { 
    if( mVerbosity > 1 ) cout << "tfidf order 1" << endl;

    long pos = tfidf_list.begin()->first;
    for ( long i = mNgram[ pos ].offset; i < mNgram[ pos + 1 ].offset; i++ ) {
      double tf = (double) mPosting[i].fre; // / mDocOffset[ mPosting[i].did ].size	
      long docFre = mNgram[ pos + 1 ].offset - mNgram[ pos ].offset;
      double tfidf = tf * log( (double)mDocOffset.size() / docFre ); 
      result.insert( pair<double, long>( tfidf, mPosting[i].did ) );
    }
  }*/

  // cosine measure and sort 
  else if( tfidf_list.size() >= 1 ) {
    if ( mVerbosity > 1 ) cout << "tfidf order 2" << endl;
    // define a container to store temp result, temp_result,
    //   afterward, the result within this container will be transfered to (multimap)result.
    map<long, double> temp_result;
    map<long, double>::iterator itR;
    					if ( mVerbosity > 1 ) showInputSegment( tfidf_list );

    // count the tfidf value of each word which is in tokens()
    it = tfidf_list.begin();
    for ( ; it != tfidf_list.end(); it++ ) {
      //if ( mVerbosity > 1) cout << mNgram[ it->first ].ngram << endl;

      long pos = it->first;
      it->second = it->second * 1.0 /*/ query.size */;
      long docFre = mNgram[ pos + 1 ].offset - mNgram[ pos ].offset;
      it->second *= log( (double)mDocOffset.size() / docFre );

      if ( mVerbosity > 1 ) {
        cout << mDocOffset.size() << "\t" << docFre << "\t" 
	     << mNgram[pos+1].ngram <<  "\t" << mNgram[pos+1].offset << "\t" 
	     <<  mNgram[pos].ngram << "\t" << mNgram[pos].offset; 
        cout << ">> " << mNgram[it->first].ngram << "  " << it->second
	     << "   " << query.size << "  " << docFre << "  " << mDocOffset.size() << endl; 
      }
      // query magnitude accumulation
      query.magnitude += pow( it->second * 1.0 , 2 ) ;

      for ( long i = mNgram[ pos ].offset; i < mNgram[ pos + 1 ].offset; i++ ) {
  	double tfidf = (double)mPosting[i].fre /* / mDocOffset[ mPosting[i].did ].size */;
	tfidf *= log( (double)mDocOffset.size() / docFre );

	itR = temp_result.find( mPosting[i].did );
	if ( itR != temp_result.end() )
	  itR->second += (double)it->second * tfidf;
	else
	  temp_result.insert(pair<long, double>( mPosting[i].did, (double)it->second * tfidf ) );	
      }
    }
    query.magnitude = sqrt( query.magnitude );
					// test
    					if( mVerbosity > 1 ){ showInputSegment( tfidf_list );
					  cout << ">> query magni: " << query.magnitude << endl;}
					// test
    itR = temp_result.begin();
    for( ; itR != temp_result.end(); itR++ ) {
      double cosine = itR->second * 1.0 / ( query.magnitude * mDocOffset[itR->first].magnitude ); 
      result.insert( pair<double, long>( cosine, itR->first ) );
    }
  }
  return result;
}

void
Online::output( string queryId, multimap<double, long, greater<double> > & result, ofstream & fout )
{
  if( mVerbosity > 1 ){
    cout << "output" << endl;
    cout << "This is the result output module" << endl;
  }

  if( result.empty() ){
    cout << "There is no document satisfying your query: " << queryId << endl;
    return;
  }

  int count = 1000;
  //int count = min( result.size() , mResultLimit );

  multimap<double, long, greater<double> >::iterator it = result.begin();

  int result_num = count;
  if( count > result.size() )
    result_num = result.size();

  int i = 0;
  while( it != result.end() && i < result_num ){
    if ( it->first > 0 )
      fout << queryId << "\tQ0\t" << toUpper(mDocOffset[it->second].docid) 
	   << "\t" << i << "\t" << it->first*100 << "\t" << "STANDARD"<< endl;
    it++;
    i++;
  }
}

void
Online::showInputSegment( map<long, double> segment )
{
  if( segment.empty() )
    return;

  cout << "Present segment part:" << endl;
  map<long, double>::iterator it;
  it = segment.begin();
  for ( ; it != segment.end(); it ++)
    cout << mNgram[it->first].ngram << " " << it->second << endl;
}

long Online::binary_search( vector<phrase_root> & A, string key, long imin, long imax)
{
  // continue searching while [imin,imax] is not empty
  while (imax >= imin)
    {
      // calculate the midpoint for roughly equal partition 
      //int imid = midpoint(imin, imax);
      long imid = (imin + imax)/2 ;

      // determine which subarray to search
      if ( A[imid].phrase < key )
        // change min index to search upper subarray
        imin = imid + 1;
      else if ( A[imid].phrase > key )
        // change max index to search lower subarray
        imax = imid - 1;
      else
        // key found at index imid
        return imid;
    }
  // key not found
  return -1;
}

long Online::binary_search( vector<ngramInfo> & A, string key, long imin, long imax)
{
  // continue searching while [imin,imax] is not empty
  while (imax >= imin)
    {
      // calculate the midpoint for roughly equal partition 
      //int imid = midpoint(imin, imax);
      long imid = (imin + imax)/2 ;

      // determine which subarray to search
      if ( A[imid].ngram < key )
        // change min index to search upper subarray
        imin = imid + 1;
      else if ( A[imid].ngram > key )
        // change max index to search lower subarray
        imax = imid - 1;
      else
        // key found at index imid
        return imid;
    }
  // key not found
  return -1;
}

long Online::findRoot( long position )
{
  if( mSynMap[position].root == position )
    return position;
  else{
    long t_root = mSynMap[position].root;
    while( mSynMap[t_root].root != t_root ){
      t_root = mSynMap[t_root].root;
    }
    return t_root;
  }
}

