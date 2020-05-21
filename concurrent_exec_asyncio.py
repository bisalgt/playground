import time
import asyncio

async def display_time():
    print('inside display time')
    start_time = time.time()
    while True:
        dur =int(time.time() - start_time)
        
        if dur%3 == 0:
            print(f'{dur} seconds has passed')
        await asyncio.sleep(1) # this particular coroutine will be paused and allow other coroutine to run concurrently
        
async def prin_num():
    print('inside print_num')
    num = 1
    while True:
        print(num)
        num += 1
        await asyncio.sleep(0.1)

# we have only single thread, can do only one operation at a time due to GIL
        

async def main():
    task1 = asyncio.create_task(display_time())
    task2 = asyncio.create_task(prin_num())
    print('inside main')
    await asyncio.gather(task1, task2)
    print('end of all tasks')
    
asyncio.run(main())
