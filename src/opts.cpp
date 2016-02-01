
#include "opts.h"

void display_usage( int exit_code )
{
    //puts( "doc2html - convert documents to HTML" );
    cout << "kokos" << endl;
    /* ... */
    exit( exit_code );
}


bool parse_inputs(int argc, char* argv[], opts_t &opts) {
	int opt = 0;
    
    /* Initialize opts before we get to work. */

    opts.generate = false;
	opts.recover = false;
	opts.userIdentifier = "user";
	opts.nShares = 0;
	opts.threshold = 0;
	//opts.password;
	opts.inFile = NULL;
	opts.destructionKey = false;
	opts.authenticator = false;
	//opts.inShares[] = NULL;

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
                opts.userIdentifier = optarg;
                break;
                
            case 's':
                opts.nShares = atoi(optarg);
                break;
                
            case 't':
                opts.threshold = atoi(optarg);
                break;

         	case 'p':
                opts.inFile = optarg;
                break;

            case 'd':
                opts.destructionKey = true;
                break;

           	case 'a':
                opts.authenticator = true;
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
    
    //opts.inputFiles = argv + optind;
    //opts.numInputFiles = argc - optind;
    
    return true;
}


bool check_options(opts_t &opts) {
	bool no_error = true;
	if (opts.generate == opts.recover) {
		cout << "[x] Please only generate or recover" << endl;
		no_error = false;
	}
	if (opts.threshold > opts.nShares) {
		cout << "[x] Shareholders should be more than threshold" << endl;
		no_error = false;
	}
	if (opts.userIdentifier == NULL) {
		cout << "[!] Warrning: no user identifier found, using default string \"user\"" << endl;
	}
	if (opts.nShares <= 1) {
		cout << "[x] Sharholders should be positive" << endl;
		no_error = false;
	}
	if (opts.threshold <= 1) {
		cout << "[x] Threshold should be positive" << endl;
		no_error = false;
	}
	if (opts.inFile == NULL) {
		cout << "[x] Input file not given" << endl;
		no_error = false;
	}
	if (!no_error) {
		cout << "[*] For help use -h" << endl;
		exit(EXIT_SUCCESS);
	}

	return no_error;
}