#include "URLPlayer.h"
#include "utils.h"
#include "Archive.h"

using namespace std;
using namespace utils;

URLPlayer::URLPlayer(const string &url, PlayerFactory *playerFactory) : webGetter("_cache"), currentPlayer(nullptr), urlJob(nullptr), playerFactory(playerFactory) {
	StringTokenizer st {url, ":"};
	string protocol = "";
	string path;
	int p = 0;

	if(st.noParts() > 1 && st.getString(1).substr(0,2) == "//" && st.getDelim(1) == ':') {
		protocol = st.getString(p++);
		path = st.getString(p).substr(1);
	} else {
		path = st.getString(p);
	}

	if(protocol == "http") {
		string musicUrl = protocol.append(":/").append(path);
		urlJob = webGetter.getURL(musicUrl);
	}
	else {
		printf("Loading file '%s'\n", path.c_str());
		File file(path);
		//currentPlayer = new ModPlayer{file.getPtr(), file.getSize()};
		currentPlayer = playerFactory->fromFile(file);
	}
};


int URLPlayer::getSamples(short *target, int noSamples) override {

	if(!currentPlayer) {
		if(urlJob) {
			if(urlJob->isDone()) {

				string target = urlJob->getFile();

				File file(target);

				if(Archive::canHandle(target)) {

					Archive *a = Archive::open(target, "_cache");
					for(const string &name : *a) {
						printf("%s\n", name.c_str());
						if(playerFactory->canHandle(name)) {
							printf("Can handle\n");
							file = a->extract(name);
							break;
						}
					}
				}
				
				if(file.exists()) {
					printf("Trying %s\n", file.getName().c_str());
					currentPlayer = playerFactory->fromFile(file); //new ModPlayer {file.getPtr(), file.getSize()};
				}
				urlJob = nullptr;
			}
		}

	}
	if(currentPlayer)
		return currentPlayer->getSamples(target, noSamples);
	sleepms(100);
	return 0;
}
