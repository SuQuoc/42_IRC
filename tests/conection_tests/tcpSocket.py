import asyncio

writers = []
lock = asyncio.Lock()

IP = "127.0.0.1"
PORT = 6667

def textRed(line): return '\033[91m' + line + '\033[0m'
def textGreen(line): return '\033[92m' + line + '\033[0m'
def textYellow(line): return '\033[93m' + line + '\033[0m'

async def makeUser(nick_name):
    await tcp_echo_client(f"PASS pw1234567\r\nUSER {nick_name} 2 3 4\r\nNICK {nick_name}\r\n", f":AfterLife 001 {nick_name} :Welcome to the Internet Relay Network, {nick_name}\r\n:AfterLife 005 {nick_name} NICKLEN=9 CHANTYPES=#& CHANMODES=oiklt :are supported by this server\r\n", nick_name)

async def read_data(reader, timeout):
    try:
        data = await asyncio.wait_for(reader.read(600), timeout=timeout)
        return data
    except asyncio.TimeoutError:
        return None
    except asyncio.CancelledError:
        pass

async def sendMsg(nick_name, writer, reader, msg):

    writer.write(msg.encode("utf-8"))
    await writer.drain()

    data = await read_data(reader, 1)
    await strcmpExit(data.decode("utf-8"), f":{nick_name}!{nick_name}@127.0.0.1 JOIN #666 * :{nick_name}\r\n", writer)
    while data := await read_data(reader, 1):
        if not data:
            break
        await strcmpExit(data.decode("utf-8"), f":1!1@127.0.0.1 JOIN #666 * :1\r\n:1!1@127.0.0.1 PRIVMSG #666 :under your bead is a nyancat!\r\n", writer)

async def strcmpExit(data, cmpstr, writer):
    if data != cmpstr:
        print(f'\nReceived: {data!r}')
        print(f'\ncmpstr  : {cmpstr!r}')
        writer.close()
        await writer.wait_closed()
        print(textRed("ERROR"), "Recv wrong or non msg!")
        exit(1)


async def tcp_echo_client(message:str, cmpstr:str, nickname):
    reader, writer = await asyncio.open_connection(IP, PORT)

    writer.write(message.encode())
    await writer.drain()

    data = await reader.read(600)
    if data.decode("utf-8") != cmpstr:
        print(f'Received: {data.decode("utf-8")!r}')
        print(f'cmpstr  : {cmpstr!r}')
        writer.close()
        await writer.wait_closed()
        print(textRed("ERROR"), "Recv wrong or non msg!")
        exit(1)

    writers.append({"w": writer, "r": reader})
    await sendMsg(nickname, writer, reader, "JOIN #666\r\nPRIVMSG #666 :under your bead is a nyancat!\r\n")

async def close_writers():
    for writer in writers:
        writer["w"].close()
        await writer["w"].wait_closed()

async def main():
    print(textYellow("\n  PASS AND JOIN TESTS\n"))
    print("welcome, join and privmsg: ", end="")
    clients = [makeUser(str(i)) for i in range(2)]
    await asyncio.gather(*clients)
    await close_writers()
    print(textGreen("OK"))

asyncio.run(main())
