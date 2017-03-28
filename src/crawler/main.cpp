#include <iostream>
#include <map>
#include <vector>
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

    priority_queue heap;
    CkStringArray seenDomains;
    CkStringArray seedUrls;

    seenDomains.put_Unique(true);
    seedUrls.put_Unique(true);

    seedUrls.Append("http://tvuol.uol.com.br/canal/jogos/");

    spider.AddAvoidOutboundLinkPattern("*?id=*");
    spider.AddAvoidOutboundLinkPattern("*?rf=*");
    spider.AddAvoidOutboundLinkPattern("*?prd=*");
    spider.AddAvoidOutboundLinkPattern("*.mypages.*");
    spider.AddAvoidOutboundLinkPattern("*.personal.*");
    spider.AddAvoidOutboundLinkPattern("*.comcast.*");
    spider.AddAvoidOutboundLinkPattern("*.aol.*");
    spider.AddAvoidOutboundLinkPattern("*~*");

    // spider.put_CacheDir("/home/dan/spiderCache/");
    // spider.put_FetchFromCache(true);
    // spider.put_UpdateCache(true);

    while (seedUrls.get_Count() > 0) {

        const char *url = seedUrls.pop();
        spider.Initialize(url);

    	// const char *robotsText = 0;
	    // robotsText = spider.fetchRobotsText();
	    // std::cout << robotsText << "\r\n";

        const char *domain = spider.getUrlDomain(url);
        seenDomains.Append(spider.getBaseDomain(domain));

        // int i;
        // bool success;
        // for (i = 0; i < 5; i++) {
        //     if (i == 0) {
        //         std::cout << "Go out\n";
        //     }

        //     success = spider.CrawlNext();
        //     if (success == true) {
        //         std::cout << spider.lastUrl() << "\r\n";

        //         if (spider.get_LastFromCache() != true) {
        //             spider.SleepMs(1000);
        //         }
        //     }
        //     else {
        //         break;
        //     }
        // }

        // for (i = 0; i < spider.get_NumOutboundLinks(); i++) {

        //     url = spider.getOutboundLink(i);
        //     const char *domain = spider.getUrlDomain(url);
        //     const char *baseDomain = spider.getBaseDomain(domain);
        //     std::cout << domain << "\r\n";

        //     if (seenDomains.Contains(baseDomain) == false) {
        //         if (seedUrls.get_Count() < 1000) {
        //             seedUrls.Append(url);
        //         }

        //     }

        // }



        spider.CrawlNext();

        int n_unspidered = spider.get_NumUnspidered();
        int n_outbound = spider.get_NumOutboundLinks();
        std::cout << spider.lastUrl() << "\r\n";

        for (int i = 0; i < n_unspidered; i++) {
            url = spider.getUnspideredUrl(0);
            spider.SkipUnspidered(0);
            std::cout << url << "\r\n";
        }

        // for (int i = 0; i < n_unspidered; i++) {
        //     url = spider.getOutboundLink(i);
        //     std::cout << url << "\r\n";
        // }

    }



   return 0;
}