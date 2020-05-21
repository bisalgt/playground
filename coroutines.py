import asyncio

async def main():
    print('hello coroutines')
    await asyncio.sleep(2)
    print('end of the coroutine')

#loop = asyncio.get_event_loop()
#loop.run_until_complete(main())
#loop.close()

# the above three lines can be done in python3.7 in a single line as,
asyncio.run(main())
