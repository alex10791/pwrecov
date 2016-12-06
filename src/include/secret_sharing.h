#ifndef __SECRET_SHARING__
#define __SECRET_SHARING__

#include "crypto++/ida.h"
#include "crypto++/osrng.h"
#include "crypto++/files.h"
#include "crypto++/default.h"
#include "crypto++/hex.h"

#include "opts.h"

using namespace CryptoPP;
using namespace std;

enum share_type {DESTRUCTION_KEY = 0, AUTHORIZER_KEY = 1, SHARE = 2};

typedef struct share_data {
	string share_id;
	share_type type;
	string secretOwner;
	int nShares;
	int threshold;
	bool authorizer_exists;
	bool destruction_key_exists;
	string share;
} share_data;

typedef struct recover_properties {
	string secretOwner;
	int nShares;
	int threshold;
	bool authorizer_exists;
	bool destruction_key_exists;
} recover_properties;

/*
 * Writes a share to file
 */
void writeShare(opts_t &opts, string &filename, char share_id[4], share_type type, string &share);

/*
 * Reads a share from file
 */
void readShare(string &filename, share_data &sd);

/*
 * Improts shares from files
 */
int importShares(opts_t &opts, recover_properties &rp, string &destruction_key, string &authorizer_key, string *shareStrings, bool validate);

/*
 * Exports shares to files
 */
void exportShares(opts_t &opts, string &destruction_key, string &authorizer_key, string *shareStrings);

/*
 * Generates shares based on options
 */
void generate(opts_t &opts, string &secret);

/*
 * Recovers secret based on options
 */
void recover(opts_t &opts, string &secret);

/*
 * Genearte AES key
 * AES encrypt a secret if required
 */
void encrypt(string &secret, string &cipher, string &key_encoded, bool do_encrypt);

/*
 * AES decrypt a secret if required
 */
void decrypt(string &cipher, string &secret, string &key_encoded, bool do_decrypt);

/*
 * Generates nShares shares from secret with a threshold of threshold (-->function arg)
 * Inputs and Outputs are strings
 */
void SecretShareString(int threshold, int nShares, const string &inputString, string *sharesEncoded);

/*
 * Recovers secret from threshold shares  
 * Inputs and Outputs are strings
 */
void SecretRecoverString(int threshold, string &outString, const string *sharesEncoded);

/*
 * AES encrypt a string 
 */
void EncryptString(SecByteBlock key, const string plain, string &cipher);

/*
 * AES decrypt a string 
 */
void DecryptString(SecByteBlock key, const string cipher, string &plain);


void statusCodeMessage(int code, string &msg);

#endif
