#ifndef utils_hpp
#define utils_hpp

///////////////// Functions /////////////////////
/*
 * Function:  get_time
 * --------------------
 *  Gets current timestamp
 *  
 *  returns: current timestamp
 */
long get_time();

/*
 * Function:  url_score
 * --------------------
 *  Computes the score for a url
 *  
 *  url: Url to be computed
 *
 *  returns: value that represents the score of the url
 */
int url_score(std::string url);

/*
 * Function:  checkpoint
 * --------------------
 *  Saves the current state of the Crawler so it can be restarted
 *  	in the same point 
 *  
 *  seenDomains: Domains that have been used before
 *	url_priority: Table with the domains that will be crawled and
 *					their priority
 *	seedUrl: Url related to every domain in url_priority
 *  total_crawled: Amount of pages seen
 *
 */
void checkpoint(CkStringArray &seenDomains,
				std::unordered_map<std::string, int> &url_priority,
				std::unordered_map<std::string, std::string> &seedUrl,
				int total_crawled);

/*
 * Function:  get_checkpoint
 * --------------------
 *  Load the state of the last Crawler saved
 *  
 *  seenDomains: Domains that have been used before
 *	url_priority: Table with the domains that will be crawled and
 *					their priority
 *	seedUrl: Url related to every domain in url_priority
 *  total_crawled: Amount of pages seen
 *
 */
void get_checkpoint(CkStringArray &seenDomains,
					std::unordered_map<std::string, int> &url_priority,
					std::unordered_map<std::string, std::string> &seedUrl,
					int *total_crawled);
////////////////////////////////////////////////
#endif