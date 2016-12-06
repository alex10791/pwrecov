
#include "secret_sharing.h"
#include <string>
#include <vector>
#include <fstream>

void generate(opts_t &opts, string &secret) {

    // cipher pipeline states
    string destruction_cipher;
    string authorizer_cipher;
    string *shareStrings = new string[opts.nShares];

    // encoded keys
    string destruction_key_encoded;
    string authorizer_key_encoded;

    encrypt(secret, destruction_cipher, destruction_key_encoded, opts.destructionKey);

    encrypt(destruction_cipher, authorizer_cipher, authorizer_key_encoded, opts.authorizer);

    SecretShareString(opts.threshold, opts.nShares, authorizer_cipher, shareStrings);

    exportShares(opts, destruction_key_encoded, authorizer_key_encoded, shareStrings);

}

void recover(opts_t &opts, string &secret) {

    string destruction_key_encoded;
    string authorizer_key_encoded;

    string *shareStrings = new string[opts.nShares];
    recover_properties rp;

    int status;
    status = importShares(opts, rp, destruction_key_encoded, authorizer_key_encoded, shareStrings, !(bool(opts.forceRecover)));

    if (opts.verbose == true) {
        if (status == 0) {
            cout << "secret_owner:\t\t" << rp.secretOwner << endl;
            cout << "nShares:\t\t" << rp.nShares << endl;
            cout << "threshold:\t\t" << rp.threshold << endl;
            cout << "authorizer_exists:\t" << rp.authorizer_exists << endl;
            cout << "destruction_key_exists:\t" << rp.destruction_key_exists << endl;
        }
        for (int i = 0; i < opts.nShares; ++i) {
            cout << "Share " << i << ":\t\t" << shareStrings[i] << endl;
        }
        cout << "destruction key:\t" << destruction_key_encoded << endl;
        cout << "authorizer key:\t\t" << authorizer_key_encoded << endl;
    }

    if (status != 0) { 
        string msg;
        statusCodeMessage(status, msg);
        cerr << "[x] Something went wrong -- Status Code: " << status << " Status Message: " << msg << endl; 
        exit(-1); 
    }

    string shared_secret;
    string authorizer_clear;

    SecretRecoverString(opts.threshold, shared_secret, shareStrings);

    decrypt(shared_secret, authorizer_clear, authorizer_key_encoded, opts.authorizer);
    decrypt(authorizer_clear, secret, destruction_key_encoded, opts.destructionKey);

}


int importShares(opts_t &opts, recover_properties &rp, string &destruction_key, string &authorizer_key, string *shareStrings, bool validate) {

    share_data *sd = new share_data[opts.numInputFiles];
    
    for (int i = 0; i < opts.numInputFiles; ++i) {
        string filename = opts.inShares[i];
        readShare(filename, sd[i]);
    }

    int type_occur[3];
    for (int i = 0; i < 3; ++i) type_occur[0] = 0;

    // if validation is required
    if (validate == true) {
        // check that common fields are the same
        for (int i = 0; i < opts.numInputFiles; ++i) {
            //if (sd[0].share_id != sd[i].share_id) { no_error = false; break; }
            if (sd[i].type < 0 || sd[i].type > 3) return -1;
            type_occur[(int)sd[i].type]++;
            if (sd[0].secretOwner != sd[i].secretOwner) return -2;
            if (sd[0].nShares != sd[i].nShares) return -3;
            if (sd[0].threshold != sd[i].threshold) return -4;
            if (sd[0].authorizer_exists != sd[i].authorizer_exists) return -5;
            if (sd[0].destruction_key_exists != sd[i].destruction_key_exists) return -6;
            //if (sd[0].share != sd[i].share) { no_error = false; break; }
        }

        // check that if destruction key exists, only on file is given, and if it does not exist no file is given
        if (sd[0].destruction_key_exists == true) {
            if (type_occur[0] != 1) return -7;
        } else if (type_occur[0] != 0) return -8;

        // check that if authorizer key exists, only on file is given, and if it does not exist no file is given
        if (sd[0].authorizer_exists == true) {
            if (type_occur[1] != 1) return -9;
        } else if (type_occur[1] != 0) return -10;
        
        // must check that common ids and shares are not used
        // TODO
        for (int i = 0; i < opts.numInputFiles; ++i) {
            for (int j = i+1; j < opts.numInputFiles; ++j) {
                if (sd[i].share_id == sd[j].share_id && sd[i].type == SHARE && sd[j].type == SHARE) {
                    return -11;
                }
                if (sd[i].share == sd[j].share && sd[i].type == SHARE && sd[j].type == SHARE) {
                    return -12;
                }
            }
        }

        rp.secretOwner = sd[0].secretOwner;
        rp.nShares = sd[0].nShares;
        rp.threshold = sd[0].threshold;
        rp.authorizer_exists = sd[0].authorizer_exists;
        rp.destruction_key_exists = sd[0].destruction_key_exists;

    } else {

        rp.secretOwner = opts.secretOwner;
        rp.nShares = opts.nShares;
        rp.threshold = opts.threshold;
        rp.authorizer_exists = (bool)opts.authorizer;
        rp.destruction_key_exists = (bool)opts.destructionKey;
    }

    int share_count = 0;
    for (int i = 0; i < opts.numInputFiles; ++i) {
        if (share_count > opts.nShares) return -13;
        if (sd[i].type == SHARE) { shareStrings[share_count] = sd[i].share; share_count++; }
        if (sd[i].type == AUTHORIZER_KEY) authorizer_key = sd[i].share;
        if (sd[i].type == DESTRUCTION_KEY) destruction_key = sd[i].share;
    }

    return 0;
}


void exportShares(opts_t &opts, string &destruction_key, string &authorizer_key, string *shareStrings) {

    string shares_filename = "shares/share.simp";
    string authorizer_filename = "shares/share.auth";
    string destruction_filename = "shares/share.dest";

    for (int i=0; i<opts.nShares; ++i)
    {

        char share_id[4] = "000";
        share_id[0]='0'+byte(i/100);
        share_id[1]='0'+byte((i/10)%10);
        share_id[2]='0'+byte(i%10);

        writeShare(opts, shares_filename, share_id, SHARE, shareStrings[i]);
    }

    writeShare(opts, authorizer_filename,  (char *)"key", AUTHORIZER_KEY,  authorizer_key);
    writeShare(opts, destruction_filename, (char *)"key", DESTRUCTION_KEY, destruction_key);

}

void readShare(string &filename, share_data &sd) {
    
    ifstream in_file;

    in_file.open(filename.c_str());

    string temp;
    int type, authorizer_exists, destruction_key_exists; 
    
    in_file >> temp >> temp >> temp >> temp >> temp;
    in_file >> temp;
    in_file >> temp >> sd.share_id;
    in_file >> temp >> type;
    sd.type = (share_type)type;
    in_file >> temp >> temp >> sd.secretOwner;
    in_file >> temp >> sd.nShares;
    in_file >> temp >> sd.threshold;
    in_file >> temp >> authorizer_exists;
    sd.authorizer_exists = (bool)authorizer_exists;
    in_file >> temp >> temp >> destruction_key_exists;
    sd.destruction_key_exists = (bool)destruction_key_exists;
    in_file >> temp >> sd.share;

}

void writeShare(opts_t &opts, string &filename, char share_id[4], share_type type, string &share) {

    ofstream out_file;
    
    // Set exceptions to be thrown on failure
    out_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {

        out_file.open((filename+"."+share_id).c_str());
        out_file << "# SECRET SHARE ISSUED #\n";
        out_file << "-----------------------\n";
        out_file << "id:\t\t\t" << share_id << "\n";
        out_file << "type:\t\t\t" << type << "\n";
        out_file << "secret owner:\t\t" << opts.secretOwner << "\n";
        out_file << "shares:\t\t\t" << opts.nShares << "\n";
        out_file << "threshold:\t\t" << opts.threshold << "\n";
        out_file << "authorizer:\t\t" << opts.authorizer << "\n";
        out_file << "destruction key:\t" << opts.destructionKey << "\n";
        out_file << "share:\t\t\t" << share << "\n";
        out_file.close();
    }
    catch (std::ios_base::failure& e) {
      std::cerr << "Could not write to " << filename+"."+share_id << std::endl;
    }

}


void encrypt(string &secret, string &cipher, string &key_encoded, bool do_encrypt){

    AutoSeededRandomPool rnd;

    // Generate a random destruction key
    SecByteBlock key(0x00, AES::DEFAULT_KEYLENGTH);
    rnd.GenerateBlock( key, key.size() );
    string key_str((char*)key.data(), key.size());

    // Pretty print cipher text
    StringSource ss1( key_str, true,
        new HexEncoder(
            new StringSink( key_encoded )
        ) // HexEncoder
    ); // StringSource


    if (do_encrypt == true) {
        EncryptString(key, secret, cipher);
    } else {
        cipher = secret;
    }

}

void decrypt(string &cipher, string &secret, string &key_encoded, bool do_decrypt){

    string key_str;

    StringSource ss(key_encoded, true, new HexDecoder(new StringSink(key_str)));
    //const byte* result = (const byte*) destination.data();
    SecByteBlock key((const byte*) key_str.data(), AES::DEFAULT_KEYLENGTH);

    //string clear;

    if (do_decrypt == true) {
        DecryptString(key, cipher, secret);
    } else {
        secret = cipher;
    }
} 


void EncryptString(SecByteBlock key, const string plain, string &cipher) {

    AutoSeededRandomPool rnd;

    // Generate a random IV
    byte iv[AES::BLOCKSIZE];
    for (int i = 0; i < AES::BLOCKSIZE; ++i)
        iv[i] = 0;
    //rnd.GenerateBlock(iv, AES::BLOCKSIZE);

    try
    {
        CBC_Mode< AES >::Encryption e;
        e.SetKeyWithIV( key, key.size(), iv );

        // The StreamTransformationFilter adds padding
        //  as required. ECB and CBC Mode must be padded
        //  to the block size of the cipher.
        StringSource ss( plain, true, 
            new StreamTransformationFilter( e,
                new StringSink( cipher )
            ) // StreamTransformationFilter      
        ); // StringSource
    }
    catch( const CryptoPP::Exception& e )
    {
        cerr << e.what() << endl;
        exit(1);
    }
}

void DecryptString(SecByteBlock key, const string cipher, string &plain) {

    AutoSeededRandomPool rnd;

    // Generate a random IV
    byte iv[AES::BLOCKSIZE];
    for (int i = 0; i < AES::BLOCKSIZE; ++i)
        iv[i] = 0;

    try
    {
        CBC_Mode< AES >::Decryption d;
        d.SetKeyWithIV( key, key.size(), iv );

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource ss( cipher, true, 
            new StreamTransformationFilter( d,
                new StringSink( plain )
            ) // StreamTransformationFilter
        ); // StringSource
    }
    catch( const CryptoPP::Exception& e )
    {
        cerr << e.what() << endl;
        exit(1);
    }
    
}




void SecretShareString(int threshold, int nShares, const string &inputString, string *sharesEncoded)
{
    RandomPool rng;

    SecByteBlock seed(32);
    OS_GenerateRandomBlock(false, seed, seed.size());
    
    //rng.IncorporateEntropy((byte *)seed, strlen(seed));
    rng.IncorporateEntropy(seed, seed.size());

    string *shares = new string[nShares];

    ChannelSwitch *channelSwitch;
    StringSource source(inputString, false, new SecretSharing(rng,
        threshold, nShares, channelSwitch = new ChannelSwitch));

    vector_member_ptrs<StringSink> stringSinks(nShares);
    string channel;
    for (int i=0; i<nShares; i++)
    {

        stringSinks[i].reset(new StringSink(shares[i]));

        channel = WordToString<word32>(i);
        stringSinks[i]->Put((byte *)channel.data(), 4);
        channelSwitch->AddRoute(channel, *stringSinks[i], DEFAULT_CHANNEL);
    }

    source.PumpAll();


    for (int i = 0; i < nShares; ++i) {
        // Pretty print cipher text
        StringSource ss3( shares[i], true,
            new HexEncoder(
                new StringSink( sharesEncoded[i] )
            ) // HexEncoder
        ); // StringSource
    }


}


void SecretRecoverString(int threshold, string &outString, const string *sharesEncoded)
{
    // decode share strings
    string *shares = new string[threshold];
    for (int i = 0; i < threshold; ++i) {
        StringSource ss(sharesEncoded[i], true, new HexDecoder(new StringSink(shares[i])));
    }


    SecretRecovery recovery(threshold, new StringSink(outString));

    vector_member_ptrs<StringSource> stringSources(threshold);
    SecByteBlock channel(4);
    int i;
    for (i=0; i<threshold; i++)
    {
        stringSources[i].reset(new StringSource(shares[i], false));
        stringSources[i]->Pump(4);
        stringSources[i]->Get(channel, 4);
        stringSources[i]->Attach(new ChannelSwitch(recovery, string((char *)channel.begin(), 4)));
    }

    while (stringSources[0]->Pump(256))
        for (i=1; i<threshold; i++)
            stringSources[i]->Pump(256);

    for (i=0; i<threshold; i++)
        stringSources[i]->PumpAll();
}




void statusCodeMessage(int code, string &msg) {
    switch( code ) {
        case 0:
            msg = "Sucess";
            break;

        case -1:
            msg = "Unknown share type";
            break;

        case -2:
            msg = "Inconsistent user";
            break;

        case -3:
            msg = "Inconsistent number of shares";
            break;

        case -4:
            msg = "Inconsistent threshold";
            break;

        case -5:
            msg = "Inconsistent authorizer existence";
            break;

        case -6:
            msg = "Inconsistent destruction key existence";
            break;

        case -7:
            msg = "Not exactly 1 destruction key given";
            break;

        case -8:
            msg = "Non-zero destruction keys given";
            break;

        case -9:
            msg = "Not exactly 1 authorizer key given";
            break;

        case -10:
            msg = "Non-zero authorizer keys given";
            break;

        case -11:
            msg = "Duplicate normal share id";
            break;

        case -12:
            msg = "Duplicate share";
            break;

        case -13:
            msg = "Shares given exeed shares genarated";
            break;

        default:
            /* Should not get here. */
            msg = "Unknown Code";
            break;
    }
}

