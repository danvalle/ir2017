#include <iostream>
#include <map>
#include <vector>
#include <math.h>
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
            if(queue.begin()->second.empty())
                queue.erase(queue.begin());
            return temp;
        }
        std::cout << "ERROR: QUEUE EMPTY!" << std::endl;
        return "";
    }
};






int main() {
	CkSpider spider;

    std::unordered_map<std::string, int> url_priority;
    CkStringArray seenDomains;
    CkStringArray seedUrls;

    seenDomains.put_Unique(true);
    seedUrls.put_Unique(true);

    seedUrls.Append("http://www.uol.com.br");

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



    while (seedUrls.get_Count() > 0) {

        const char *url = seedUrls.pop();
        spider.Initialize(url);
        std::cout << url << "\r\n";

    	// const char *robotsText = 0;
	    // robotsText = spider.fetchRobotsText();
	    // std::cout << robotsText << "\r\n";

        const char *domain = spider.getUrlDomain(url);
        seenDomains.Append(spider.getBaseDomain(domain));

        int i;
        bool success;
        for (i = 0; pow(2, i) <= spider.get_NumOutboundLinks() + 1; i++) {
            success = spider.CrawlNext();
            if (success == true) {
                if (spider.get_LastFromCache() != true) {
                    spider.SleepMs(2000);
                }
		        std::cout << i << "," << pow(2, i) << " - " << spider.get_NumOutboundLinks() << "\r\n";
            }
            else {
                break;
            }
        }

        for (i = 0; i < spider.get_NumOutboundLinks(); i++) {
            url = spider.getOutboundLink(i);
            const char *domain = spider.getUrlDomain(url);
            const char *baseDomain = spider.getBaseDomain(domain);
        	
            if (seenDomains.Contains(baseDomain) == false) {
	        	url_priority[baseDomain]++;

                // if (seedUrls.get_Count() < 1000) {
                //     seedUrls.Append(url);
                // }
            }
        }

        std::string max_url;
        int max_url_counter = 0;
        std::unordered_map<std::string, int>::iterator it;
		for (it = url_priority.begin(); it != url_priority.end(); it++) {
			if (it->second > max_url_counter) {
				max_url = it->first;
				max_url_counter = it->second;
			}  
			std::cout << it->first << " - "<<it->second << "\r\n";
		}

		const char *c_url = max_url.c_str();
		seedUrls.Append(c_url);
		std::cout << "MAX:" << c_url << "\r\n";

    }



   return 0;
}