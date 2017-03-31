#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <algorithm>
#include <unordered_map>
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



    std::string max_url = "terra.com.br";
    url_priority[max_url]++;
    seedUrl[max_url] = "https://www.terra.com.br/";

    int crawled_pages = 0;
    while (!url_priority.empty()) {

        std::string url = seedUrl[max_url];
    	url_priority.erase(max_url);
		seedUrl.erase(max_url);
		crawled_pages++;
        std::cout << "==================\r\nCrawling " << url << " ( " << crawled_pages << " )\r\n";

        spider.Initialize(url.c_str());
        const char *domain = spider.getUrlDomain(url.c_str());
        seenDomains.Append(spider.getBaseDomain(domain));


    	// const char *robotsText = 0;
	    // robotsText = spider.fetchRobotsText();
	    // std::cout << robotsText << "\r\n";




        // CRAWL IN LINKS
        int i;
        bool success;
        priority_queue inlink_queue;
        std::vector<std::string> used;
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
					std::cout << "Inside crawl- " << smallest_url << "\r\n";
				}





                if (spider.get_LastFromCache() != true) {
                    spider.SleepMs(20000);
                }
            }
            else {
                break;
            }
        }







        // UPDATE OUTBOUND COUNTER
        std::cout << "\r\nOUTBOUND:\r\n";
        int n_outlinks = spider.get_NumOutboundLinks();
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
        spider.ClearOutboundLinks();



        // GET NEXT URL
        int max_url_counter = 0;
        std::unordered_map<std::string, int>::iterator it;
		for (it = url_priority.begin(); it != url_priority.end(); it++) {
			if (it->second > max_url_counter) {
				max_url = it->first;
				max_url_counter = it->second;
			}  
		}


        std::cout << "\r\nCOUNTER:\r\n";
		for (it = url_priority.begin(); it != url_priority.end(); it++) {
			std::cout << it->first << ", "<<it->second << "\r\n";
		}
        std::cout << "+++++++++++++++++\r\nLEN: " << url_priority.size();
		std::cout << "\r\nMAX: " << max_url << "\r\n\r\n";


		if (crawled_pages >= 1000000) {
			break;
		} 
    }


   return 0;
}