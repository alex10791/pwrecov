
#include <iostream>

//#include "cryptopp"
//#include "aes.h"
#include "ida.h"
#include "osrng.h"
#include "files.h"

#include <string>
#include <vector>

using namespace std;

using namespace CryptoPP;

void SecretShareFile(int threshold, int nShares, const char *filename, const char *seed);
void SecretRecoverFile(int threshold, const char *outFilename, char *const *inFilenames);

int main(int argc, char* argv[]) {
	
	for (int i = 0; i < argc; ++i) {
		std::cout << argv[i] << std::endl;
	}

	RandomPool rng;
	int threshold = 3;
	int nShares = 5;
	char filename[16] = "kokos.txt";
	const char* inFilenames[16] = {"kokos.txt.000", "kokos.txt.001", "kokos.txt.002"};

	char outFilename[16] = "kokosOut.txt";
	char seed[16] = "1234";

	SecretShareFile(threshold, nShares, filename, seed);
	SecretRecoverFile(3, outFilename, (char *const *)inFilenames);

}

void SecretShareFile(int threshold, int nShares, const char *filename, const char *seed)
{
    RandomPool rng;
    rng.IncorporateEntropy((byte *)seed, strlen(seed));

    ChannelSwitch *channelSwitch;
    FileSource source(filename, false, new SecretSharing(rng,
        threshold, nShares, channelSwitch = new ChannelSwitch));

    vector_member_ptrs<FileSink> fileSinks(nShares);
    string channel;
    for (int i=0; i<nShares; i++)
    {
        char extension[5] = ".000";
        extension[1]='0'+byte(i/100);
        extension[2]='0'+byte((i/10)%10);
        extension[3]='0'+byte(i%10);
        fileSinks[i].reset(new FileSink((string(filename)+extension).c_str()));

        channel = WordToString<word32>(i);
        fileSinks[i]->Put((byte *)channel.data(), 4);
        channelSwitch->AddRoute(channel, *fileSinks[i], DEFAULT_CHANNEL);
    }

    source.PumpAll();
}


void SecretRecoverFile(int threshold, const char *outFilename, char *const *inFilenames)
{
    SecretRecovery recovery(threshold, new FileSink(outFilename));

    vector_member_ptrs<FileSource> fileSources(threshold);
    SecByteBlock channel(4);
    int i;
    for (i=0; i<threshold; i++)
    {
        fileSources[i].reset(new FileSource(inFilenames[i], false));
        fileSources[i]->Pump(4);
        fileSources[i]->Get(channel, 4);
        fileSources[i]->Attach(new ChannelSwitch(recovery, string((char *)channel.begin(), 4)));
    }

    while (fileSources[0]->Pump(256))
        for (i=1; i<threshold; i++)
            fileSources[i]->Pump(256);

    for (i=0; i<threshold; i++)
        fileSources[i]->PumpAll();
}






