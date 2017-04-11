#include <iostream>
#include <fstream>

#include <chrono>
#include <regex>
#include <unordered_map>

#include <CkSpider.h>
#include <CkStringArray.h>

#include "utils.hpp"


long get_time() {
	return static_cast<unsigned long> 
		(std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count());
}


int url_score(std::string url) {
	if (url.length() == 0) {
		return 1000;
	}
	
	int score;
	int blocks = std::count(url.begin(), url.end(), '/') + 1;
	score = (url.length()/blocks) + blocks;

	std::smatch m;
	std::regex pos(".br");
	if (std::regex_search (url,m,pos)) {
		score -= 5;
	}  
	std::regex neg("(id=)|(rf=)|(prd=)|(.mypages.)|(.personal.)|(/send)");
	if (std::regex_search (url,m,neg)) {
		score += 10;
	}  

	if (score < 0) {
		score = 0;
	}
	return score;
}


void checkpoint(CkStringArray &seenDomains,
				std::unordered_map<std::string, int> &url_priority,
				std::unordered_map<std::string, std::string> &seedUrl,
				int total_crawled) {

	std::ofstream up;
	up.open("data/checkpoint_url_priority", std::ios_base::out);
	std::unordered_map<std::string, int>::iterator it_prio;
	for (it_prio = url_priority.begin(); it_prio != url_priority.end(); it_prio++) {
		up << it_prio->first << " " << it_prio->second << "\r\n";
	}
	
	std::ofstream su;
	su.open("data/checkpoint_seed_url", std::ios_base::out);
	std::unordered_map<std::string, std::string>::iterator it_seed;
	for (it_seed = seedUrl.begin(); it_seed != seedUrl.end(); it_seed++) {
		su << it_seed->first << " " << it_seed->second << "\r\n";
	}

	std::ofstream tc;
	tc.open("data/checkpoint_total_crawled", std::ios_base::out);
	tc << total_crawled;

	seenDomains.SaveToFile("data/checkpoint_seen_domains");

}

void get_checkpoint(CkStringArray &seenDomains,
					std::unordered_map<std::string, int> &url_priority,
					std::unordered_map<std::string, std::string> &seedUrl,
					int *total_crawled) {

   	seenDomains.LoadFromFile("data/checkpoint_seen_domains");

   	std::ifstream tc("data/checkpoint_total_crawled");
   	int total;
	while (tc >> total) {
		(*total_crawled) = total;
	}

   	std::ifstream up("data/checkpoint_url_priority");
   	std::string a;
   	int b;
   	while (up >> a >> b) {
		url_priority[a] = b;
	}

	std::ifstream su("data/checkpoint_seed_url");
   	std::string c;
   	std::string d;
   	while (su >> c >> d) {
		seedUrl[c] = d;
	}

}