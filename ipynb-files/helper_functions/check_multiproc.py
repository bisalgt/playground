# from multiprocessing import Process

# # def print_func(continent='Asia'):
# #     print('The name of continent is : ', continent)

# # if __name__ == "__main__":  # confirms that the code is under main function
# #     names = ['America', 'Europe', 'Africa']
# #     procs = []
# #     proc = Process(target=print_func)  # instantiating without any argument
# #     procs.append(proc)
# #     proc.start()

# #     # instantiating process with arguments
# #     for name in names:
# #         # print(name)
# #         proc = Process(target=print_func, args=(name,))
# #         procs.append(proc)
# #         proc.start()


# #     # complete the processes
# #     for proc in procs:
# #         proc.join()

# import time
# from multiprocessing import Process

# def delay_sec(sec):
#     print("Inside Function")
#     time.sleep(sec)
#     print("done")
#     return True

# proc = Process(target=delay_sec, args=(1,))
# run_loop = True
# count = 0
# while run_loop:
#     print("inside while")
#     # global proc
#     if not proc.is_alive():
#         count += 1
#         print("increase count!")
#         # proc.terminate()
#         proc = Process(target=delay_sec, args=(1,))
#         proc.start()
#         run_loop = False




# print("end of file")


import multiprocessing
import time
number_of_processes = 5

def exec_process():
    time.sleep(2)

p = multiprocessing.Pool(processes = number_of_processes)
print(dir(p))
count = 0
if_check = True
while True:
    if if_check:
        count += 1
        p.apply_async(exec_process,)
        if p.is_alive():
            if_check = False
        else:
            if_check = True
p.close()
p.join()