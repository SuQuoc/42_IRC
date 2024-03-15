import asyncio

IP = "127.0.0.1"
PORT = 6667
PASS = "pw1234567"

class tcpClient:
        
    def __init__(self, name):
        self.name = name

    async def __aenter__(self):
        self.reader, self.writer = await asyncio.open_connection(IP, PORT)
        return self
    
    async def writeMsg(self, message):
        self.writer.write(message.encode())
        await self.writer.drain()

    async def readMsg(self):
       data = await asyncio.wait_for(self.reader.read(600), 5)
       self.last_data = data.decode("utf-8")

    async def strcmpExit(self, data, cmpstr):
        if data != cmpstr:
            print(f'\nReceived: {data!r}')
            print(f'\ncmpstr  : {cmpstr!r}')
            exit(1)
        
    async def __aexit__(self, exc_type, exc, tb):
        self.writer.close()
        await self.writer.wait_closed()
        print(f"closed {self.name}")

async def main():
    async with tcpClient("niki") as user, tcpClient("eule") as eule:
        await user.writeMsg(f"PASS {PASS}\r\nUSER {user.name} 2 3 4\r\nNICK {user.name}\r\n")
        await user.readMsg()
        await user.strcmpExit(user.last_data, f":AfterLife 001 {user.name} :Welcome to the Internet Relay Network, {user.name}\r\n:AfterLife 005 {user.name} NICKLEN=9 CHANTYPES=#& CHANMODES=oiklt :are supported by this server\r\n")

        await eule.writeMsg(f"PASS {PASS}\r\nUSER {eule.name} 2 3 4\r\nNICK {eule.name}\r\n")
        await eule.readMsg()
        await eule.strcmpExit(eule.last_data, f":AfterLife 001 {eule.name} :Welcome to the Internet Relay Network, {eule.name}\r\n:AfterLife 005 {eule.name} NICKLEN=9 CHANTYPES=#& CHANMODES=oiklt :are supported by this server\r\n")

asyncio.run(main())