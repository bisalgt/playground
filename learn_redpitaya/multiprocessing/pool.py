from multiprocessing import Pool
import time
start = time.time()

def f(x):
    time.sleep(3)
    return x*x

if __name__ == '__main__':
    with Pool(6) as p:
        print(p.map(f, [1, 2, 3, 4, 5, 8]))

stop = time.time()


print(stop-start)

