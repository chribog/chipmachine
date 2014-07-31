#include "ChipMachine.h"
#include "PlaylistDatabase.h"

#include <cctype>

using namespace std;
using namespace utils;
using namespace grappix;
using namespace tween;

//#define ENABLE_TELNET

namespace chipmachine {


void ChipMachine::render_item(Rectangle &rec, int y, uint32_t index, bool hilight) {
	string text;
	Color c;
	if(index < playlists.size()) {
		text = format("<%s>", playlists[index]);
		c = hilight ? markColor : Color(0xff6688ff);
	} else {
		auto res = iquery.getResult(index-playlists.size());
		auto parts = split(res, "\t");
		text = format("%s / %s", parts[0], parts[1]);
		c = hilight ? markColor : resultFieldTemplate->color;
	}
	grappix::screen.text(listFont, text, rec.x, rec.y, c, resultFieldTemplate->scale);
}

#define Z 0xff444488
const vector<uint32_t> heart = { 0,Z,Z,0,Z,Z,0,0,
                                 Z,Z,Z,Z,Z,Z,Z,0,
                                 Z,Z,Z,Z,Z,Z,Z,0,
                                 0,Z,Z,Z,Z,Z,0,0,
                                 0,0,Z,Z,Z,0,0,0,
                                 0,0,0,Z,0,0,0,0 };

#undef Z
#define Z 0xff44cccc
const vector<uint32_t> net = { 0,0,Z,Z,Z,0,0,0,
                               0,0,Z,0,Z,0,0,0,
                               Z,Z,Z,0,Z,0,Z,Z,
                               0,0,0,0,Z,0,Z,0,
                               0,0,0,0,Z,Z,Z,0 };
#undef Z


ChipMachine::ChipMachine() : currentScreen(0), eq(SpectrumAnalyzer::eq_slots), starEffect(screen), scrollEffect(screen), songList(this, Rectangle(tv0.x, tv0.y + 28, screen.width() - tv0.x, tv1.y - tv0.y - 28), 20) {

	PlaylistDatabase::getInstance();

	auto path = current_exe_path() + ":" + File::getAppDir();
	LOGD("####### Finding font");
	File ff = File::findFile(path, "data/Bello.otf");
	scrollEffect.set("font", ff.getName());


#ifdef ENABLE_TELNET
	telnet = make_unique<TelnetInterface>(*this);
	telnet->start();
#endif

	for(int i=0; i<3; i++) {
		mainScreen.add(prevInfoField.fields[i]);
		mainScreen.add(currentInfoField.fields[i]);
		mainScreen.add(nextInfoField.fields[i]);
		mainScreen.add(outsideInfoField.fields[i]);
	}

	xinfoField = make_shared<TextField>();
	mainScreen.add(xinfoField);

	nextField = make_shared<TextField>();
	mainScreen.add(nextField);

	timeField = make_shared<TextField>();
	mainScreen.add(timeField);
	lengthField = make_shared<TextField>();
	mainScreen.add(lengthField);
	songField = make_shared<TextField>();
	mainScreen.add(songField);

	auto bm = image::bitmap(8, 6, &heart[0]);
	favTexture = Texture(bm);
	glBindTexture(GL_TEXTURE_2D, favTexture.id());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	bm = image::bitmap(8, 5, &net[0]);
	netTexture = Texture(bm);
	glBindTexture(GL_TEXTURE_2D, netTexture.id());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// SEARCHSCREEN

	iquery = MusicDatabase::getInstance().createQuery();

	resultFieldTemplate = make_shared<TextField>();
	searchField = make_shared<LineEdit>();
	searchField->setPrompt("#");
	searchScreen.add(searchField);
	searchField->visible(false);

	topStatus = make_shared<TextField>();
	searchScreen.add(topStatus);
	topStatus->visible(false);

	setup_rules();

	initLua();

	playlistField = make_shared<TextField>(listFont, "Favorites", tv1.x - 80, tv1.y - 10, 0.5, 0xff888888);
	mainScreen.add(playlistField);

	commandField = make_shared<LineEdit>(font, ">", tv0.x, tv0.y, 1.0, 0xff888888);
	searchScreen.add(commandField);
	commandField->visible(false);

	scrollEffect.set("scrolltext", "Chipmachine Beta 1 -- Begin typing to search -- CRSR UP/DOWN to select -- ENTER to play, SHIFT+ENTER to enque -- CRSR LEFT/RIGHT for subsongs -- F6 for next song -- F5 for pause -- F8 to clear queue -- ESCAPE to clear search text ----- ");
	toastField = make_shared<TextField>(font, "", tv0.x, tv1.y - 134, 2.0, 0x00ffffff);
	renderSet.add(toastField);
	starEffect.fadeIn();

	File f { ".login" };
	if(f.exists())
		userName = f.read();

}

void ChipMachine::set_scrolltext(const std::string &txt) {
	scrollEffect.set("scrolltext", txt);
}

void ChipMachine::initLua() {

	unordered_map<string, string> dbmap;
	lua.registerFunction<void, string, string>("set_db_var", [&](string name, string val) {
		LOGD("%s %s", name, val);
		if(val == "start") {
		} else if(val == "end") {
			MusicDatabase::getInstance().initDatabase(dbmap);
			dbmap.clear();
		} else {
			dbmap[name] = val;
		}
	});

	lua.registerFunction<void, string, uint32_t, string>("set_var", [=](string name, uint32_t index, string val) {
		//LOGD("%s(%d) = %s", name, index, val);
		setVariable(name, index, val);
	});

	auto path = File::getUserDir() + ":" + current_exe_path() + ":" + File::getAppDir();
	File f = File::findFile(path, "lua/db.lua");

	/*{ "lua/db.lua" };
	if(!f.exists()) {
		f.copyFrom("lua/db.lua.orig");
		f.close();
	}*/

	lua.loadFile(f.getName());
	lua.load(R"(
		for a,b in pairs(DB) do
			if type(b) == 'table' then
				set_db_var(a, 'start')
				for a1,b1 in pairs(b) do
					set_db_var(a1, b1)
				end
				set_db_var(a, 'end')
			end
		end
	)");
	MusicDatabase::getInstance().generateIndex();

	File f2 = File::findFile(path, "lua/screen.lua");
	/*File f2 { "lua/screen.lua" };
	if(!f2.exists()) {
		f2.copyFrom("lua/screen.lua.orig");
		f2.close();
	}*/

	lua.set_global("SCREEN_WIDTH", screen.width());
	lua.set_global("SCREEN_HEIGHT", screen.height());

	Resources::getInstance().load<string>(f2.getName() /*"lua/screen.lua" */, [=](shared_ptr<string> contents) {
		lua.load(*contents, "lua/screen.lua");
		lua.load(R"(
			for a,b in pairs(Settings) do
				if type(b) == 'table' then
					for a1,b1 in ipairs(b) do
						set_var(a, a1, b1)
					end
				else
					set_var(a, 0, b)
				end
			end
		)");
	});
}

void ChipMachine::play(const SongInfo &si) {
	player.addSong(si);
	player.nextSong();
}

void ChipMachine::update() {

	static string msg;
	auto m = player.getMeta("message");
	if(m != msg) {
		msg = m;
		// Turn linefeeds into spaces
		replace(m.begin(), m.end(), '\n', ' ');
		// Turn space sequences into single spaces
		auto last = unique(m.begin(), m.end(), [](const char &a, const char &b) -> bool {
			return (a == ' ' && b == ' ');
		});
		m.resize(last - m.begin());
		scrollEffect.set("scrolltext", m);
	}

	if(currentDialog && currentDialog->getParent() == nullptr)
		currentDialog = nullptr;

	update_keys();

	auto state = player.getState();
	//LOGD("STATE %d vs %d %d %d", state, MusicPlayerList::STOPPED, MusicPlayerList::WAITING, MusicPlayerList::PLAY_STARTED);
	if(state == MusicPlayerList::PLAY_STARTED) {
		LOGD("MUSIC STARTING");
		//state = PLAYING;
		currentInfo = player.getInfo();
		LOGD("Prev song %s, new song %s", currentInfoField.getInfo().title, currentInfo.title);
		prevInfoField.setInfo(currentInfoField.getInfo());
		currentInfoField.setInfo(currentInfo);
		currentTune = currentInfo.starttune;
		currentTween.finish();
		auto sc = currentInfoField[0].scale;

		if(currentInfo.numtunes > 0)
			songField->setText(format("[%02d/%02d]", currentTune+1, currentInfo.numtunes));
		else
			songField->setText("");

		auto sub_title = player.getMeta("sub_title");

		int tw = font.get_width(currentInfo.title, sc);

		LOGD("%s vs %s", nextInfoField.path, currentInfoField.path);

		auto f = [=]() {
			xinfoField->setText(sub_title);
			auto d = (tw-(tv1.x-tv0.x-20));
			if(d > 20)
				make_tween().sine().repeating().to(currentInfoField[0].pos.x, currentInfoField[0].pos.x - d).seconds((d+200.0)/200.0);
		};

		auto favorites = PlaylistDatabase::getInstance().getPlaylist(currentPlaylistName);
		auto favsong = find(favorites.begin(), favorites.end(), currentInfo);
		isFavorite = (favsong != favorites.end());

		if(nextInfoField == currentInfoField) {
			currentTween = make_tween().from(prevInfoField, currentInfoField).
			from(currentInfoField, nextInfoField).
			from(nextInfoField, outsideInfoField).seconds(1.5).on_complete(f);
		} else {
			currentTween = make_tween().from(prevInfoField, currentInfoField).
			from(currentInfoField, outsideInfoField).seconds(1.5).on_complete(f);
		}

	}

	if(state == MusicPlayerList::PLAYING || state == MusicPlayerList::STOPPED) {
		auto psz = player.listSize();
		if(psz > 0) {
			auto n = player.getInfo(1);
			if(n.path != currentNextPath) {
				if(n.title == "") {
					n.title = path_filename(urldecode(n.path, ""));
				}

				if(psz == 1)
					nextField->setText("Next");
				else
					nextField->setText(format("Next (%d)", psz));
				nextInfoField.setInfo(n);
				currentNextPath = n.path;
			}
		} else if(nextField->getText() != "") {
			nextInfoField.setInfo(SongInfo());
			nextField->setText("");
		}
	}

	int tune = player.getTune();
	if(currentTune != tune) {
		songField->add = 0.0;
		make_tween().sine().to(songField->add, 1.0).seconds(0.5);
		currentInfo = player.getInfo();
		auto sub_title = player.getMeta("sub_title");
		xinfoField->setText(sub_title);
		currentInfoField.setInfo(currentInfo);
		currentTune = tune;
		songField->setText(format("[%02d/%02d]", currentTune+1, currentInfo.numtunes));
	}

	if(player.playing()) {

		bool party = (player.getPermissions() & MusicPlayerList::PARTYMODE) != 0;
		if(!lockDown && party) {
			lockDown = true;
			make_tween().to(timeField->color, Color(0xffff0000)).seconds(0.5);
		} else if(lockDown && !party) {
			lockDown = false;
			make_tween().to(timeField->color, timeColor).seconds(2.0);
		}


		auto p = player.getPosition();
		int length = player.getLength();
		timeField->setText(format("%02d:%02d", p/60, p%60));
		if(length > 0)
			lengthField->setText(format("(%02d:%02d)", length/60, length%60));
		else
			lengthField->setText("");

		auto sub_title = player.getMeta("sub_title");
		if(sub_title != xinfoField->getText())
			xinfoField->setText(sub_title);
	}

	if(!player.getAllowed()) {
		toast("Not allowed", 1);
	} else
	if(player.hasError()) {
		toast(player.getError(), 1);
	}

	for(int i=0; i<(int)eq.size(); i++) {
		if(!player.isPaused()) {
			if(eq[i] >= 4)
				eq[i]-=2;
			else
				eq[i] = 2;
		}
	}

	if(player.playing()) {
		auto spectrum = player.getSpectrum();
		for(int i=0; i<(int)player.spectrumSize(); i++) {
			if(spectrum[i] > 5) {
				float f = log(spectrum[i]) * spectrumHeight;
				if(f > eq[i])
					eq[i] = f;
			}
		}
	}

}

void ChipMachine::toast(const std::string &txt, int type) {

	static vector<Color> colors = { 0xffffff, 0xff8888, 0x55aa55 }; // Alpha intentionally left at zero

	toastField->setText(txt);
	int tlen = toastField->getWidth();
	toastField->pos.x = tv0.x + ((tv1.x - tv0.x) - tlen) / 2;
	toastField->color = colors[type];

	make_tween().to(toastField->color.alpha, 1.0).seconds(0.25).on_complete([=]() {
		make_tween().delay(1.0).to(toastField->color.alpha, 0.0).seconds(0.25);
	});
}

void ChipMachine::render(uint32_t delta) {

	screen.clear(0xff000000 | bgcolor);

	if(starsOn)
		starEffect.render(delta);
	scrollEffect.render(delta);

	for(int i=0; i<(int)eq.size(); i++) {
		screen.rectangle(spectrumPos.x + (spectrumWidth)*i, spectrumPos.y-eq[i], spectrumWidth-1, eq[i], spectrumColor);
	}

	if(currentScreen == MAIN_SCREEN) {
		mainScreen.render(delta);
		if(isFavorite)
			screen.draw(favTexture, favPos.x, favPos.y, favPos.w, favPos.h, nullptr);
	} else {
		searchScreen.render(delta);
		songList.render();
	}

	if(WebRPC::inProgress() > 0 || WebGetter::inProgress() > 0) {
		screen.draw(netTexture, 2, 2, 8*3, 5*3, nullptr);
	}

	renderSet.render(delta);

	font.update_cache();
	listFont.update_cache();

	screen.flip();
}


}