#ifndef UTILS_H
#define UTILS_H

#include <sys/stat.h>
#include <stdint.h>
#include <typeinfo>
#include <cstdio>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>

//#include "utils/format.h"

namespace utils {

typedef unsigned int uint;

const char path_separator = '/';

class io_exception : public std::exception {
public:
	io_exception(const char *ptr = "IO Exception") : msg(ptr) {}
	virtual const char *what() const throw() { return msg; }
private:
	const char *msg;
};

class file_not_found_exception : public std::exception {
public:
	virtual const char *what() const throw() { return "File not found"; }
};

#define THROW(e, args...) throw e(args, __FILE__, __LINE__)

class File {
public:
	File();
	File(const std::string &name);
	~File() {
		if(readFP)
			fclose(readFP);
		if(writeFP)
			fclose(writeFP);
	}
	void write(const uint8_t *data, int size); // throw(io_exception);
	void write(const std::string &text);
	void close();

	bool exists();
	static bool exists(const std::string &fileName);
	uint8_t *getPtr();
	const std::string &getName() const { return fileName; }
	int getSize() const { 
		if(size < 0) {
			struct stat ss;
			if(stat(fileName.c_str(), &ss) != 0)
				throw io_exception("Could not stat file");
			size = ss.st_size;
		}
		return size;
	}
	std::vector<std::string> getLines();
	void remove() {
		if(std::remove(fileName.c_str()) != 0)
			throw io_exception("Could not delete file");
	}

	void copyFrom(File &otherFile);

	int read(uint8_t *target, int len);

	void seek(int where);
	template <typename T> int read(T *target, int count) {
		open();
		return fread(target, sizeof(T), count, readFP);
	}

	template <typename T> T read() {
		open();
		T temp;
		if(fread(&temp, sizeof(T), 1, readFP) > 0)
			return temp;
		throw io_exception{};
	}


private:
	void readAll(); // throw(file_not_found_exception, io_exception);
	void open();

	std::string fileName;
	std::vector<uint8_t> data;
	mutable int size;
	bool loaded;
	FILE *writeFP;
	FILE *readFP;
};

class StringTokenizer {
public:
	StringTokenizer(const std::string &s, const std::string &delim);
	int noParts() { return args.size(); }
	const std::string &getString(int no) { return args[no]; }
	const char getDelim(int no) { return delims[no]; }
private:
	std::vector<std::string> args;
	std::vector<char> delims;
};

std::vector<std::string> split(const std::string &s, const std::string &delim = " ");

std::string urlencode(const std::string &s, const std::string &chars);
std::string urldecode(const std::string &s, const std::string &chars);

void sleepms(uint ms);
void makedir(const std::string &name);
void makedirs(const std::string &name);

bool endsWith(const std::string &name, const std::string &ext);
void makeLower(std::string &s);

std::string path_basename(const std::string &name);
std::string path_directory(const std::string &name);
std::string path_filename(const std::string &name);
std::string path_extention(const std::string &name);
std::string path_suffix(const std::string &name);
std::string path_prefix(const std::string &name);
std::string utf8_encode(const std::string &s);

// SLICE

template <class InputIterator> class slice {
public:
	slice(InputIterator start, InputIterator stop) : start(start), stop(stop) {}

	InputIterator begin() const {
		return start;
	}

	InputIterator end() const {
		return stop;
		//return const_iterator(*this, end);
	}

private:
	InputIterator start;
	InputIterator stop;

};

template <class T> slice<typename T::const_iterator> make_slice(T &vec, int start, int len) {
	return slice<typename T::const_iterator>(vec.begin() + start, vec.begin() + start + len);
}

// VAR

class illegal_conversion_exception : public std::exception {
public:
	virtual const char *what() const throw() { return "Illegal conversion"; }
};

class Holder {
public:
	virtual const std::type_info& getType() = 0;
	virtual void *getValue() = 0;
};

template <class T> class VHolder : public Holder {
public:
	VHolder(const T &t) : value(t) {}

	virtual const std::type_info &getType() {
		return typeid(value);
	}

	virtual void *getValue() {
		return (void*)&value;
	}

private:
	T value;
};

template <> class VHolder<const char *> : public Holder {
public:
	VHolder(const char *t) : value(t) {
	}

	virtual const std::type_info &getType() {
		return typeid(value);
	}

	virtual void *getValue() {
		return (void*)&value;
	}

private:
	std::string value;
};


class var {
public:
	template <class T> var(T t) {
		holder = new VHolder<T>(t);
	}

	operator int() {
		if(holder->getType() == typeid(int)) {
			return *((int*)holder->getValue());
		} else if(holder->getType() == typeid(std::string) || holder->getType() == typeid(std::string)) {
			const std::string &s = *((std::string*)holder->getValue());
			char *endptr = nullptr;
			int i = strtol(s.c_str(), &endptr, 0);
			if(endptr == nullptr || *endptr == 0)
				return i;
		}
		throw illegal_conversion_exception();
	}

	operator std::string() {
		if(holder->getType() == typeid(std::string)) {
			return *((std::string*)holder->getValue());
		} else if(holder->getType() == typeid(int)) {
			int i = *((int*)holder->getValue());
			return std::to_string(i);
		}
		throw illegal_conversion_exception();	
	}

private:
	Holder *holder;
};


};

#endif // UTILS_H
