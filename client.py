import asyncio
import websockets

SERVER_ADDR = "192.168.28.217"

async def receive_messages(websocket):
    while True:
        message = await websocket.recv()
        print(f"Received: {message}")

async def send_messages(websocket):
    while True:
        message = input("Enter your message: ")
        await websocket.send(message)

async def client_handler():
    async with websockets.connect(f"ws://{SERVER_ADDR}:8765") as websocket:
        await asyncio.gather(
            receive_messages(websocket),
            send_messages(websocket)
        )

# Explicitly create and set an event loop
loop = asyncio.new_event_loop()
asyncio.set_event_loop(loop)

# Run the client using the explicitly created event loop
loop.run_until_complete(client_handler())
