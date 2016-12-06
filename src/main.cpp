
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

#include "secret_sharing.h"
#include "opts.h"

// file stream abstraction
#include <fstream>


using namespace std;



void SetStdinEcho(bool enable = true);


int main(int argc, char* argv[]) {

	opts_t opts;

	parse_inputs(argc, argv, opts);
	check_options(opts);
	
	string secret;
	string secret_verify;

	if (opts.generate == true) {
		cout << "Type Secret: " << endl;
		SetStdinEcho(false);
		getline(cin, secret);
		SetStdinEcho(true);
		cout << "Verify Secret: " << endl;
		SetStdinEcho(false);
		getline(cin, secret_verify);
		SetStdinEcho(true);
		if (secret != secret_verify) {
			cerr << "[x] Secret missmatch, please try again" << endl;
			exit(-1);	
		}
    	generate(opts, secret);
	} else {
    	recover(opts, secret);
    	cout << "[*] Successful Secret Recovery" << endl;
    	cout << "[*] Secret: " << secret << endl;
    }


	return EXIT_SUCCESS;
}




void SetStdinEcho(bool enable)
{
#ifdef WIN32
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    if( !enable )
        mode &= ~ENABLE_ECHO_INPUT;
    else
        mode |= ENABLE_ECHO_INPUT;

    SetConsoleMode(hStdin, mode );

#else
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    if( !enable )
        tty.c_lflag &= ~ECHO;
    else
        tty.c_lflag |= ECHO;

    (void) tcsetattr(STDIN_FILENO, TCSANOW, &tty);
#endif
}



