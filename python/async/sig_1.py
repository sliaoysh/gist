import asyncio, signal
from asyncio import AbstractEventLoop
from typing import Set

async def delay(delay_seconds: int) -> int:
  print(f'sleeping for {delay_seconds} second(s)')
  await asyncio.sleep(delay_seconds)
  print(f'finished sleeping for {delay_seconds} second(s)')
  return delay_seconds


def cancel_tasks():
    print('Got a SIGINT!')
    tasks: Set[asyncio.Task] = asyncio.all_tasks()
    print(f'Cancelling {len(tasks)} task(s).')
    [task.cancel() for task in tasks]


async def main():
    loop: AbstractEventLoop = asyncio.get_running_loop()
    loop.add_signal_handler(signal.SIGINT, cancel_tasks)
    await delay(100)


asyncio.run(main())
