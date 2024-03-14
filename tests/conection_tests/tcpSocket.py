import asyncio
from time import sleep

writers = []
lock = asyncio.Lock()

async def makeUser(nick_name):
    await tcp_echo_client(f"PASS pw1234567\r\nUSER 1 2 3 4\r\nNICK {nick_name}\r\n", f":AfterLife 001 {nick_name} :Welcome to the Internet Relay Network, 1\r\n:AfterLife 005 {nick_name} NICKLEN=9 CHANTYPES=#& CHANMODES=oiklt :are supported by this server\r\n")
    await sendMsg("JOIN #666\r\nPRIVMSG #666 :under your bead is a nyancat!")

async def sendMsg(msg):
    async with lock:
        for client in writers:
            client["w"].write(msg.encode("utf-8"))
            await client["w"].drain()
            data = await client["r"].read(600)
            """ print(data.decode()) """


async def tcp_echo_client(message:str, cmpstr:str):
    reader, writer = await asyncio.open_connection('127.0.0.1', 6667)

    print(f'Send: {message!r}')
    writer.write(message.encode())
    await writer.drain()

    data = await reader.read(600)
    if data.decode("utf-8") == cmpstr:
        print(f'Received: {data.decode("utf-8")!r}')
        print(f'cmpstr  : {cmpstr!r}')
        writer.close()
        await writer.wait_closed()
        print("ERROR")
        exit(1)

    writers.append({"w": writer, "r": reader})

async def close_writers():
    for writer in writers:
        writer["w"].close()
        await writer["w"].wait_closed()
        print('Close the connection')
    

async def main():
    clients = [makeUser(str(i)) for i in range(100)]
    await asyncio.gather(*clients)
    await close_writers()

asyncio.run(main())
