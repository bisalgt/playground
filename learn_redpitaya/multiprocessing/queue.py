import multiprocessing as mp
import os
import time

print(os.getpid())

def func1(q):
    print('---func1-----')
    print(os.getpid())
    time.sleep(3)
    q.put('hello')

def func2(timestamp=None):
    print('---func2-----')
    print(os.getpid())
    time.sleep(2)


if __name__ == '__main__':
    mp.set_start_method('fork')
    q = mp.Queue()
    p1 = mp.Process(target=func1, args=(q,))
    p1.start()
    print(q.get())
    p1.join()