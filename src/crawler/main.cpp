#include <iostream>
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
	CkSpider spider;
    CkStringArray seenDomains;
    seenDomains.put_Unique(true);

    std::unordered_map<std::string, int> url_priority;
    std::unordered_map<std::string, std::string> seedUrl;

    const int num_threads = 1;
    std::thread thr[num_threads];
    std::vector<std::string> thread_htmls(num_threads);
    initialize_thread_strings(num_threads, thread_htmls);

    std::string max_url = "terra.com.br";
    url_priority[max_url]++;
    seedUrl[max_url] = "https://www.terra.com.br/";

	max_url = "r7.com";
    url_priority[max_url]++;
    seedUrl[max_url] = "http://www.r7.com/";
	
	max_url = "bahianoticias.com.br";
    url_priority[max_url]++;
    seedUrl[max_url] = "http://www.bahianoticias.com.br/";


    int total_crawled = 0;
    while (!url_priority.empty()) {
        int i;

		// SEND DOMAINS TO THREADS
        int filled_threads = 0;
        std::vector<std::string> new_outbound_links;
        while ((filled_threads < num_threads) && (!url_priority.empty())) {
        	std::vector<std::string> thread_links;
        	fill_thread_links(&thread_links, url_priority, seedUrl, &spider, &seenDomains);
			
	        thr[filled_threads] = std::thread(crawl_domains,
	        	thread_links,
	        	&new_outbound_links,
	        	&thread_htmls[filled_threads],
				&total_crawled,
				filled_threads);
	        
	        filled_threads++;
		}
		for (i = 0; i < filled_threads; i++) {
	        thr[i].join();
		}

		update_counter(spider, seenDomains, url_priority, seedUrl, new_outbound_links);

	    std::cout << "\r\n\r\n+++++++++++++++++\r\nQUEUE LEN: " << url_priority.size();
		std::cout << "\r\nTotal Crawled: " << total_crawled;
		std::cout << "\r\n+++++++++++++++++\r\n";
    }

    std::cout << "TERMINOU - FILA VAZIA";
    return 0;
}