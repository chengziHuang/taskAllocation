#include "task_allocation.h"
#include <chrono>
using namespace std;

// To see the allocation result, uncomment this marco
//#define PRINT 


int main(int argc, char* argv[]) {
	
	string path;
	if (argc < 2) {
		return 0;
	}
	else {
		path = argv[1];
	}


	vector<processor> processorset;
	vector<task*> taskset;
	// read
	tie(processorset, taskset) = read(path);


	// allocate light task
	vector<uniprocessor*> result1 = allocate_light_task(processorset, taskset);

	// get remain
	vector<heavy_task*> heavyset;
	tie(processorset, heavyset) = get_remain(processorset, taskset, result1);

	if (heavyset.size() == 0) {
#ifdef PRINT
		for (int i = 0; i < result1.size(); i++) {
			result1[i]->print();
		}
#endif 
		cout << "success" << endl;
		return 0;
	}

	if (heavyset.size() > 0 && processorset.size() == 0) {
		cout << "failure" << endl;
		return 0;
	}
	auto start = chrono::steady_clock::now();
	
	if (allocate_heavy_task(processorset, heavyset) == true) {
#ifdef PRINT
		for (int i = 0; i < result1.size(); i++) {
			result1[i]->print();
		}
#endif 
		cout << "success" << endl;
	}
	else {
		cout << "failure" << endl;
	}
	return 0;
}
