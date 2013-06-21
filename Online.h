#ifndef _ONLINE_H_
#define _ONLINE_H_

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "defs.h"
#include "util.h"
using namespace std;

class Online {

  public:
        
	Online( int maxlength, int verbosity, int phrase, int root );

        ~Online();

	void init( ifstream & );
 
	void onlineQuerying( ifstream & fin, ofstream & fout );

	void showInputSegment( map<long, double> segment );

  private:
  	// function members
	void loadSynMap( ifstream & );
	
	void loadDocOffset();

	void loadPosting();
	
	void loadDictionary();

	void onlineQuerying( string queryNum, vector< vector<string> > & tokens, ofstream & fout);
	
	void calculateDocumentMagnitude();

	multimap<double, long, greater<double> > judgeSimilarity( long docSize, string queryId, map<long, double> & tfidf_list );

	void output( string, multimap<double, long, greater<double> > & result, ofstream & fout );
	
	long binary_search( vector<phrase_root> & A, string key, long imin, long imax);

	long binary_search( vector<ngramInfo> & A, string key, long imin, long imax);

	long findRoot( long position );

	// container members
	//map<string, vector<posting> > mDict;
	//map<string, long> mDict;
	vector<ngramInfo> mNgram;

	vector<entry> mDocOffset;  
     
	//vector<sequence> mSequences;

	vector<posting> mPosting;

	vector<phrase_root> mSynMap;

	int mMaxLength;
	int mVerbosity;
	int mIncludeSyn;
	int mIncludePhrase;
};

#endif
