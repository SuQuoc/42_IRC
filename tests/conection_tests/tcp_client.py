import asyncio

IP = "127.0.0.1"
PORT = 6667
PASS = "pw1234567"

class tcpClient:
        
    def __init__(self, name):
        self.name = name

    async def __aenter__(self):
        return self

    async def writeMsg(self, message):
        self.writer.write(message.encode())
        await self.writer.drain()

    async def readMsg(self):
       data = await asyncio.wait_for(self.reader.read(600), 10)
       self.last_data = data.decode("utf-8")

    async def strcmpExit(self, data, cmpstr):
        if data != cmpstr:
            print(f'\nReceived: {data!r}')
            print(f'\ncmpstr  : {cmpstr!r}')
            exit(1)

    async def userJoin(self):
        self.reader, self.writer = await asyncio.open_connection(IP, PORT)
        await self.writeMsg(f"PASS {PASS}\r\nUSER {self.name} 2 3 4\r\nNICK {self.name}\r\n")
        await self.readMsg()
        await self.strcmpExit(self.last_data, f":AfterLife 001 {self.name} :Welcome to the Internet Relay Network, {self.name}\r\n:AfterLife 005 {self.name} NICKLEN=9 CHANTYPES=#& CHANMODES=oiklt :are supported by this server\r\n")

    async def userJoinAndWrite(self):
        await self.userJoin()
        await self.writeMsg(f"JOIN #666\r\n")
        await self.readMsg()
        l = self.last_data.split("\r\n")
        await self.strcmpExit(l[0] + "\r\n", f":{self.name}!{self.name}@127.0.0.1 JOIN #666 * :{self.name}\r\n")
        await asyncio.sleep(1)
        for i in range(1000):
            await self.writeMsg(f"PRIVMSG #666 :Under your nyancat is a bead\r\n")

    async def joinChannel(self, can_join=True):
        await self.writeMsg(f"JOIN #666\r\n")
        await self.readMsg()
        if can_join == True:
            can_join = f":{self.name}!{self.name}@127.0.0.1 JOIN #666 * :{self.name}\r\n"
        else:
            can_join = f":AfterLife 473 {self.name} #666 :Cannot join channel (+i)\r\n"
        await self.strcmpExit(self.last_data, can_join)

    async def modeTest(self):
       await self.joinChannel()
       await self.writeMsg(f"MODE #666 +i\r\n")
       await self.readMsg()
       await self.strcmpExit(self.last_data, f":{self.name}!{self.name}@127.0.0.1 MODE #666 +i\r\n")

    async def __aexit__(self, exc_type, exc, tb):
        self.writer.close()
        await self.writer.wait_closed()

""" async def main():
    async with tcpClient("niki") as niki, tcpClient("eule") as eule:
        await niki.userJoin()
        await eule.userJoin()

asyncio.run(main()) """