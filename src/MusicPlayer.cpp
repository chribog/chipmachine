
#include "MusicPlayer.h"

#include <coreutils/utils.h>
#include <coreutils/format.h>

#include <audioplayer/audioplayer.h>

#include <musicplayer/plugins/OpenMPTPlugin/OpenMPTPlugin.h>
#include <musicplayer/plugins/HTPlugin/HTPlugin.h>
#include <musicplayer/plugins/HEPlugin/HEPlugin.h>
#include <musicplayer/plugins/GSFPlugin/GSFPlugin.h>
#include <musicplayer/plugins/NDSPlugin/NDSPlugin.h>
#include <musicplayer/plugins/USFPlugin/USFPlugin.h>
#include <musicplayer/plugins/VicePlugin/VicePlugin.h>
#include <musicplayer/plugins/SexyPSFPlugin/SexyPSFPlugin.h>
#include <musicplayer/plugins/GMEPlugin/GMEPlugin.h>
#include <musicplayer/plugins/SC68Plugin/SC68Plugin.h>
#include <musicplayer/plugins/StSoundPlugin/StSoundPlugin.h>
#include <musicplayer/plugins/AdPlugin/AdPlugin.h>
#include <musicplayer/plugins/UADEPlugin/UADEPlugin.h>

#include <archive/archive.h>

using namespace std;
using namespace utils;

namespace chipmachine {

class RSNPlayer : public ChipPlayer {
public:
	RSNPlayer(const vector<string> &l, ChipPlugin *plugin) : songs(l), plugin(plugin) {
		player = shared_ptr<ChipPlayer>(plugin->fromFile(l[0]));
		//player->onMeta([=](const std::vector<std::string> &meta, ChipPlayer* player) {			
			//for(const auto &m : meta) {
			//	setMeta(m, player->getMeta(m));
			//}
		//});
		setMeta("title", player->getMeta("title"),
			"sub_title", player->getMeta("sub_title"),
			"game", player->getMeta("game"),
			"composer", player->getMeta("composer"),
			"length", player->getMeta("length"),
			"format", player->getMeta("format"),
			"songs", l.size());
	}

	virtual int getSamples(int16_t *target, int noSamples) override {
		if(player)
			return player->getSamples(target, noSamples);
		return 0;
	}

	virtual void seekTo(int song, int seconds) {
		player = nullptr;
		player = shared_ptr<ChipPlayer>(plugin->fromFile(songs[song]));
		if(player) {
			setMeta("sub_title", player->getMeta("sub_title"),
			//"game", player->getMeta("game"),
			//"composer", player->getMeta("composer"),
			"length", player->getMeta("length")
			//"format", player->getMeta("format")
			);
			if(seconds > 0)
				player->seekTo(-1, seconds);
		}
	}
private:
	vector<string> songs;
	std::shared_ptr<ChipPlayer> player;
	ChipPlugin *plugin;

};

class RSNPlugin : public ChipPlugin {
public:
	RSNPlugin(std::vector<ChipPlugin*> &plugins) : plugins(plugins) {}

	virtual std::string name() const { return "RSNPlugin"; }

	virtual ChipPlayer *fromFile(const std::string &fileName) {
		vector<string> l;
		makedir(".rsn");
		auto *a = Archive::open(fileName, ".rsn", Archive::TYPE_RAR);
		for(auto s : *a) {
			if(path_extension(s) == "spc") {
				a->extract(s);
				LOGD("Found %s", s);
				l.push_back(string(".rsn/") + s);
			}
		};
		delete a;

		if(l.size() > 0) {
			auto name = l[0];
			utils::makeLower(name);
			for(auto plugin : plugins) {
				if(plugin->canHandle(name)) {
					return new RSNPlayer(l, plugin);
				}
			}
		}
		return nullptr;

	};

	virtual bool canHandle(const std::string &name) {
		return (path_extension(name) == "rsn");
	}
private:
	std::vector<ChipPlugin*> &plugins;

};

MusicPlayer::MusicPlayer() : fifo(32786), plugins {
		//new ModPlugin {},
		new RSNPlugin { this->plugins },
		new OpenMPTPlugin {},
		new HTPlugin {},
		new HEPlugin {"data/hebios.bin"},
		new GSFPlugin {},
		new NDSPlugin {},
		new USFPlugin {},
		new VicePlugin {"data/c64"},
		new SexyPSFPlugin {},
		new GMEPlugin {},
		new SC68Plugin {"data/sc68"},
		new StSoundPlugin {},
		new AdPlugin {},
		new UADEPlugin {}
	}
{
	dontPlay = false;
	AudioPlayer::play([=](int16_t *ptr, int size) mutable {

		if(dontPlay) {
			memset(ptr, 0, size*2);
			return;
		}

		LOCK_GUARD(playerMutex);

		if(fadeOut > 0 && fadeOut <= pos) {
			player = nullptr;
		}

		if(!paused && player) {
			int sz = size;

			sub_title = player->getMeta("sub_title");

			if(fadeOut == 0 && changedSong == false) {
				if(length > 0 && pos/44100 > length) {
					LOGD("#### SONGLENGTH");
					fadeOut = pos + 44100*3;
				}
				if(fifo.getSilence() > 44100*5) {
					LOGD("############# SILENCE");
					//fadeOut = pos + 44100*3;
					fadeOut = pos;
				}
			}

			while(true) {
				if(sz <= 0)
					LOGD("WTF!");
				int rc = player->getSamples((int16_t*)fifo.ptr(), sz);				
				if(rc <= 0) {
					LOGD("RC %d", rc);
					player = nullptr;
					break;
				}

				//LOGD("SILENCE %d", fifo.getSilence());
				if(fadeOut != 0)
					fifo.setVolume((fadeOut - pos) / (3.0*44100.0));
				fifo.processShorts(nullptr, rc);
				pos += rc/2;
				sz -= rc;
				if(fifo.filled() >= size*2) {
					fifo.getShorts(ptr, size);
					std::lock_guard<std::mutex> guard(fftMutex);
					fft.addAudio(ptr, size);
					break;
				}
			}

		} else
			memset(ptr, 0, size*2);
	});
}

void MusicPlayer::seek(int song, int seconds) {
	LOCK_GUARD(playerMutex);
	if(player) {
		if(seconds < 0)
			pos = 0;
		else
			pos = seconds * 44100;
		player->seekTo(song, seconds);
		//length = player->getMetaInt("length");
		if(song >= 0)
			changedSong = true;
		updatePlayingInfo();
	}
}

void MusicPlayer::playFile(const std::string &fileName) {

	dontPlay = true;

	LOCK_GUARD(playerMutex);
	//toPlay = fileName;
	player = nullptr;
	player = fromFile(fileName);
	//toPlay = "";

	dontPlay = false;

	if(player) {

		fifo.clear();
		fadeOut = 0;
		changedSong = false;
		pause(false);
		pos = 0;
		updatePlayingInfo();
	}
}

void MusicPlayer::updatePlayingInfo() {
	SongInfo si;
	if(player) {
		auto game = player->getMeta("game");
		si.title = player->getMeta("title");
		if(game != "") {
			if(si.title != "") {
				si.title = format("%s (%s)", game, si.title);
			} else
			si.title = game;
		}
		si.composer = player->getMeta("composer");
		si.format = player->getMeta("format");
		//si.length = player->getMetaInt("length");
		si.numtunes = player->getMetaInt("songs");
		si.starttune = player->getMetaInt("startSong");

		length = player->getMetaInt("length");
		message = player->getMeta("message");
		sub_title = player->getMeta("sub_title");

	}
	{
		LOCK_GUARD(infoMutex);
		playingInfo = si;
	}
}

void MusicPlayer::pause(bool dopause) {
	if(dopause) AudioPlayer::pause_audio();
	else AudioPlayer::resume_audio();
	paused = dopause;
}

string MusicPlayer::getMeta(const string &what) {
	if(what == "message") {
		LOCK_GUARD(infoMutex);
		return message;
	}

	if(what == "sub_title") {
		LOCK_GUARD(infoMutex);
		return sub_title;
	}

	LOCK_GUARD(playerMutex);
	if(player)
		return player->getMeta(what);
	else
		return "";
}

// PRIVATE

shared_ptr<ChipPlayer> MusicPlayer::fromFile(const std::string &fileName) {
	shared_ptr<ChipPlayer> player;
	string name = fileName;
	utils::makeLower(name);
	for(auto *plugin : plugins) {
		if(plugin->canHandle(name)) {
			printf("Playing with %s\n", plugin->name().c_str());
			player = shared_ptr<ChipPlayer>(plugin->fromFile(fileName));
			break;
		}
	}
	return player;
}


}