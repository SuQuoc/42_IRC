from tcp_client import tcpClient
import asyncio

async def main():
    print("connect 2 users: ", end="")
    async with tcpClient("niki") as niki, tcpClient("eule") as eule:
        await niki.userJoin()
        await eule.userJoin()
        
    print("ok")

    print("stress test 1000 clients: ", end="")
    await asyncio.gather(*(tcpClient(i).userJoinAndWrite() for i in range(1000)))
    print("ok")

asyncio.run(main())