from drs import drs

from random import uniform
from random import random
from random import randint

import os

def mkdir(path):
    folder = os.path.exists(path)

    if not folder:
        os.makedirs(path)

def generateProcessor(n, Sm):
    
    upper_bound = [1 for x in range (0, n - 1)]
    lower_bound = [0 for x in range (0, n - 1)]
    processorSet = drs(n - 1, Sm - 1, upper_bounds=upper_bound, lower_bounds=lower_bound)
    processorSet.append(1) 

    return processorSet

def generateTaskSet(Sm, UB, p_hu, u_max, p_max, p_hc, r_max, e_max):
    # p_hu: probability of a task to be a high-utilization task 
    # u_max: the max ratio of overload utilization of a heavy task
    # p_max: the max ratio of overload critical-path length and period
    # p_hc: probability of a task to ba a HI-critical task
    # r_max: the max ratio of overload and nominal utilizations
    # e_max: the max ratio of T_max and T

    U_O = 0
    U_N = 0
    taskset = []
    bound1 = Sm * UB
    bound2 = Sm * UB - 0.1 * UB
    while True:
        D_i = randint(10, 1000)
        T_i = D_i
        # generate C_i^{O}
       
        C_iO = uniform(1.01, u_max) * T_i
        
        
        # generate L_i^{O}
        L_iO =  D_i * p_max
        l = random()
        if l > 0.8:
            L_iO = L_iO
        else:
            if l > 0.4:
                L_iO = 0.8 * L_iO
            else:
                L_iO = 0.4 * L_iO

        # generate Z_i and T_max
        p_ic = random()
        if p_ic <= p_hc:
            Z_i = 'HI'
            R_i = uniform(1, r_max)
            T_max_i = D_i
        else:
            Z_i = 'LOW'
            R_i = 1
            e_i = randint(2, e_max)
            T_max_i = D_i * e_i

        # generate C_i^{LO} and L_i^{LO}
        C_iN = C_iO / R_i
        L_iN = L_iO / R_i

        U_N = U_N + (C_iN / T_i)
        if Z_i == 'HI':
            U_O = U_O + (C_iO / T_i)
        else:
            U_O = U_O + (C_iO / T_max_i)
        if (U_O > bound1 or U_N > bound1):
            U_N = U_N - (C_iN / T_i)
            if Z_i == 'HI':
                U_O = U_O - (C_iO / T_i)
            else:
                U_O = U_O - (C_iO / T_max_i)    
            continue
        else:
            tup = (Z_i, D_i, C_iN, C_iO, L_iN, L_iO, T_max_i)
            taskset.append(tup)
            if (U_O > bound2 or U_N > bound2):
                break
    print(U_O / Sm, U_N / Sm)
    return taskset

def storeResults(processorSet, taskSet,  filePath):
    with open(filePath, 'w') as f:
        f.write(str(len(processorSet)) + '\t' + str(len(taskSet)))
        f.write('\n')
        
        for i in range(0, len(processorSet)):
            f.write('processor{}'.format(i) + '\t' + str(processorSet[i]) + '\t')
                    
        f.write('\n')

        for i in range(0, len(taskSet)):
            f.write('task{}'.format(i) + '\t' + str(taskSet[i][0]) + '\t' + str(taskSet[i][1]) + '\t' + str(taskSet[i][2]) + '\t' + str(taskSet[i][3]) + '\t' + str(taskSet[i][4]) + '\t' + str(taskSet[i][5]) + '\t' + str(taskSet[i][6]))
            f.write('\n')

if __name__ == "__main__":
    # parameter setting
    n = 32 # number of processor
    Sm = 0.8*n  # total capacity
    U = [0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0]  # max(U^N / S_m, U^O / S_m) 
    p_hu = 1 # probability of a task to be a high-utilization task
    u_max = 2.0 # the max ratio of overload utilization of a heavy task
    p_max =  0.3 # the max ratio of overload critical-pcd ath length and period
    p_hc = 0.5 # probability of a task to ba a HI-critical task
    r_max = 2.0 # the max ratio of overload and nominal utilizations
    e_max = 5 # the max ratio of T_max and T

    for UB in U:
        folder = "./test/{}_{}_{}_{}_{}_{}_{}_{}_{}".format(n, Sm, UB, p_hu, u_max, p_max, p_hc, r_max, e_max)
        mkdir(folder)
        for i in range(0, 1000):
            ProcessorSet = generateProcessor(n, Sm)
            taskSet = generateTaskSet(Sm, UB, p_hu, u_max, p_max, p_hc, r_max, e_max)
            filename = 'test{}.test'.format(i)
            path = folder + "/" + filename
            storeResults(ProcessorSet, taskSet, path)
        