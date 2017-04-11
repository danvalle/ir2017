#include <iostream>

#include <map>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <CkSpider.h>
#include <CkStringArray.h>

#include "outbound.hpp"
#include "utils.hpp"

#define NUM_DOMAINS_IN_THREAD 15
#define MAX_HTML_SIZE 5000000
#define MAX_DOMAINS_LIST_SIZE 2000


void initialize_thread_strings(int num_threads, std::vector<std::string> &thread_htmls) {
	int i;
	for (i = 0; i < num_threads; i++) {
		thread_htmls[i].reserve(1.1*MAX_HTML_SIZE);
		thread_htmls[i] = "|||";
	}
}


void update_counter(CkSpider &spider,
					CkStringArray &seenDomains,
					std::unordered_map<std::string, int> &url_priority,
					std::unordered_map<std::string, std::string> &seedUrl,
                    std::vector<std::string> new_outbound_links) {
    // UPDATE OUTBOUND COUNTER
    int i;
	std::string url;
    int n_outlinks = new_outbound_links.size();
    for (i = 0; i < n_outlinks; i++) {
        url = new_outbound_links[i];
        const char *domain = spider.getUrlDomain(url.c_str());
        const char *baseDomain = spider.getBaseDomain(domain);

        if ((seenDomains.Contains(baseDomain) == false) && (url_priority.size() < MAX_DOMAINS_LIST_SIZE)) {
        	url_priority[baseDomain]++;
        	if (url_score(url) < url_score(seedUrl[baseDomain])) {
	        	seedUrl[baseDomain] = url;     		
        	}
        }
    }
}


void fill_thread_links(std::vector<std::string> *thread_links,
					  std::unordered_map<std::string, int> &url_priority,
					  std::unordered_map<std::string, std::string> &seedUrl,
					  CkSpider *spider,
					  CkStringArray *seenDomains) {
    // GET LIST OF DOMAINS TO CRAWL NEXT
    
    // std::cout << "\r\n++++++TABLE++++++\r\n";
		// std::unordered_map<std::string, int>::iterator it;
	// for (it = url_priority.begin(); it != url_priority.end(); it++) {
		// std::cout << it->first << ", "<<it->second << "\r\n";
	// }
    // std::cout << "+++++++++++++++++\r\nLEN: " << url_priority.size();

    int i;
    std::string url;
    for (i = 0; i < NUM_DOMAINS_IN_THREAD; i++) { 
    	std::string max_url;
	    int max_url_counter = 0;
        std::unordered_map<std::string, int>::iterator it;
		for (it = url_priority.begin(); it != url_priority.end(); it++) {
			if (it->second > max_url_counter) {
				max_url = it->first;
				max_url_counter = it->second;
			}  
		}

    	url = seedUrl[max_url];
		url_priority.erase(max_url);
		seedUrl.erase(max_url);

		thread_links->push_back(url);
        const char *domain = spider->getUrlDomain(url.c_str());
        seenDomains->Append(spider->getBaseDomain(domain));


   		if (url_priority.empty()) {
   			break;
   		}
	}

	std::cout << "\r\nMAX: ";
	for (std::vector<std::string>::const_iterator i = thread_links->begin(); i != thread_links->end(); i++) {
	    std::cout << *i << ", ";			
	}
	std::cout << "\r\n";
} 