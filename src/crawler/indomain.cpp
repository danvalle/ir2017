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


/*
 * Function:  write_html
 * --------------------
 *  Writes the url and html from pages in file 
 *  
 *  spider: collector object 
 *  htmls: url and html organized in pattern
 *  thread_num: number of the thread that is writing
 *
 */
void write_html(CkSpider &spider, std::string *htmls, int thread_num) {
    std::string new_html = spider.lastHtml();
    // If valid, add to html
    if (new_html.length() > 0) {
    	std::replace(new_html.begin(), new_html.end(), '|', ' ');
    	*htmls += " ";
    	*htmls += spider.lastUrl();
    	*htmls += " | ";
    	*htmls += new_html;
    	*htmls += " |||";
    	// When it has collected enough, write into file
    	if (htmls->size() > MAX_HTML_SIZE) {
    		std::ofstream outfile;
    		outfile.open("data/html_"+std::to_string(thread_num)+".txt",
    					 std::ios_base::app);
    		outfile << *htmls;

    		*htmls = "";
	    }
    }
}


/*
 * Function:  get_valid_domain
 * --------------------
 *  Look for the next valid domain to crawl inside 
 *  
 *  domain_n: current domain
 *  num_domains: number of domains  
 *  inside_crawl: list of depth and validity in each domains
 *
 *  return: next valid domain to crawl
 */
int get_valid_domain(int domain_n, int num_domains, 
					 std::vector<int> inside_crawl) {
	// While domain is invalid, look for next one
	while (inside_crawl[domain_n] < 0) {
		domain_n = (domain_n + 1) % num_domains;
	}
	return domain_n;
}


/*
 * Function:  collect_outbound_links
 * --------------------
 *  Collect ountbound urls found in a page 
 *  
 *  spider: collector object 
 *  new_outbound_links: keep all outbound urls found
 *
 */
void collect_outbound_links(CkSpider &spider,
							std::vector<std::string> *new_outbound_links,
							CkStringArray *seenDomains) {
    int i;
    std::string url;
    const char *domain;
    std::string baseDomain;
    // For each outbound link found, get its base domain
    int n_outlinks = spider.get_NumOutboundLinks();
    for (i = 0; i < n_outlinks; i++) {
        url = spider.getOutboundLink(i);
        domain = spider.getUrlDomain(url.c_str());
        baseDomain = spider.getBaseDomain(domain);
        // If there is no space and it has not been seen before, collect
        if ((url.find_first_of(' ') == std::string::npos) &&
            (seenDomains->Contains(baseDomain.c_str()) == false) &&
            (new_outbound_links->size() < 50000)) {
            new_outbound_links->push_back(url);
        }
    }
}


/*
 * Function:  crawl_domains
 * --------------------
 *  Craw pages inside sent domains and collect html and
 *  	links to future domains
 *  
 *  thread_links: links of domains to be crawled 
 *  seenDomains: domains already crawled 
 *  new_outbound_links: keep all outbound urls found
 *  htmls: collected url and html to be written
 *  total_crawled: number of total pages crawled
 *  thread_num: number of the thread
 *
 */
void crawl_domains(std::vector<std::string> thread_links,
                   CkStringArray *seenDomains,
				   std::vector<std::string> *new_outbound_links,		
				   std::string *htmls,
				   int *total_crawled,
				   int thread_num) {
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

    // Priorit queue to get best inside link to crawl
    priority_queue inlink_queue;
    std::vector<priority_queue> inlink_queues;
    for (i=0; i<num_domains;i++) {
        inlink_queues.push_back(inlink_queue);
    }

    // While there are available domains to crawl
    while (complete_domains < num_domains) {
    	// Get first valid domain, initalize it and fetch robots.txt
    	domain_n = get_valid_domain(domain_n, num_domains, inside_crawl);
        url = thread_links[domain_n];
        spider.Initialize(url.c_str());
		spider.fetchRobotsText();

        // If last crawl in the same domain was in less than 30s, wait
    	long sleep_time = last_crawled_time[domain_n] + same_domain_wait_time;
    	sleep_time -= get_time();
    	if (sleep_time > 0) {
            spider.SleepMs(sleep_time);
    	}
    
        // Crawl page and collect all the information
		last_crawled_time[domain_n] = get_time();
        success = spider.CrawlNext();
        if (success == true) {
	        (*total_crawled)++;
	        collect_outbound_links(spider, new_outbound_links, seenDomains);
			write_html(spider, htmls, thread_num);
        }

        // If it is a good source of outboundlinks, prepare queue to crawl other page inside
        if ((success == true) && 
            (pow(2, inside_crawl[domain_n]) <= spider.get_NumOutboundLinks())) {

			used.push_back(url);
			inside_crawl[domain_n]++;

			// Sort inside links by score
            int size = spider.get_NumUnspidered();
            for (int i = 0; i < size; i++) {
                url = spider.getUnspideredUrl(0);
                spider.SkipUnspidered(0);

                inlink_queues[domain_n].add(url_score(url), url);
            }

            // Get next inside link to crawl adn finish if queue is empty.
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
        // If crawl was not successful, stop crawling that domain
        } else {
            complete_domains++;
            inside_crawl[domain_n] = -1;
        }
    	
        domain_n = (domain_n + 1) % num_domains;
    	spider.ClearOutboundLinks();
    }
}
