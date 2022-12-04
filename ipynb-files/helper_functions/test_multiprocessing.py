# import multiprocessing
# import time

# def sleepy_man():
#     print('Starting to sleep')
#     time.sleep(1)
#     print('Done sleeping')

# tic = time.time()
# p1 =  multiprocessing.Process(target= sleepy_man)
# p2 =  multiprocessing.Process(target= sleepy_man)
# p1.start()
# p2.start()
# p1.join()
# p2.join() 
# toc = time.time()

# print('Done in {:.4f} seconds'.format(toc-tic))


# import asyncio

# async def count():
#     print("One")
#     await asyncio.sleep(1)
#     print("two")
    
# async def main():
#     await asyncio.gather(count(), count(), count())
    

# if __name__ == "__main__":
#         import time
#         s = time.perf_counter()
#         asyncio.run(main())
#         elapsed = time.perf_counter() - s
#         print(f"{__file__} executed in {elapsed:0.2f} seconds.")


from multiprocessing import Process

def print_func(continent='Asia'):
    print('The name of continent is : ', continent)

if __name__ == "__main__":  # confirms that the code is under main function
    names = ['America', 'Europe', 'Africa']
    procs = []
    proc = Process(target=print_func)  # instantiating without any argument
    procs.append(proc)
    proc.start()

    # instantiating process with arguments
    for name in names:
        # print(name)
        proc = Process(target=print_func, args=(name,))
        procs.append(proc)
        proc.start()

    # complete the processes
    for proc in procs:
        proc.join()
