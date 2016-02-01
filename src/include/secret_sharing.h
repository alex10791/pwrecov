#ifndef __SECRET_SHARING__
#define __SECRET_SHARING__

#include "ida.h"
#include "osrng.h"
#include "files.h"

//extern globalArgs_t globalArgs;

using namespace CryptoPP;
using namespace std;

void generate();
void recover();

void SecretShareFile(int threshold, int nShares, const char *filename, const char *seed);
void SecretRecoverFile(int threshold, const char *outFilename, char *const *inFilenames);


#endif
