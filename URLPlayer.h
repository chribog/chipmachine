#ifndef URL_PLAYER_H
#define URL_PLAYER_H

#include "ChipPlayer.h"
#include "WebGetter.h"
#include "PlayerFactory.h"

class URLPlayer : public ChipPlayer {
public:

	URLPlayer(const std::string &url, PlayerFactory *playerFactory);
	int getSamples(int16_t *target, int noSamples) override;
	void seekTo(int song, int seconds) override;
	std::string getMetaData(const std::string &what) override;

private:
	WebGetter webGetter;
	ChipPlayer *currentPlayer;
	WebGetter::Job *urlJob;
	std::mutex m;
	PlayerFactory *playerFactory;
};

#endif // URL_PLAYER_H