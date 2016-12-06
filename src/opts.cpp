
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "opts.h"

using namespace std;


static const char *optString = "gru:s:t:p:dafvh?";        //Il:o:vh?

void display_usage( int exit_code )
{
    //puts( "doc2html - convert documents to HTML" );
    cout << endl;
    cout << "\t-g\t:\tgenerate shares from secret" << endl << endl;
    cout << "\t-r\t:\trecover secret from shares" << endl << endl;
    cout << "\t-u\t:\tsecret owner identifier such as email (no spaces) --default user" << endl << endl;
    cout << "\t-s\t:\tnumber of shares to generate" << endl << endl;
    cout << "\t-t\t:\tshares threshold required for recovery" << endl << endl;
//    cout << "-p\t:\tsecret" << endl;
    cout << "\t-d\t:\tgenerate destruction key" << endl << endl;
    cout << "\t-a\t:\tgenerate authorizer key" << endl << endl;
    cout << "\t-f\t:\tforce recover using options instead of file data (skips file validation)" << endl << endl;
    cout << "\t-v\t:\tverbose output" << endl << endl;
    cout << "\t-h\t:\tdisplay this message" << endl << endl;
    /* ... */
    exit( exit_code );
}


bool parse_inputs(int argc, char* argv[], opts_t &opts) {
	int opt = 0;
    
    /* Initialize opts before we get to work. */

    opts.generate = false;
	opts.recover = false;
	opts.secretOwner = "user";
	opts.nShares = 0;
	opts.threshold = 0;
	//opts.password;
//	opts.inFile = NULL;
	opts.destructionKey = false;
	opts.authorizer = false;
	opts.inShares = argv;
    opts.numInputFiles = 0;
    opts.forceRecover = false;
    opts.verbose = false;

    opt = getopt( argc, argv, optString );
    while( opt != -1 ) {
        switch( opt ) {
            case 'g':
                opts.generate = true; /* true */
                break;
                
            case 'r':
                opts.recover = true;
                break;
                    
            case 'u':
                opts.secretOwner = optarg;
                break;
                
            case 's':
                opts.nShares = atoi(optarg);
                break;
                
            case 't':
                opts.threshold = atoi(optarg);
                break;

//         	case 'p':
//                opts.inFile = optarg;
//                break;

            case 'd':
                opts.destructionKey = true;
                break;

            case 'a':
                opts.authorizer = true;
                break;

            case 'f':
                opts.forceRecover = true;
                break;

            case 'v':
                opts.verbose = true;
                break;
          
            case 'h':   /* fall-through is intentional */
            case '?':
                display_usage(EXIT_SUCCESS);
                break;
                
            default:
                /* You won't actually get here. */
                break;
        }
        
        opt = getopt( argc, argv, optString );
    }
    
    opts.inShares = argv + optind;
    opts.numInputFiles = argc - optind;
    
    return true;
}


bool check_options(opts_t &opts) {
	bool no_error = true;
    int min_number_of_files = opts.threshold;
    if (opts.authorizer == true) min_number_of_files++;
    if (opts.destructionKey == true) min_number_of_files++;

	if (opts.generate == opts.recover) {
		cout << "[x] Please only generate or recover" << endl;
		no_error = false;
	}
	if ((opts.forceRecover || opts.generate) && opts.threshold > opts.nShares) {
		cout << "[x] Shareholders should be more than threshold" << endl;
		no_error = false;
	}
	if (opts.secretOwner == NULL) {
		cout << "[!] Warrning: no secret owner identifier found, using default string \"user\"" << endl;
	}
	if ((opts.forceRecover || opts.generate) && (opts.nShares < 1 || opts.nShares > 1000)) {
		cout << "[x] Sharholders should be positive" << endl;
		no_error = false;
	}
	if ((opts.forceRecover || opts.generate) && opts.threshold <= 1) {
		cout << "[x] Threshold should be positive" << endl;
		no_error = false;
	}
    /*
	if (opts.inFile == NULL) {
		cout << "[x] Input file not given" << endl;
        no_error = false;
	}
    */
    if (opts.recover == true && opts.numInputFiles < min_number_of_files) {
        cout << "[x] Input shares must be at least as as many as threshold" << endl;
        no_error = false;
    }
	if (!no_error) {
		cout << "[*] For help use -h" << endl;
		exit(EXIT_SUCCESS);
	}

	return no_error;
}