from tcp_client import tcpClient
import asyncio

async def main():
    print("\n       BASIC TESTS\n", end="")
    print("connect 2 users: ", end="")
    async with tcpClient("niki") as niki, tcpClient("eule") as eule:
        await niki.userJoin()
        await eule.userJoin()
    print("ok")

    print("mode test +i: ", end="")
    async with tcpClient("niki") as niki, tcpClient("eule") as eule:
        await niki.userJoin()
        await eule.userJoin()
        await niki.modeTest()
        await eule.joinChannel(False)
    print("ok")

    print("stress test 1000 clients: ", end="")
    await asyncio.gather(*(tcpClient(i).userJoinAndWrite() for i in range(500)))
    print("ok")

asyncio.run(main())