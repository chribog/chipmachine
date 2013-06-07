#ifndef TELNET_SERVER_H
#define TELNET_SERVER_H

#include "utils.h"

#include <string>
#include <vector>
#include <functional>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <tuple>

#include <netlink/socket.h>
#include <netlink/socket_group.h>

class TelnetInit;


class TelnetServer { //: public Terminal {
public:

	class Session {
	public:

		Session(NL::Socket *socket) : socket(socket), state(NORMAL), localEcho(true) {}
		Session(const Session &s) = delete;
		Session(const Session &&s) : socket(s.socket), state(NORMAL), localEcho(true) {}

		typedef std::function<void(Session&)> Callback;

		void write(const std::vector<int8_t> &data, int len = -1);
		void write(const std::string &text);

		template <class... A>
		void write(const std::string &fmt, A... args) {
			std::string s = utils::format(fmt, args...);
			write(s);
		}

		NL::Socket *getSocket() const { return socket; }
		void handleIndata(std::vector<int8_t> &buffer);

		char getChar();
		bool hasChar() const;
		std::string getLine();
		void startThread(Callback callback);

		

	private:
		NL::Socket *socket;

		enum State {
			NORMAL,
			CR_READ,
			FOUND_IAC,
			OPTION,
			SUB_OPTION,
			FOUND_IAC_SUB	
		};

		State state;
		int8_t option;
		std::vector<int8_t> optionData;
		std::vector<int8_t> inBuffer;
		mutable std::mutex inMutex;
		std::thread sessionThread;
		bool localEcho;

		void setOption(int opt, int val) {
			LOGD("Set option %d %d", opt, val);
		}


		void handleOptionData() {
			LOGD("optionData %d : %d bytes", option, optionData.size());
			optionData.resize(0);
		}
	};

	enum {
		IAC = -1, //ff
		DONT = -2, //fe
		DO = -3, //fd
		WONT = -4, //fc
		WILL = -5,	 //fb
		SB = -6,
		GA = -7,
		EL = -8,
		EC = -9,
		AYT = -10,
		AO = -11,
		IP = -12,
		BRK = -13,
		DM = -14,
		NOP = -15,
		SE = -16,

		LF = 10,
		CR = 13
	};

	enum {
		TRANSMIT_BINARY = 0,
		ECHO = 1,
		SUPRESS_GO_AHEAD = 3,
		TERMINAL_TYPE = 24,
		WINDOW_SIZE = 31
	};

	TelnetServer(int port);
	void run();
	void runThread();

	void setOnConnect(Session::Callback callback) {
		connectCallback = callback;
	}

	Session& getSession(NL::Socket* socket) {

		for(auto &s : sessions) {
			if(s.getSocket() == socket)
				return s;
		}
		return no_session;
	}



private:

	class OnAccept : public NL::SocketGroupCmd {
		void exec(NL::Socket* socket, NL::SocketGroup* group, void* reference) override;
	};

	class OnRead : public NL::SocketGroupCmd {
		void exec(NL::Socket* socket, NL::SocketGroup* group, void* reference) override;
	};


	class OnDisconnect : public NL::SocketGroupCmd {
		void exec(NL::Socket* socket, NL::SocketGroup* group, void* reference) override;
	};


	TelnetInit *init;
	Session no_session;
	NL::Socket socketServer;

	OnAccept onAccept;
	OnRead onRead;
	OnDisconnect onDisconnect;

	NL::SocketGroup group;

	std::thread mainThread;

	Session::Callback connectCallback;

	std::vector<int8_t> buffer;

	std::vector<Session> sessions;
};



#endif // TELNET_SERVER_H
