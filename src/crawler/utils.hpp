#ifndef utils_hpp
#define utils_hpp

///////////////// Functions /////////////////////
long get_time();

int url_score(std::string url);

void checkpoint(CkStringArray &seenDomains,
				std::unordered_map<std::string, int> &url_priority,
				std::unordered_map<std::string, std::string> &seedUrl,
				int total_crawled);

void get_checkpoint(CkStringArray &seenDomains,
					std::unordered_map<std::string, int> &url_priority,
					std::unordered_map<std::string, std::string> &seedUrl,
					int *total_crawled);
////////////////////////////////////////////////
#endif