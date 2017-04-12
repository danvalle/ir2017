#include <iostream>
#include <fstream>

#include <map>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <math.h>
#include <CkSpider.h>
#include <CkStringArray.h>

#include "outbound.hpp"
#include "utils.hpp"


#define MAX_HTML_SIZE 5000000


void write_html(CkSpider &spider, std::string *htmls, int thread_num) {
    std::string new_html = spider.lastHtml();
    if (new_html.length() > 0) {
    	std::replace(new_html.begin(), new_html.end(), '|', ' ');
    	*htmls += " ";
    	*htmls += spider.lastUrl();
    	*htmls += " | ";
    	*htmls += new_html;
    	*htmls += " |||";
    	if (htmls->size() > MAX_HTML_SIZE) {
    		std::ofstream outfile;
    		outfile.open("data/html_"+std::to_string(thread_num)+".txt", std::ios_base::app);
    		outfile << *htmls;

    		*htmls = "";
	    }
    }
}


int get_valid_domain(int domain_n, int num_domains, std::vector<int> inside_crawl) {
	while (inside_crawl[domain_n] < 0) {
		domain_n = (domain_n + 1) % num_domains;
	}
	return domain_n;
}


void collect_outbound_links(CkSpider &spider, std::vector<std::string> *new_outbound_links, CkStringArray *seenDomains) {
    int i;
    std::string url;
    const char *domain;
    std::string baseDomain;
    int n_outlinks = spider.get_NumOutboundLinks();
    for (i = 0; i < n_outlinks; i++) {
        url = spider.getOutboundLink(i);
        domain = spider.getUrlDomain(url.c_str());
        baseDomain = spider.getBaseDomain(domain);
        if ((url.find_first_of(' ') == std::string::npos) &&
            (seenDomains->Contains(baseDomain.c_str()) == false) &&
            (new_outbound_links->size() < 100000)) {
            new_outbound_links->push_back(url);
        }
    }
}


void crawl_domains(std::vector<std::string> thread_links,
                   CkStringArray *seenDomains,
				   std::vector<std::string> *new_outbound_links,		
				   std::string *htmls,
				   int *total_crawled,
				   int thread_num) {
    // CRAWL IN LINKS
    CkSpider spider;

    int i;
    int domain_n = 0;
    int complete_domains = 0;
    int num_domains = thread_links.size();
    bool success;
    std::string url;
    std::vector<std::string> used;
    std::vector<int> inside_crawl(num_domains, 0);
    std::vector<long> last_crawled_time(num_domains, 0);
    long same_domain_wait_time = 30000;


    //  PRIORITY QUEUE TO GET BEST INLINK TO CRAWL
    priority_queue inlink_queue;
    std::vector<priority_queue> inlink_queues;
    for (i=0; i<num_domains;i++) {
        inlink_queues.push_back(inlink_queue);
    }


    while (complete_domains < num_domains) {
    	domain_n = get_valid_domain(domain_n, num_domains, inside_crawl);
        url = thread_links[domain_n];
        spider.Initialize(url.c_str());
		spider.fetchRobotsText();

        // SLEEP IF LESS THAN 30s
    	long sleep_time = last_crawled_time[domain_n] + same_domain_wait_time;
    	sleep_time -= get_time();
    	if (sleep_time > 0) {
	    	std::cout << "Sleep " << sleep_time << "\r\n";
            spider.SleepMs(sleep_time);
    	}
    
        // Crawl page and get all the info
		last_crawled_time[domain_n] = get_time();

        success = spider.CrawlNext();
        if (success == true) {
	        (*total_crawled)++;
	        collect_outbound_links(spider, new_outbound_links, seenDomains);
			// std::cout << "2^Pages in Domain: " << pow(2,inside_crawl[domain_n]) << " <= " << spider.get_NumOutboundLinks() << "\r\n";
			write_html(spider, htmls, thread_num);
        }

        // If it is a good source of outboundlinks, prepare queue to crawl other page inside
        if ((success == true) && 
            (pow(2, inside_crawl[domain_n]) <= spider.get_NumOutboundLinks())) {

			used.push_back(url);
			inside_crawl[domain_n]++;

            int size = spider.get_NumUnspidered();
            // std::cout << "Page Inlinks: " << size << "\r\n";
            for (int i = 0; i < size; i++) {
                url = spider.getUnspideredUrl(0);
                spider.SkipUnspidered(0);

                inlink_queues[domain_n].add(url_score(url), url);
            }

            // Get next inlink to crawl. Finish if queue is empty.
            if (!inlink_queues[domain_n].is_empty()) {
                std::string next_url = inlink_queues[domain_n].remove();
                while (std::find(used.begin(), used.end(), next_url) != used.end()) {
                	if (inlink_queues[domain_n].is_empty()) {
                		next_url = "";
                		inside_crawl[domain_n] = -1;
                		complete_domains++;
                		break;
                	}
                    next_url = inlink_queues[domain_n].remove();
                }         
                thread_links[domain_n] = next_url;            
            }

        } else {
            complete_domains++;
            inside_crawl[domain_n] = -1;
        }
    	
        domain_n = (domain_n + 1) % num_domains;
    	spider.ClearOutboundLinks();
    }
}
