
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "secret_sharing.h"
#include "opts.h"



using namespace std;



void convert_document( void );




int main(int argc, char* argv[]) {

	opts_t opts;

	parse_inputs(argc, argv, opts);
	check_options(opts);

    convert_document();


    generate();

/*
	RandomPool rng;
	int threshold = 3;
	int nShares = 5;
	char filename[16] = "kokos.txt";
	const char* inFilenames[16] = {"kokos.txt.000", "kokos.txt.001", "kokos.txt.002"};

	char outFilename[16] = "kokosOut.txt";
	char seed[16] = "1234";

	SecretShareFile(threshold, nShares, filename, seed);
	SecretRecoverFile(3, outFilename, (char *const *)inFilenames);
*/

	return EXIT_SUCCESS;
}


void convert_document( void )
{
    /* ... */
	cout << "GOOD" << endl;
}











