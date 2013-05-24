
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <sys/stat.h>

#include "modplug.h"

#include <vector>
#include <string>
#include <memory>
//#include <thread>
//#include <mutex>

#include "ChipPlayer.h"
#include "URLPlayer.h"

#include "common/Fifo.h"
#include "utils.h"

#include "VicePlayer.h"


#ifdef WIN32
#include "AudioPlayerWindows.h"
#else
#include "AudioPlayerLinux.h"
#endif

#include "Archive.h"

typedef unsigned int uint;
using namespace std;
using namespace utils;


class ModPlayer : public ChipPlayer {
public:
	ModPlayer(uint8_t *data, int size) {
		ModPlug_Settings settings;
		ModPlug_GetSettings(&settings);
		settings.mChannels = 2;
		settings.mFrequency = 44100;
		settings.mBits = 16;
		settings.mLoopCount = -1;
		ModPlug_SetSettings(&settings);
		mod = ModPlug_Load(data, size);
	}

	int getSamples(short *target, int noSamples) override {
		return ModPlug_Read(mod, (void*)target, noSamples*2) / 2;
	}

private:
	ModPlugFile *mod;
};

extern "C" {
#include "sexypsf/driver.h"
}

static Fifo *sexyFifo;

void sexyd_update(unsigned char *pSound, long lBytes)
{
	if(sexyFifo)
		sexyFifo->putBytes((char*)pSound, lBytes);
}


class PSXPlayer : public ChipPlayer {
public:
	PSXPlayer(const string &fileName) : fifo(1024 * 128) {
		char temp[1024];
		strcpy(temp, fileName.c_str());
		psfInfo = sexy_load(temp);
		sexyFifo = &fifo;
	}

	int getSamples(short *target, int noSamples) override {
		while(fifo.filled() < noSamples*2) {
			int rc = sexy_execute();
			if(rc <= 0)
				return rc;
		}
		if(fifo.filled() == 0)
			return 0;

		return fifo.getShorts(target, noSamples);
	}
private:
	Fifo fifo;
	PSFINFO *psfInfo;
};

#include <algorithm>

class PlayerSystem : public PlayerFactory {
	ChipPlayer *fromFile(File &file) override {
		if(file.getName().rfind(".psf") != string::npos)
			return new PSXPlayer {file.getName()};
		return new ModPlayer {file.getPtr(), file.getSize()};
	}

	virtual bool canHandle(const std::string &name) override {

		string uname;
		std::transform(name.begin(), name.end(), uname.begin(), ::toupper);
		printf("Checking %s\n", uname.c_str());
		return true;
		return (
			(uname.rfind(".MOD") != string::npos) ||
			(uname.rfind(".XM") != string::npos)
			);

	}

};

/*
Should handle; url parsing, http gets, archive extraction, local caching

URL FORMAT:

[file:/|http:/|]<filePath>[:port]:<SongPath>:<songStart>
eg
http://somesite.com/music/sids.zip:/Hubbard/commando.sid:2
file://mdat.monkey+smp.monkey



*/

int main(int argc, char* argv[]) {

	setvbuf(stdout, NULL, _IONBF, 0);
	printf("Modplayer test\n");

	//return 0;
	//URLPlayer urlPlayer {argv[1], new PlayerSystem()};
	//ChipPlayer *player = &urlPlayer; 
	//ChipPlayer *player = new ModPlayer { modFile.getPtr(), modFile.getSize() };	
	VicePlayer::init("c64");
	ChipPlayer *player = new VicePlayer { argv[1] };

	AudioPlayerNative ap;

	int bufSize = 4096;
	vector<short> buffer(bufSize);
	while(true) {
		int rc = player->getSamples(&buffer[0], bufSize);
		if(rc > 0)
			ap.writeAudio(&buffer[0], rc);
	}

	//waveOutClose(hWaveOut);
	return 0;

}