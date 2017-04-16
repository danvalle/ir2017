#ifndef indomain_hpp
#define indomain_hpp

///////////////// Functions /////////////////////

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
void write_html(CkSpider &spider, std::string *htmls, int thread_num);

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
					 std::vector<int> inside_crawl);

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
							std::vector<std::string> &new_outbound_links);

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
                   int thread_num);
////////////////////////////////////////////////
#endif