import asyncio
import aiohttp
from aiohttp import ClientSession

async def fetch_status(session: ClientSession, url: str) -> int:
    t = aiohttp.ClientTimeout(total=.1)
    async with session.get(url, timeout=t) as result:
        return result.status


async def main():
    session_timeout = aiohttp.ClientTimeout(total=1, connect=.1)
    async with aiohttp.ClientSession(timeout=session_timeout) as session:
        status = await fetch_status(session, 'https://example.com')
        print(f'Status was {status}')


asyncio.run(main())
