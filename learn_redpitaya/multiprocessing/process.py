from multiprocessing import Process
import os
print(os.getpid())
def f(name):
    print(os.getppid()) # get parent process id
    print(os.getpid()) # get process id of current process
    print('hello', name)

if __name__ == '__main__':
    p = Process(target=f, args=('bob',))
    p.start()
    p.join()