#ifndef allocate_heavy_task_h
#define allocate_heavy_task_h

#include "generate_random.h"
#include "task_processor.h"
#include <vector>
#include <chrono>
#include <random>
#include <cmath>
#include <algorithm>
#include <memory>
#include "tabu_list.h"

using namespace std;

//#define PRINT

bool dresing_processor_speed(processor p1, processor p2);
bool allocate_heavy_task(vector<processor> processorset, vector<heavy_task*> taskset);
bool decresing_heavy_utilisation(heavy_task* t1, heavy_task* t2);
double get_T0(vector<double> response_time_result, vector<heavy_task*> taskset);
bool local_search(vector<heavy_task*> taskset, vector<int> index_HI, vector<int> index_LO, vector<double>& response_time_result, int count_max, double T0);
double cost_function(double r, double deadline);
double temperature(double T);
bool check(vector<double> response_time_result, vector<heavy_task*> taskset);

double cost_function(double r, double deadline) {
	double result = r - deadline;
	if (result < 0) {
		return 0.0;
	}
	else {
		return result;
	}
}


bool check(vector<double> response_time_result, vector<heavy_task*> taskset) {
	bool result = true;
	for (int i = 0; i < taskset.size(); i++) {
		if (response_time_result[i] > taskset[i]->deadline) {
			result = false;
			break;
		}
	}
	return result;
}

double temperature(double T) {
	return 0.98 * T;
}

bool local_search(vector<heavy_task*> taskset, vector<int> index_HI, vector<int> index_LO, vector<double>& response_time_result, int count_max, double T0) {
	int count = 0;
	double T = T0;

	tabu_list tl(taskset.size());
	while (count < count_max) {
		int rule = generate_random_number(1, 10);
		if (rule == 1) {
			// HI_LO to HI_LO
			if (index_HI.size() <= 1) {
				continue;
			}

			int i = generate_random_number(0, index_HI.size() - 1);
			int j = generate_random_number(0, index_HI.size() - 1);

			if (i == j || taskset[index_HI[i]]->cluster_info[0].size() == 0) {
				continue;
			}

			int index_i = index_HI[i];
			int index_j = index_HI[j];

			

			processor target = *select_randomly(taskset[index_i]->cluster_info[0].processor_list.begin(), taskset[index_i]->cluster_info[0].processor_list.end());

			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = new heavy_task(taskset[index_j]);

			new_task_i->cluster_info[0].del(target);
			new_task_j->cluster_info[0].add(target);

			double r_i = response_time_result[index_i];
			double r_j = response_time_result[index_j];

			double new_r_i = new_task_i->response_time();
			double new_r_j = new_task_j->response_time();

			double delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
				- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);
		
			
			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				response_time_result[index_j] = new_r_j;
				taskset[index_i] = new_task_i;
				taskset[index_j] = new_task_j;
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}


		if (rule == 2) {
			// HI_HI TO HI_HI
			if (index_HI.size() <= 1) {
				continue;
			}
			
			int i = generate_random_number(0, index_HI.size() - 1);
			int j = generate_random_number(0, index_HI.size() - 1);
			if (i == j || taskset[index_HI[i]]->cluster_info[1].size() == 0) {
				continue;
			}

			int index_i = index_HI[i];
			int index_j = index_HI[j];

			processor target = *select_randomly(taskset[index_i]->cluster_info[1].processor_list.begin(), taskset[index_i]->cluster_info[1].processor_list.end());

			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = new heavy_task(taskset[index_j]);

			new_task_i->cluster_info[1].del(target);
			new_task_j->cluster_info[1].add(target);

			double r_i = response_time_result[index_i];
			double r_j = response_time_result[index_j];

			double new_r_i = new_task_i->response_time();
			double new_r_j = new_task_j->response_time();
			
			double delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
				- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);
			
			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				response_time_result[index_j] = new_r_j;
				taskset[index_i] = new_task_i;
				taskset[index_j] = new_task_j;
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}


		if (rule == 3) {
			// LO_HI to LO_HI
			if (index_LO.size() <= 1) {
				continue;
			}

			int i = generate_random_number(0, index_LO.size() - 1);
			int j = generate_random_number(0, index_LO.size() - 1);

			if (i == j || taskset[index_LO[i]]->cluster_info[0].size() == 0) {
				continue;
			}

			int index_i = index_LO[i];
			int index_j = index_LO[j];

			processor target = *select_randomly(taskset[index_i]->cluster_info[0].processor_list.begin(), taskset[index_i]->cluster_info[0].processor_list.end());

			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = new heavy_task(taskset[index_j]);

			new_task_i->cluster_info[0].del(target);
			new_task_j->cluster_info[0].add(target);

			double r_i = response_time_result[index_i];
			double r_j = response_time_result[index_j];

			double new_r_i = new_task_i->response_time();
			double new_r_j = new_task_j->response_time();

			double delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
				- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);
		
			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				response_time_result[index_j] = new_r_j;
				taskset[index_i] = new_task_i;
				taskset[index_j] = new_task_j;
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}

		if (rule == 4) {
			// LO_LO to LO_LO
			if (index_LO.size() <= 1) {
				continue;
			}

			int i = generate_random_number(0, index_LO.size() - 1);
			int j = generate_random_number(0, index_LO.size() - 1);

			if (i == j || taskset[index_LO[i]]->cluster_info[1].size() == 0) {
				continue;
			}

			int index_i = index_LO[i];
			int index_j = index_LO[j];

			processor target = *select_randomly(taskset[index_i]->cluster_info[1].processor_list.begin(), taskset[index_i]->cluster_info[1].processor_list.end());

			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = new heavy_task(taskset[index_j]);

			new_task_i->cluster_info[1].del(target);
			new_task_j->cluster_info[1].add(target);

			double r_i = response_time_result[index_i];
			double r_j = response_time_result[index_j];

			double new_r_i = new_task_i->response_time();
			double new_r_j = new_task_j->response_time();

			double delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
				- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);

			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				response_time_result[index_j] = new_r_j;
				taskset[index_i] = new_task_i;
				taskset[index_j] = new_task_j;
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}

		if (rule == 5) {
			// HI_LO to LO_HI
			if (index_LO.size() == 0 || index_HI.size() == 0) {
				continue;
			}

			int i = generate_random_number(0, index_HI.size() - 1);
			int j = generate_random_number(0, index_LO.size() - 1);

			if (taskset[index_HI[i]]->cluster_info[0].size() == 0) {
				continue;
			}
			
			int index_i = index_HI[i];
			int index_j = index_LO[j];

			processor target = *select_randomly(taskset[index_i]->cluster_info[0].processor_list.begin(), taskset[index_i]->cluster_info[0].processor_list.end());

			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = new heavy_task(taskset[index_j]);

			new_task_i->cluster_info[0].del(target);
			new_task_j->cluster_info[0].add(target);

			double r_i = response_time_result[index_i];
			double r_j = response_time_result[index_j];

			double new_r_i = new_task_i->response_time();
			double new_r_j = new_task_j->response_time();

			double delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
				- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);

			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				response_time_result[index_j] = new_r_j;
				taskset[index_i] = new_task_i;
				taskset[index_j] = new_task_j;
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}

		if (rule == 6) {
			// LO_HI to HI_LO
			if (index_LO.size() == 0 || index_HI.size() == 0) {
				continue;
			}

			int i = generate_random_number(0, index_LO.size() - 1);
			int j = generate_random_number(0, index_HI.size() - 1);

			if (taskset[index_LO[i]]->cluster_info[0].size() == 0) {
				continue;
			}

			int index_i = index_LO[i];
			int index_j = index_HI[j];

			processor target = *select_randomly(taskset[index_i]->cluster_info[0].processor_list.begin(), taskset[index_i]->cluster_info[0].processor_list.end());

			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = new heavy_task(taskset[index_j]);

			new_task_i->cluster_info[0].del(target);
			new_task_j->cluster_info[0].add(target);

			double r_i = response_time_result[index_i];
			double r_j = response_time_result[index_j];

			double new_r_i = new_task_i->response_time();
			double new_r_j = new_task_j->response_time();

			double delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
				- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);

			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				response_time_result[index_j] = new_r_j;
				taskset[index_i] = new_task_i;
				taskset[index_j] = new_task_j;
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}

		if (rule == 7) {
			// HI_LO to HI_HI
			if (index_HI.size() == 0) {
				continue;
			}

			int i = generate_random_number(0, index_HI.size() - 1);
			if (taskset[index_HI[i]]->cluster_info[0].size() == 0) {
				continue;
			}
			int index_i = index_HI[i];
			int index_j;
			processor target = *select_randomly(taskset[index_i]->cluster_info[0].processor_list.begin(), taskset[index_i]->cluster_info[0].processor_list.end());
			
			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = nullptr;

			new_task_i->cluster_info[0].del(target);
			new_task_i->cluster_info[1].add(target);

			double delta;
			double new_r_i, new_r_j;
			if (index_LO.size() != 0) {
				// select a random LO task, and add this processor to LO_LO
				int j = generate_random_number(0, index_LO.size() - 1);
				index_j = index_LO[j];

				new_task_j = new heavy_task(taskset[index_j]);
				new_task_j->cluster_info[1].add(target);

				double r_i = response_time_result[index_i];
				double r_j = response_time_result[index_j];

				new_r_i = new_task_i->response_time();
				new_r_j = new_task_j->response_time();

				delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
					- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);
			}
			else {
				double r_i = response_time_result[index_i];
				new_r_i = new_task_i->response_time();

				delta = cost_function(r_i, new_task_i->deadline) - cost_function(new_r_i, new_task_i->deadline);
			}

			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				taskset[index_i] = new_task_i;
				if (index_LO.size() != 0) {
					response_time_result[index_j] = new_r_j;
					taskset[index_j] = new_task_j;
				}
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}

		if (rule == 8) {
			// LO_HI to LO_LO
			if (index_LO.size() == 0) {
				continue;
			}

			int i = generate_random_number(0, index_LO.size() - 1);
			if (taskset[index_LO[i]]->cluster_info[0].size() == 0) {
				continue;
			}

			int index_i = index_LO[i];
			int index_j;
			processor target = *select_randomly(taskset[index_i]->cluster_info[0].processor_list.begin(), taskset[index_i]->cluster_info[0].processor_list.end());
		
			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = nullptr;

			new_task_i->cluster_info[0].del(target);
			new_task_i->cluster_info[1].add(target);

			double delta;
			double new_r_i, new_r_j;
			if (index_HI.size() != 0) {
				// select a random HI task, and add this processor to HI_HI
				int j = generate_random_number(0, index_HI.size() - 1);
				index_j = index_HI[j];

				new_task_j = new heavy_task(taskset[index_j]);
				new_task_j->cluster_info[1].add(target);

				double r_i = response_time_result[index_i];
				double r_j = response_time_result[index_j];

				new_r_i = new_task_i->response_time();
				new_r_j = new_task_j->response_time();

				delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
					- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);
			}
			else {
				double r_i = response_time_result[index_i];
				new_r_i = new_task_i->response_time();

				delta = cost_function(r_i, new_task_i->deadline) - cost_function(new_r_i, new_task_i->deadline);
			}

			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				taskset[index_i] = new_task_i;
				if (index_HI.size() != 0) {
					response_time_result[index_j] = new_r_j;
					taskset[index_j] = new_task_j;
				}
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}

		if (rule == 9) {
			// HI_HI to HI_LO
			if (index_HI.size() == 0) {
				continue;
			}

			int i = generate_random_number(0, index_HI.size() - 1);
			if (taskset[index_HI[i]]->cluster_info[1].size() == 0) {
				continue;
			}

			int index_i = index_HI[i];
			int index_j;
			
			processor target = *select_randomly(taskset[index_i]->cluster_info[1].processor_list.begin(), taskset[index_i]->cluster_info[1].processor_list.end());

			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = nullptr;

			new_task_i->cluster_info[1].del(target);
			new_task_i->cluster_info[0].add(target);

			double delta;
			double new_r_i, new_r_j;

			if (index_LO.size() != 0) {
				// find the LO_LO, which contains this processor
				index_j = -1;
				for (int i = 0; i < index_LO.size(); i++) {
					if (taskset[index_LO[i]]->cluster_info[1].find(target) == true) {
						index_j = index_LO[i];
						break;
					}
				}

				new_task_j = new heavy_task(taskset[index_j]);
				new_task_j->cluster_info[1].del(target);

				double r_i = response_time_result[index_i];
				double r_j = response_time_result[index_j];

				new_r_i = new_task_i->response_time();
				new_r_j = new_task_j->response_time();

				delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
					- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);
			}
			else {
				double r_i = response_time_result[index_i];
				new_r_i = new_task_i->response_time();

				delta = cost_function(r_i, new_task_i->deadline) - cost_function(new_r_i, new_task_i->deadline);
			}

			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				taskset[index_i] = new_task_i;
				if (index_LO.size() != 0) {
					response_time_result[index_j] = new_r_j;
					taskset[index_j] = new_task_j;
				}
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}

		if (rule == 10) {
			// LO_LO to LO_HI
			if (index_LO.size() == 0) {
				continue;
			}

			int i = generate_random_number(0, index_LO.size() - 1);
			if (taskset[index_LO[i]]->cluster_info[1].size() == 0) {
				continue;
			}

			int index_i = index_LO[i];
			int index_j;

			processor target = *select_randomly(taskset[index_i]->cluster_info[1].processor_list.begin(), taskset[index_i]->cluster_info[1].processor_list.end());

			heavy_task* new_task_i = new heavy_task(taskset[index_i]);
			heavy_task* new_task_j = nullptr;

			new_task_i->cluster_info[1].del(target);
			new_task_i->cluster_info[0].add(target);

			double delta;
			double new_r_i, new_r_j;

			if (index_HI.size() != 0) {
				// find the HI_HI, which contains this processor
				index_j = -1;
				for (int i = 0; i < index_HI.size(); i++) {
					if (taskset[index_HI[i]]->cluster_info[1].find(target) == true) {
						index_j = index_HI[i];
						break;
					}
				}

				new_task_j = new heavy_task(taskset[index_j]);
				new_task_j->cluster_info[1].del(target);

				double r_i = response_time_result[index_i];
				double r_j = response_time_result[index_j];

				new_r_i = new_task_i->response_time();
				new_r_j = new_task_j->response_time();

				delta = cost_function(r_i, new_task_i->deadline) + cost_function(r_j, new_task_j->deadline)
					- cost_function(new_r_i, new_task_i->deadline) - cost_function(new_r_j, new_task_j->deadline);
			}
			else {
				double r_i = response_time_result[index_i];
				new_r_i = new_task_i->response_time();

				delta = cost_function(r_i, new_task_i->deadline) - cost_function(new_r_i, new_task_i->deadline);
			}

			double x = delta / T;
			if (exp(x) > generate_random_01()) {
				response_time_result[index_i] = new_r_i;
				taskset[index_i] = new_task_i;
				if (index_HI.size() != 0) {
					response_time_result[index_j] = new_r_j;
					taskset[index_j] = new_task_j;
				}
			}

			if (check(response_time_result, taskset)) {
#ifdef PRINT
				for (int i = 0; i < taskset.size(); i++) {
					taskset[i]->print();
				}
#endif
				//cout << count << endl;
				return true;
			}
			T = temperature(T);
			count++;
			continue;
		}
	}
	return false;
}



double get_T0(vector<double> response_time_result, vector<heavy_task*> taskset) {
	double result = 0.0;
	for (int i = 0; i < taskset.size(); i++) {
		
		result += cost_function(response_time_result[i], taskset[i]->deadline);
	}
	return result;
}


bool decresing_heavy_utilisation(heavy_task* t1, heavy_task* t2) {
	/*
	* The tasks are ordered by decreasing utilisation
	*/
	double u1 = t1->C_N / t1->deadline;
	double u2 = t2->C_N / t2->deadline;
	return u1 > u2;
}

bool dresing_processor_speed(processor p1, processor p2) {
	return p1.speed > p2.speed;
}

bool allocate_heavy_task(vector<processor> processorset, vector<heavy_task*> taskset) {
	// initial solution
	sort(processorset.begin(), processorset.end(), dresing_processor_speed);
	sort(taskset.begin(), taskset.end(), decresing_heavy_utilisation);

	vector<int> index_HI;
	vector<int> index_LO;
	for (int i = 0; i < taskset.size(); i++) {
		if (taskset[i]->criticality == "HI") {
			index_HI.push_back(i);
		}
		else {
			index_LO.push_back(i);
		}
	}
	int j = 0;
	for (int i = 0; i < taskset.size(); i++) {
		while (j < processorset.size()) {


			if (taskset[i]->criticality == "LOW") {
				
				taskset[i]->cluster_info[1].add(processorset[j]);

				if (index_HI.size() != 0) {
					int random_index = index_HI[generate_random_number(0, index_HI.size() - 1)];
					taskset[random_index]->cluster_info[1].add(processorset[j]);
				}
			}
			else {
				taskset[i]->cluster_info[0].add(processorset[j]);
			}

			j++;
			if (taskset[i]->response_time_low() <= taskset[i]->deadline) {
				break;
			}
		}
	}


	if (j < processorset.size()) {
		return true; // all tasks have been already schedulable now!
	}

	vector<double> response_time_result;
	response_time_result.reserve(taskset.size());
	for (int i = 0; i < taskset.size(); i++) {
		response_time_result.push_back(taskset[i]->response_time());
	}
	
	double T0 = get_T0(response_time_result, taskset);
	if (T0 == 0.0) {
		return true;
	}

	// local search
	int max_count = (int)(processorset.size() *  processorset.size() * 100);
	
	bool result = local_search(taskset, index_HI, index_LO, response_time_result, max_count, T0);

	return result;
}

#endif 
