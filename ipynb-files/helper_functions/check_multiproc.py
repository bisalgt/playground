# # from multiprocessing import Process

# # # def print_func(continent='Asia'):
# # #     print('The name of continent is : ', continent)

# # # if __name__ == "__main__":  # confirms that the code is under main function
# # #     names = ['America', 'Europe', 'Africa']
# # #     procs = []
# # #     proc = Process(target=print_func)  # instantiating without any argument
# # #     procs.append(proc)
# # #     proc.start()

# # #     # instantiating process with arguments
# # #     for name in names:
# # #         # print(name)
# # #         proc = Process(target=print_func, args=(name,))
# # #         procs.append(proc)
# # #         proc.start()


# # #     # complete the processes
# # #     for proc in procs:
# # #         proc.join()

# # import time
# # from multiprocessing import Process

# # def delay_sec(sec):
# #     print("Inside Function")
# #     time.sleep(sec)
# #     print("done")
# #     return True

# # proc = Process(target=delay_sec, args=(1,))
# # run_loop = True
# # count = 0
# # while run_loop:
# #     print("inside while")
# #     # global proc
# #     if not proc.is_alive():
# #         count += 1
# #         print("increase count!")
# #         # proc.terminate()
# #         proc = Process(target=delay_sec, args=(1,))
# #         proc.start()
# #         run_loop = False




# # print("end of file")


# # import multiprocessing
# # import time
# # number_of_processes = 5

# # def exec_process():
# #     time.sleep(2)

# # p = multiprocessing.Pool(processes = number_of_processes)
# # print(dir(p))
# # count = 0
# # if_check = True
# # while True:
# #     if if_check:
# #         count += 1
# #         p.apply_async(exec_process,)
# #         if p.is_alive():
# #             if_check = False
# #         else:
# #             if_check = True
# # p.close()
# # p.join()


# # import signal, os

# # def handler(signum, frame):
# #     print('Signal handler called with signal', _)

# # # print(dir(signal))

# # # Set the alarm to fire every second
# # signal.signal(signal.SIGALRM, handler)


# # # ... Do stuff - the alarm will keep firing ...

# # # Done: clear alarm
# # signal.alarm(0)



# import multiprocessing
# import time

# count = 0

# start = time.perf_counter()


# def do_something():
#     global count
#     print("sleep 2 sec...")
#     time.sleep(2)
#     count += 1
#     print("done sleeping...")
#     print(count, " returned")
#     return count
    
# p1 = multiprocessing.Process(target=do_something)



# finish = time.perf_counter()

# print(f"Finished {finish -start}")

# if __name__ == "__main__":
#     p1.start()


# import asyncio
# import socket

# s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# s.bind((socket.gethostname(), 1255))
# s.listen(5)

# count = 0
# async def periodic():
#     global count
#     while True:
#         print('periodic')
#         await asyncio.sleep(1)
#         count += 1
#         yield count



# # while True:
# #     clientsocket, address = s.accept()
# #     print(f"Connection from {address} has been established!")
# #     msg = count
    
# #     clientsocket.send(bytes(msg, "utf-8"))
# print("counted as : ", count)

# def stop():
#     task.cancel()

# loop = asyncio.get_event_loop()
# loop.call_later(5, stop)
# task = loop.create_task(periodic())

# try:
#     loop.run_until_complete(task)
# except asyncio.CancelledError:
#     pass



import asyncio

response = 0

async def handle(x):
    await asyncio.sleep(0.1)
    return x

async def run():
    global response
    for number in range(1, 21):
        # response = await handle(number)
        print(number)
        if response == 10:

            # run wait_for_next "in background" instead of blocking flow:
            asyncio.ensure_future(wait_for_next(response))

async def wait_for_next(x):
    while response == x:
        print('waiting',response,x)
        await asyncio.sleep(0.5)
    print('done')


if __name__ == '__main__':
    loop = asyncio.get_event_loop()
    loop.run_until_complete(run())