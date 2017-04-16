#include <iostream>
#include <fstream>

#include <chrono>
#include <regex>
#include <unordered_map>

#include <CkStringArray.h>

#include "utils.hpp"


/*
 * Function:  get_time
 * --------------------
 *  Gets current timestamp
 *  
 *  returns: current timestamp
 */
long get_time() {
	return static_cast<unsigned long> 
		(std::chrono::duration_cast<std::chrono::milliseconds>
			(std::chrono::system_clock::now().time_since_epoch()).count());
}


/*
 * Function:  url_score
 * --------------------
 *  Computes the score for a url
 *  
 *  url: Url to be computed
 *
 *  returns: value that represents the score of the url
 */
int url_score(std::string url) {
	// Empty urls
	if (url.length() == 0) {
		return 1000;
	}
	
	// Score uses length and amount of blocks in the url
	int score;
	int blocks = std::count(url.begin(), url.end(), '/') + 1;
	score = (url.length()/blocks) + blocks;

	// Give of remove points for patterns
	std::smatch m;
	std::regex pos(".br");
	if (std::regex_search (url,m,pos)) {
		score -= 5;
	}  
	std::regex neg("(id=)|(rf=)|(prd=)|(.mypages.)|(.personal.)|(/send)|(.png)");
	if (std::regex_search (url,m,neg)) {
		score += 10;
	}  

	if (score < 0) {
		score = 0;
	}
	return score;
}


/*
 * Function:  checkpoint
 * --------------------
 *  Saves the current state of the Crawler so it can be restarted
 *  	in the same point 
 *  
 *  seenDomains: Domains that have been used before
 *	url_priority: Table with the domains that will be crawled and
 *					their priority
 *	seedUrl: Url related to every domain in url_priority
 *  total_crawled: Amount of pages seen
 *
 */
void checkpoint(CkStringArray &seenDomains,
				std::unordered_map<std::string, int> &url_priority,
				std::unordered_map<std::string, std::string> &seedUrl,
				int total_crawled) {

	// Write domains and counter value in each line
	std::ofstream up;
	up.open("data/checkpoint_url_priority", std::ios_base::out);
	std::unordered_map<std::string, int>::iterator it_prio;
	for (it_prio = url_priority.begin(); it_prio != url_priority.end(); it_prio++) {
		up << it_prio->first << " " << it_prio->second << "\r\n";
	}
	
	// Write domains from priority and its url
	std::ofstream su;
	su.open("data/checkpoint_seed_url", std::ios_base::out);
	std::unordered_map<std::string, std::string>::iterator it_seed;
	for (it_seed = seedUrl.begin(); it_seed != seedUrl.end(); it_seed++) {
		su << it_seed->first << " " << it_seed->second << "\r\n";
	}

	// Keep track of the amount of pages already crawled
	std::ofstream tc;
	tc.open("data/checkpoint_total_crawled", std::ios_base::out);
	tc << total_crawled;

	// Save list of domains already crawled
	seenDomains.SaveToFile("data/checkpoint_seen_domains");

}


/*
 * Function:  get_checkpoint
 * --------------------
 *  Load the state of the last Crawler saved
 *  
 *  seenDomains: Domains that have been used before
 *	url_priority: Table with the domains that will be crawled and
 *					their priority
 *	seedUrl: Url related to every domain in url_priority
 *  total_crawled: Amount of pages seen
 *
 */
void get_checkpoint(CkStringArray &seenDomains,
					std::unordered_map<std::string, int> &url_priority,
					std::unordered_map<std::string, std::string> &seedUrl,
					int *total_crawled) {

	// Load domains already seen
   	seenDomains.LoadFromFile("data/checkpoint_seen_domains");

   	// Get amount of pages crawled
   	std::ifstream tc("data/checkpoint_total_crawled");
   	int total;
	while (tc >> total) {
		(*total_crawled) = total;
	}

	// Load table of domains and counter
   	std::ifstream up("data/checkpoint_url_priority");
   	std::string a;
   	int b;
   	while (up >> a >> b) {
		url_priority[a] = b;
	}

	// Load urls from domains 
	std::ifstream su("data/checkpoint_seed_url");
   	std::string c;
   	std::string d;
   	while (su >> c >> d) {
		seedUrl[c] = d;
	}

}