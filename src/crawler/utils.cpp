#include <chrono>
#include <regex>

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

