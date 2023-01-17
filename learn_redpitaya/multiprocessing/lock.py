from multiprocessing import Process, Lock
import time
def f(l, i):
    l.acquire()
    try:
        time.sleep(1)
        print('hello world', i)
    finally:
        l.release()

def f2(i):
    time.sleep(1)
    print("hello process ", i)

if __name__ == '__main__':
    lock = Lock()

    for num in range(10):
        Process(target=f, args=(lock, num)).start()
        # Process(target=f2, args=(num,)).start()