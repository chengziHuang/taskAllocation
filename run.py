import os
import subprocess
from unittest import result
import datetime


result_list = []

starttime = datetime.datetime.now()
for root, dirs, files in os.walk('./'):
    for file in files:
        if os.path.splitext(file)[1] == '.test':
            print(file)
            output = subprocess.Popen(["./allocation", file], stdout=subprocess.PIPE).communicate()[0]
            output = str(output)

            if ('success' in output):
                result_list.append(1)
            if ('failure' in output):
                result_list.append(0)


ratio = sum(result_list) / len(result_list)
print("Acceptance ratio: ", ratio)
print("Time: ", (datetime.datetime.now() - starttime).seconds)
dir_name = os.path.basename(os.getcwd())
parameter = dir_name.split("_")


with open("./result.txt", "w") as f:
    f.write("n = {}, Sm = {}, UB = {}, p_hu = {}, u_max = {}, p_max = {}, p_hc = {}, r_max = {}, e_max = {} \n".format(parameter[0], parameter[1], parameter[2], parameter[3], parameter[4], parameter[5], parameter[6], parameter[7], parameter[8]))
    f.write("Acceptance ratio: {} \n".format( ratio))
    f.write("Time: {} \n".format( (datetime.datetime.now() - starttime).seconds))
