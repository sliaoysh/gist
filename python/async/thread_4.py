import requests
import asyncio

def get_status_code(url: str) -> int:
    response = requests.get(url)
    return response.status_code


async def main():
    urls = ['https://www.example.com' for _ in range(10)]
    tasks = [asyncio.to_thread(get_status_code, url) for url in urls]
    results = await asyncio.gather(*tasks)
    print(results)


asyncio.run(main())