import aiohttp
from aiohttp import ClientSession
import asyncio
import logging

async def fetch_status(session: ClientSession, url: str, delay: int = 0) -> int:
    await asyncio.sleep(delay)
    async with session.get(url) as result:
        return result.status


async def main():
    async with aiohttp.ClientSession() as session:
        fetchers = \
                [asyncio.create_task(fetch_status(session, 'python://bad.com')),
                 asyncio.create_task(fetch_status(session, 'https://www.example.com', delay=3)),
                 asyncio.create_task(fetch_status(session, 'https://www.example.com', delay=3))]

        done, pending = await asyncio.wait(fetchers, return_when=asyncio.FIRST_EXCEPTION)

        print(f'Done task count: {len(done)}')
        print(f'Pending task count: {len(pending)}')

        for done_task in done:
            if done_task.exception() is None:
                print(done_task.result())
            else:
                print('-----------')
                logging.error("Request got an exception", exc_info=done_task.exception())
                print('-----------')

        for pending_task in pending:
            pending_task.cancel()


asyncio.run(main())
