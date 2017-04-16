#ifndef outbound_hpp
#define outbound_hpp


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


///////////////// Functions /////////////////////
/*
 * Function:  initialize_thread_strings
 * --------------------
 *  Initialize and reserve memory for strings that will collect the html
 *  
 *  num_threads: chosen number of threads
 *  thread_htmls: strings that will collect the html
 *
 */
void initialize_thread_strings(int num_threads, 
							   std::vector<std::string> &thread_htmls);

/*
 * Function:  fill_thread_links
 * --------------------
 *  Gets best urls from table to send to thread and update seen domains list
 *  
 *  thread_links: vector to be filled with urls
 *  url_prioriy: table of domains and counter  
 *  seedUrl: table of domains and their respective url
 *  spider: collector object 
 *  seenDomains: domains already seen
 *
 */
void fill_thread_links(std::vector<std::string> *thread_links,
					  std::unordered_map<std::string, int> &url_priority,
					  std::unordered_map<std::string, std::string> &seedUrl,
					  CkSpider *spider,
					  CkStringArray *seenDomains);

/*
 * Function:  update_counter
 * --------------------
 *  Update table of next domains with links collected 
 *  
 *  spider: collector object 
 *  url_prioriy: table of domains and counter  
 *  seedUrl: table of domains and their respective url
 *  new_outbound_links: collected outbound urls
 *
 */
void update_counter(CkSpider &spider,
					std::unordered_map<std::string, int> &url_priority,
					std::unordered_map<std::string, std::string> &seedUrl,
                    std::vector<std::string> new_outbound_links);

////////////////////////////////////////////////
#endif