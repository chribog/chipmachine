#ifndef REMOTE_LOADER_H
#define REMOTE_LOADER_H

#include <webutils/webgetter.h>
#include <coreutils/file.h>

#include <string>
#include <functional>
#include <unordered_map>

class RemoteLoader {
public:

	RemoteLoader() : webgetter(utils::File::getCacheDir() + "_webfiles") {}

	void registerSource(const std::string &name, const std::string url, const std::string local_dir);

	bool load(const std::string &path, std::function<void(utils::File)> done_cb);

	static RemoteLoader& getInstance() {
		static RemoteLoader loader;
		return loader;
	}

private:

	struct Source {
		Source() {}
		Source(const std::string &url, const std::string &ld) : url(url), local_dir(ld) {}
		std::string url;
		std::string local_dir;
	};

	std::unordered_map<std::string, Source> sources;

	WebGetter webgetter;

};

#endif // REMOTE_LOADER_H
