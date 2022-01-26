import time
start = time.perf_counter()

def find_sum(n):
    return sum(list(range(n)))

def main():
    s1 = find_sum(100000000)
    s2 = find_sum(100050)
    s3 = find_sum(100222)
# print(stop-start)

if __name__=='__main__':
    main()
stop = time.perf_counter()

print(stop-start)
