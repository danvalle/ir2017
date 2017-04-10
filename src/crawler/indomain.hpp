#ifndef indomain_hpp
#define indomain_hpp

///////////////// Functions /////////////////////
void write_html(CkSpider &spider, std::string *htmls, int thread_num);

int get_valid_domain(int domain_n, int num_domains, std::vector<int> inside_crawl);

void collect_outbound_links(CkSpider &spider, std::vector<std::string> &new_outbound_links);

void crawl_domains(std::vector<std::string> thread_links,
                   std::vector<std::string> *new_outbound_links,        
                   std::string *htmls,
                   int *total_crawled,
                   int thread_num);
////////////////////////////////////////////////
#endif