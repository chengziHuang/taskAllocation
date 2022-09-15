#ifndef tabu_list_h
#define tabu_list_h

#include <string>
#include <deque>
using namespace std;

class tabu_item {
public:
	string from_name;
	string end_name;
	bool from_cluster; // false means the low cluster, true means the high cluster
	bool end_cluster;
	string processor_name;
	double value_ob;
	tabu_item(string from_name, string end_name, bool from_cluster, bool end_cluster, string processor_name, double value_ob) {
		from_name = this->from_name;
		end_name = this->end_name;
		from_cluster = this->from_cluster;
		end_cluster = this->end_cluster;
	}

	bool compare(tabu_item item) {
		return ((this->from_name == item.from_name) && (this->end_name == item.end_name) && (this->from_cluster == item.end_cluster) && (this->processor_name == item.processor_name) && (this->value_ob == item.value_ob));
	}
};

class tabu_list {
public:
	int max_size;
	deque<tabu_item> list;

	tabu_list(int max_size) {
		this->max_size = max_size;
	}

	bool find(tabu_item item) {
		for (int i = 0; i < list.size(); i++) {
			if (list.at(i).compare(item)) {
				return true;
			}
		}
		return false;
	}

	void add(tabu_item item) {
		list.push_back(item);
		if (list.size() > max_size) {
			list.pop_front();
		}
	}
};

#endif