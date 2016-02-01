#ifndef __OPTS__
#define __OPTS__

#include <iostream>

using namespace std;

typedef struct opts_t {
	bool generate;					// -g generate shares
	bool recover;					// -r recover secret
	const char* userIdentifier;		// -u user identification e.g. email or username
	int nShares;					// -s number of shares
	int threshold;					// -t threshold count
	//char* password;				// -p password string
	char* inFile;					// -p password file
	bool destructionKey;			// -d generate destruction key
	bool authenticator;				// -a generate authenticator key
	//char* inShares[];				// -? input shares

} opts_t;

static const char *optString = "gru:s:t:p:dah?";		//Il:o:vh?

void display_usage( int exit_code );
bool parse_inputs(int argc, char* argv[], opts_t &opts);
bool check_options(opts_t &opts);


#endif
