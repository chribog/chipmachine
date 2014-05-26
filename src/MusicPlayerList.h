#ifndef MUSIC_PLAYER_LIST_H
#define MUSIC_PLAYER_LIST_H

#include "SongInfo.h"
#include "MusicPlayer.h"

#include <webutils/webgetter.h>

#include <mutex>
#include <cstdint>
#include <deque>

namespace chipmachine {

class MusicPlayerList {
public:
	enum State {
		STOPPED,
		WAITING,
		LOADING,
		STARTED,
		PLAY_STARTED,
		PLAYING
	};

	void addSong(const SongInfo &si);
	void clearSongs();
	void nextSong();
	void playFile(const std::string &fileName);
	State update();
	uint16_t *getSpectrum();
	int spectrumSize();
	SongInfo getInfo(int index = 0);
	int getLength();
	int getPosition();
	int listSize();

	void seek(int song, int seconds = -1);
private:
	void updateInfo();

	MusicPlayer mp;
	std::mutex plMutex;
	std::deque<SongInfo> playList;

	std::atomic<int> files;
	std::string loadedFile;

	WebGetter webgetter { "_files" };

	State state = STOPPED;
	SongInfo currentInfo;

};

} // namespace chipmachine

#endif // MUSIC_PLAYER_LIST_H

