#include <iostream>

#include <map>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <CkSpider.h>
#include <CkStringArray.h>

#include "outbound.hpp"
#include "utils.hpp"

#define NUM_DOMAINS_IN_THREAD 50
#define MAX_HTML_SIZE 5000000
#define MAX_DOMAINS_LIST_SIZE 4000


/*
 * Function:  initialize_thread_strings
 * --------------------
 *  Initialize and reserve memory for strings that will collect the html
 *  
 *  num_threads: chosen number of threads
 *  thread_htmls: strings that will collect the html
 *
 */
void initialize_thread_strings(int num_threads,
							   std::vector<std::string> &thread_htmls) {
	int i;
	// Reserve a little more than the amount necessary to send to file
	for (i = 0; i < num_threads; i++) {
		thread_htmls[i].reserve(1.1*MAX_HTML_SIZE);
		thread_htmls[i] = "|||";
	}
}


/*
 * Function:  fill_thread_links
 * --------------------
 *  Gets best urls from table to send to thread and update seen domains list
 *  
 *  thread_links: vector to be filled with urls
 *  url_prioriy: table of domains and counter  
 *  seedUrl: table of domains and their respective url
 *  spider: collector object 
 *  seenDomains: domains already seen
 *
 */
void fill_thread_links(std::vector<std::string> *thread_links,
					  std::unordered_map<std::string, int> &url_priority,
					  std::unordered_map<std::string, std::string> &seedUrl,
					  CkSpider *spider,
					  CkStringArray *seenDomains) {
    int i;
    std::string url;
    // Until vector is not full, look for the most used domains in the table
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

		// When found, erase it from table and add it to seen domains
    	url = seedUrl[max_url];
		url_priority.erase(max_url);
		seedUrl.erase(max_url);

		thread_links->push_back(url);
        const char *domain = spider->getUrlDomain(url.c_str());
        seenDomains->Append(spider->getBaseDomain(domain));

        // If table is empty, stop
   		if (url_priority.empty()) {
   			break;
   		}
	}
} 


/*
 * Function:  update_counter
 * --------------------
 *  Update table of next domains with links collected 
 *  
 *  spider: collector object 
 *  url_prioriy: table of domains and counter  
 *  seedUrl: table of domains and their respective url
 *  new_outbound_links: collected urls
 *
 */
void update_counter(CkSpider &spider,
					std::unordered_map<std::string, int> &url_priority,
					std::unordered_map<std::string, std::string> &seedUrl,
                    std::vector<std::string> new_outbound_links) {
    // UPDATE OUTBOUND COUNTER
    int i;
	std::string url;
    int n_outlinks = new_outbound_links.size();
    // For each link found, add to counter if possible
    for (i = 0; i < n_outlinks; i++) {
        url = new_outbound_links[i];
        const char *domain = spider.getUrlDomain(url.c_str());
        std::string baseDomain = spider.getBaseDomain(domain);

        // If found url to a domain is better than prior, substitute it
        if ((url_priority.size() < MAX_DOMAINS_LIST_SIZE) && 
        	(baseDomain.length() > 5) && (url.length() > 5)) {
        	url_priority[baseDomain]++;
        	if (url_priority[baseDomain] == 1) {        		
        		seedUrl[baseDomain] = url;
        	} 
        	else if (url_score(url) < url_score(seedUrl[baseDomain])) {
	        	seedUrl[baseDomain] = url;
        	}
        }
    }
}
