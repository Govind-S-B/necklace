import asyncio
import websockets

SERVER_ADDR = "192.168.29.147"

async def client_handler():
    async with websockets.connect(f"ws://{SERVER_ADDR}:8765") as websocket:
        while True:
            pass
            # message = await websocket.recv()
            # print(f"Received: {message}")

asyncio.get_event_loop().run_until_complete(client_handler())
