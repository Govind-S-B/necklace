import asyncio
import websockets

async def receive_messages(websocket):
    while True:
        message = await websocket.recv()
        print(f"Received: {message}")

async def send_messages(websocket):
    while True:
        message = input("Enter your message: ")
        await websocket.send(message)

async def client_handler():
    async with websockets.connect("ws://192.168.29.119:8765") as websocket:
        await asyncio.gather(
            receive_messages(websocket),
            send_messages(websocket)
        )

asyncio.get_event_loop().run_until_complete(client_handler())
