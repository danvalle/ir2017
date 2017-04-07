#include <iostream>
#include <thread>

#include <map>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <math.h>
#include <CkSpider.h>
#include <CkStringArray.h>



class priority_queue {
    std::map<int, std::vector<std::string>> queue;
public:
    void add(int priority, std::string str) {
        if(!queue[priority].empty()) {
            queue[priority].push_back(str);
        } else {
            std::vector<std::string> vec;
            vec.push_back(str);
            queue[priority] = vec;
        }
    }
    std::string remove() {
        if(!queue.begin()->second.empty()) {
            std::string temp = queue.begin()->second[0];
            queue.begin()->second.erase(queue.begin()->second.begin());
            if(queue.begin()->second.empty()) {
                queue.erase(queue.begin());
            }
            return temp;
        }
        std::cout << "ERROR: QUEUE EMPTY!\r\n";
        return "";
    }
    bool is_empty() {
    	if(!queue.empty()) {
    		return false;
    	} else {
    		return true;
    	}
    }
};



void fill_thread_links(std::vector<std::string> *thread_links,
					  std::unordered_map<std::string, int> &url_priority,
					  std::unordered_map<std::string, std::string> &seedUrl,
					  CkSpider *spider,
					  CkStringArray *seenDomains) {
    // GET LIST OF DOMAINS TO CRAWL NEXT
    std::cout << "++++++TABLE++++++\r\n";
		std::unordered_map<std::string, int>::iterator it;
	for (it = url_priority.begin(); it != url_priority.end(); it++) {
		std::cout << it->first << ", "<<it->second << "\r\n";
	}
    std::cout << "+++++++++++++++++\r\nLEN: " << url_priority.size();

    int i;
    std::string url;
    for (i = 0; i < 5; i++) { 
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

} 


void crawl_domains(std::vector<std::string> thread_links) {
    // CRAWL IN LINKS
    CkSpider spider;

    int i;
    bool success;
    priority_queue inlink_queue;
    std::vector<std::string> used;
	std::string url = thread_links.at(0);

	spider.Initialize(url.c_str());
	std::cout << "\r\n==================\r\nCrawling " << url << "\r\n";
	for (i = 0; pow(2, i) <= spider.get_NumOutboundLinks() + 1; i++) {

        success = spider.CrawlNext();
        if (success == true) {
	        std::cout << "Keep inside: " << pow(2, i) << " <= " << spider.get_NumOutboundLinks()+1 << "\r\n";

        	int size = spider.get_NumUnspidered();
        	std::cout << "Page Inlinks: " << size << "\r\n";
			for (int i = 0; i < size; i++) {
				url = spider.getUnspideredUrl(0);
				spider.SkipUnspidered(0);
				inlink_queue.add(url.length(), url);
			}

			if (!inlink_queue.is_empty()) {
				std::string smallest_url = inlink_queue.remove();
				while (std::find(used.begin(), used.end(), smallest_url) != used.end()) {
					smallest_url = inlink_queue.remove();
					std::cout << "Exists -  " << smallest_url << "\r\n";
				}
				used.push_back(smallest_url);
				spider.AddUnspidered(smallest_url.c_str());
				std::cout << "Inside crawl - " << smallest_url << "\r\n";
			}

            if (spider.get_LastFromCache() != true) {
                spider.SleepMs(1000);
            }
        }
        else {
            break;
        }
    }

    spider.ClearOutboundLinks();
}




void update_counter(CkSpider &spider,
					CkStringArray &seenDomains,
					std::unordered_map<std::string, int> &url_priority,
					std::unordered_map<std::string, std::string> &seedUrl) {
    // UPDATE OUTBOUND COUNTER
    int i;
	std::string url;
    int n_outlinks = spider.get_NumOutboundLinks();
    std::cout << "\r\nOUTBOUND:\r\n";
    for (i = 0; i < n_outlinks; i++) {
        url = spider.getOutboundLink(i);
        const char *domain = spider.getUrlDomain(url.c_str());
        const char *baseDomain = spider.getBaseDomain(domain);
   	
        if (seenDomains.Contains(baseDomain) == false) {
        	std::cout << baseDomain << " : " << url << "\r\n";
        	url_priority[baseDomain]++;
        	seedUrl[baseDomain] = url; 
        }
    }
}




int main() {
	CkSpider spider;
    CkStringArray seenDomains;
    seenDomains.put_Unique(true);

    std::unordered_map<std::string, int> url_priority;
    std::unordered_map<std::string, std::string> seedUrl;


    // spider.AddAvoidOutboundLinkPattern("*?id=*");
    // spider.AddAvoidOutboundLinkPattern("*?rf=*");
    // spider.AddAvoidOutboundLinkPattern("*?prd=*");
    // spider.AddAvoidOutboundLinkPattern("*.mypages.*");
    // spider.AddAvoidOutboundLinkPattern("*.personal.*");

    // spider.put_CacheDir("/home/dan/spiderCache/");
    // spider.put_FetchFromCache(true);
    // spider.put_UpdateCache(true);

    const int num_threads = 2;
    std::thread thr[num_threads];


    std::string max_url = "terra.com.br";
    url_priority[max_url]++;
    seedUrl[max_url] = "https://www.terra.com.br/";

	max_url = "r7.com";
    url_priority[max_url]++;
    seedUrl[max_url] = "http://www.r7.com/";
	
	max_url = "bahianoticias.com.br";
    url_priority[max_url]++;
    seedUrl[max_url] = "http://www.bahianoticias.com.br/";


    while (!url_priority.empty()) {
       int i;
    	// const char *robotsText = 0;
	    // robotsText = spider.fetchRobotsText();
	    // std::cout << robotsText << "\r\n";


 		// SEND DOMAINS TO THREADS
       	int filled_threads = 0;
        while ((filled_threads < num_threads) && (!url_priority.empty())) {
        	std::vector<std::string> thread_links;
        	fill_thread_links(&thread_links, url_priority, seedUrl, &spider, &seenDomains);
	        thr[filled_threads] = std::thread(crawl_domains, thread_links);
	        filled_threads++;
		}

		for (i = 0; i < filled_threads; i++) {
	        thr[i].join();
		}

		update_counter(spider, seenDomains, url_priority, seedUrl);

    }


   return 0;
}