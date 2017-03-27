#include <iostream>
#include <vector>
#include <CkSpider.h>
#include <CkStringArray.h>

using namespace std;


int main() {
	CkSpider spider;

    CkStringArray seenDomains;
    CkStringArray seedUrls;

    seenDomains.put_Unique(true);
    seedUrls.put_Unique(true);

    seedUrls.Append("https://www.uol.com.br/");

    spider.AddAvoidOutboundLinkPattern("*?id=*");
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

    	const char *robotsText = 0;
	    robotsText = spider.fetchRobotsText();
	    // std::cout << robotsText << "\r\n";

        const char *domain = spider.getUrlDomain(url);
        seenDomains.Append(spider.getBaseDomain(domain));

        int i;
        bool success;
        for (i = 0; i < 5; i++) {
            success = spider.CrawlNext();
            if (success == true) {
                std::cout << spider.lastUrl() << "\r\n";

                if (spider.get_LastFromCache() != true) {
                    spider.SleepMs(1000);
                }
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
                if (seedUrls.get_Count() < 1000) {
                    seedUrls.Append(url);
                }

            }

        }

    }



   return 0;
}