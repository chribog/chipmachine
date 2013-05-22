
#include "WebGetter.h"

#include <curl/curl.h>

using namespace utils;
using namespace std;

WebGetter::Job::Job(const string &url, const string &targetDir) : loaded(false), targetDir(targetDir), fp(nullptr) {
	printf("Job created");
	jobThread = thread {&Job::urlGet, this, url};
}

bool WebGetter::Job::isDone() { 
	bool l;
	m.lock();
	l = loaded;
	m.unlock();
	return l;
}

string WebGetter::Job::getFile() {
	return target;
}

void WebGetter::Job::urlGet(string url) {

	target = targetDir + "/" + urlencode(url, ":/\\?;");

	printf("Getting %s -> %s\n", url.c_str(), target.c_str());

	CURL *curl;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
	curl_easy_setopt(curl, CURLOPT_WRITEHEADER, this);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerFunc);
	curl_easy_perform(curl);

	if(fp)
		fclose(fp);

	m.lock();
	loaded = true;
	m.unlock();
}

size_t WebGetter::Job::writeFunc(void *ptr, size_t size, size_t nmemb, void *userdata) {
	Job *job = (Job*)userdata;
	if(!job->fp) {
		job->fp = fopen(job->target.c_str(), "wb");
		printf("Opened %s to %p\n", job->target.c_str(), job->fp);
	}
	if(job->fp) {
		fwrite(ptr, size, nmemb, job->fp); 
	}
	return size * nmemb;
}

size_t WebGetter::Job::headerFunc(void *ptr, size_t size, size_t nmemb, void *userdata) {
	int sz = size * nmemb;
	char *text = (char*)ptr;
	Job *job = (Job*)userdata;

	while(sz > 0 && (text[sz-1] == '\n' || text[sz-1] == '\r'))
		sz--;

	string line = string(text, sz);

	printf("HEADER:'%s'\n", line.c_str());

	if(line.substr(0, 9) == "Location:") {
		string newUrl = line.substr(10);
		printf("Newurl %s\n", newUrl.c_str());
		job->target = job->targetDir + "/" + urlencode(newUrl, ":/\\?;");
	}


	//delete [] tmp;
	return size *nmemb;
}

WebGetter::WebGetter(const string &workDir) : workDir(workDir) {
	makedir(workDir.c_str());
}


WebGetter::Job *WebGetter::getURL(const string &url) {
	return new Job(url, workDir);
}
