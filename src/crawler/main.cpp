#include <iostream>
#include <fstream>
#include <thread>

#include <map>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <CkSpider.h>
#include <CkStringArray.h>

#include "outbound.hpp"
#include "indomain.hpp"
#include "utils.hpp"


int main() {
	// Initialize variable
    int total_crawled = 0;
    const int num_threads = 50;
    std::thread thr[num_threads];
    std::vector<std::string> thread_htmls(num_threads);
    initialize_thread_strings(num_threads, thread_htmls);

	CkSpider spider;
    CkStringArray seenDomains;
    seenDomains.put_Unique(true);

    std::unordered_map<std::string, int> url_priority;
    std::unordered_map<std::string, std::string> seedUrl;

    // Look for checkpoints. If found, initialize it.
    std::ifstream cp("data/checkpoint_url_priority");
    if (cp) {
    	std::cout << "CHECKPOINT FOUND" << "\r\n";
    	get_checkpoint(seenDomains, url_priority, seedUrl, &total_crawled);
    } else {
	    std::string max_url = "bb.com.br";
	    url_priority[max_url]++;
	    seedUrl[max_url] = "http://www.bb.com.br/pbb/pagina-inicial#/";

		max_url = "bradesco.com.br";
	    url_priority[max_url]++;
	    seedUrl[max_url] = "https://banco.bradesco/html/classic/index.shtm";

		max_url = "itau.com.br";
	    url_priority[max_url]++;
	    seedUrl[max_url] = "https://www.itau.com.br/";

		max_url = "caixa.gov.br";
	    url_priority[max_url]++;
	    seedUrl[max_url] = "http://www.caixa.gov.br/Paginas/home-caixa.aspx";

	    max_url = "santander.com.br";
	    url_priority[max_url]++;
	    seedUrl[max_url] = "https://www.santander.com.br/br/";
    }

    // Will crawl until there is no more domains in the table
    long start_time = get_time();
    while (!url_priority.empty()) {
        int i;

		// Will fill the maximum amount of threads it can
        int filled_threads = 0;
        std::vector<std::string> new_outbound_links;
        while ((filled_threads < num_threads) && (!url_priority.empty())) {
        	// Get the best links in the table to send to the next thread
        	std::vector<std::string> thread_links;
        	fill_thread_links(&thread_links, url_priority, seedUrl, &spider, &seenDomains);
			
			// Send domains to thread, so it can independently crawl them
	        thr[filled_threads] = std::thread(crawl_domains,
	        	thread_links,
	        	&seenDomains,
	        	&new_outbound_links,
	        	&thread_htmls[filled_threads],
				&total_crawled,
				filled_threads);
	        
	        filled_threads++;
		}
		// Wait until all threads have finished
		for (i = 0; i < filled_threads; i++) {
	        thr[i].join();
		}

		// Update table to start next cicle
		update_counter(spider, url_priority, seedUrl, new_outbound_links);

		// Create checkpoint
		std::cout << "\r\nCREATING CHECKPOINT\r\n";
		checkpoint(seenDomains, url_priority, seedUrl, total_crawled);
	    std::cout << "\r\n+++++++++++++++++\r\nQueue Len: " << url_priority.size();
		std::cout << "\r\nTotal Pages Crawled: " << total_crawled;
	    std::cout << "\r\nTotal Domains Crawled: " << seenDomains.get_Length();
		std::cout << "\r\nTime: " << get_time() - start_time;
		std::cout << "\r\n+++++++++++++++++\r\n";
    }

    return 0;
}