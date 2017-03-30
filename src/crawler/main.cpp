#include <iostream>
#include <map>
#include <vector>
#include <math.h>
#include <unordered_map>
#include <CkSpider.h>
#include <CkStringArray.h>







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
    // spider.AddAvoidOutboundLinkPattern("*.comcast.*");
    // spider.AddAvoidOutboundLinkPattern("*.aol.*");
    // spider.AddAvoidOutboundLinkPattern("*~*");

    // spider.put_CacheDir("/home/dan/spiderCache/");
    // spider.put_FetchFromCache(true);
    // spider.put_UpdateCache(true);



    std::string max_url = "terra.com.br";
    url_priority[max_url]++;
    seedUrl[max_url] = "https://www.terra.com.br/";

    while (!url_priority.empty()) {

        std::string url = seedUrl[max_url];
    	url_priority.erase(max_url);
		seedUrl.erase(max_url);
        std::cout << "Crawling " << url << "\r\n";

        spider.Initialize(url.c_str());
        const char *domain = spider.getUrlDomain(url.c_str());
        seenDomains.Append(spider.getBaseDomain(domain));


    	// const char *robotsText = 0;
	    // robotsText = spider.fetchRobotsText();
	    // std::cout << robotsText << "\r\n";


        // CRAWL IN LINKS
        int i;
        bool success;
        for (i = 0; pow(2, i) <= spider.get_NumOutboundLinks() + 1; i++) {
            success = spider.CrawlNext();
            if (success == true) {
                if (spider.get_LastFromCache() != true) {
                    spider.SleepMs(2000);
                }
		        std::cout << i << " -> " << pow(2, i) << " <= " << spider.get_NumOutboundLinks()+1 << "\r\n";
            }
            else {
            	std::cout << "CRAWL ERROR" << "\r\n";
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

    }


   return 0;
}