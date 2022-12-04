
import asyncio

async def count(arg):
    print(f"{arg}")
    await asyncio.sleep(arg)
    print(f"{arg}")

    
async def main():
    print("hello 1")
    await asyncio.gather(count(3), count(2), count(1))
    print("world2")
    

if __name__ == "__main__":
        import time
        s = time.perf_counter()
        asyncio.run(main())
        elapsed = time.perf_counter() - s
        print(f"{__file__} executed in {elapsed:0.2f} seconds.")
