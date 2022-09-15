#ifndef alocate_light_task_h
#define alocate_light_task_h

#define PRINT_F
//#define PRINT

#include "task_processor.h"
#include "generate_random.h"
#include <algorithm>
#include <vector>
#include <random>       
#include <chrono>
#include <tuple>
#include <cmath>

using namespace std;


bool decreasing_criticality(task* t1, task* t2);
bool increasing_order(uniprocessor* p1, uniprocessor* p2);
double quality_of_partition(vector<uniprocessor*> p_success);
tuple<vector<uniprocessor*>, double> partitioned_framework(vector<processor> processorset, vector<task*> taskset);
double temperature_function(double T);
vector<uniprocessor*> allocate_light_task(vector<processor> processorset, vector<task*> taskset);



bool decreasing_criticality(task* t1, task* t2) {
	/*
	* The tasks are ordered according to decreasing criticality level.
	* In the case of equal criticality levels, decreasing utilisation isused.
	*/
	if (t1->criticality == "HI" && t2->criticality == "LOW") {
		return true;
	}
	else {
		if (t1->criticality == "LOW" && t2->criticality == "HI") {
			return false;
		}
		else {
			double u1 = t1->C_N / t1->deadline;
			double u2 = t2->C_N / t2->deadline;
			return u1 > u2;
		}
	}
}

bool increasing_order(uniprocessor* p1, uniprocessor* p2) {
	return p1->speed < p2->speed;
}

double quality_of_partition(vector<uniprocessor*> p_success) {
	if (p_success.size() == 0) {
		return 0.0;
	}
	double tmp = 0.0;
	int number_task = 0;
	for (int i = 0; i < p_success.size(); i++) {
		tmp = tmp + ((max((p_success[i]->U_O_O + p_success[i]->U_N_O), (p_success[i]->U_O_N + p_success[i]->U_N_N))) / p_success[i]->get_bound());
		number_task += (int)p_success[i]->host.size();
	}
	return tmp / p_success.size() * number_task;
}

double temperature_function(double T) {
	return 0.98 * T;
}

tuple<vector<uniprocessor*>, double> partitioned_framework(vector<processor> processorset, vector<task*> taskset) {
	double quality = 0.0;

	vector<uniprocessor*> p_success;
	vector<uniprocessor*> tmp_processor;
	p_success.reserve(processorset.size());

	if (processorset.size() == 0) {
		return make_tuple(p_success, 0.0);
	}

	sort(taskset.begin(), taskset.end(), decreasing_criticality);
	
	vector<uniprocessor*> attempt;
	attempt.reserve(processorset.size());
	for (int p = 0; p < processorset.size(); p++) {
		attempt.push_back(new uniprocessor(processorset[p]));
	}
	sort(attempt.begin(), attempt.end(), increasing_order);

	for (int t = 0; t < taskset.size(); t++) {
		if (taskset[t]->C_O > taskset[t]->deadline || taskset[t]->C_N > taskset[t]->deadline) {
			continue;
		}
		for (int p = 0; p < processorset.size(); p++) {
			bool test = attempt[p]->test_and_add(taskset[t]);
			if (test == true) {
				break;
			}
		}
	}

	tmp_processor.clear();
	for (int p = 0; p < processorset.size(); p++) {
		if (attempt[p]->host.size() != 0) {
			tmp_processor.push_back(attempt[p]);
		}
	}

	double qop = quality_of_partition(tmp_processor);
	if (qop > quality) {
			quality = qop;
			p_success = tmp_processor;
	}

	return make_tuple(p_success, quality);
}


vector<uniprocessor*> allocate_light_task(vector<processor> processorset, vector<task*> taskset) {
	vector<processor> use;
	vector<processor> unused(processorset);
	vector<uniprocessor*> p_success;
	double quality;
	int test_number =  processorset.size() * 100;
	
	double T = 100.0;

	// initial solution
	tie(p_success, quality) = partitioned_framework(use, taskset);
	double best_quality = quality;



	int count = 0;
	while (count < test_number) {
		int rule = generate_random_number(0, 1);
		if (rule == 1) {
			// add processor
			if (unused.size() == 0) {
				continue;
			}
			vector<processor>::iterator index = select_randomly(unused.begin(), unused.end());
			vector<processor> neighbor(use);
			neighbor.push_back(*index);

			vector<uniprocessor*> p_local;
			double quality_local;
			tie(p_local, quality_local) = partitioned_framework(neighbor, taskset);
			T = temperature_function(T);
			if ((exp(quality_local - quality) / T) >= generate_random_01()) {

				if (quality_local > best_quality) {
					p_success = p_local;
					best_quality = quality_local;
				}
				quality = quality_local;

				use = neighbor;
				unused.erase(index);
			}
			count++;
		}
		else {
			// delete processor
			if (use.size() == 0) {
				continue;
			}


			vector<processor> neighbor(use);
			vector<processor>::iterator index = select_randomly(neighbor.begin(), neighbor.end());
			processor tmp = *index;
			neighbor.erase(index);

			vector<uniprocessor*> p_local;
			double quality_local;
			tie(p_local, quality_local) = partitioned_framework(neighbor, taskset);
			T = temperature_function(T);
			if ((exp(quality_local - quality) / T) >= generate_random_01()) {
				if (quality_local > best_quality) {
					p_success = p_local;
					best_quality = quality_local;
				}
				quality = quality_local;

				use = neighbor;
				unused.push_back(tmp);
			}
			count++;
		}
	}



	return p_success;
}

#endif // partitioned_scheduling_h
