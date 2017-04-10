#ifndef outbound_hpp
#define outbound_hpp

///////////////// Functions /////////////////////
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

void initialize_thread_strings(int num_threads, std::vector<std::string> &thread_htmls);

void update_counter(CkSpider &spider,
					CkStringArray &seenDomains,
					std::unordered_map<std::string, int> &url_priority,
					std::unordered_map<std::string, std::string> &seedUrl,
                    std::vector<std::string> new_outbound_links);

void fill_thread_links(std::vector<std::string> *thread_links,
					  std::unordered_map<std::string, int> &url_priority,
					  std::unordered_map<std::string, std::string> &seedUrl,
					  CkSpider *spider,
					  CkStringArray *seenDomains);

////////////////////////////////////////////////
#endif