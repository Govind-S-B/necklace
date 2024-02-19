import asyncio
import websockets

connected_clients = set()

async def server_handler(websocket, path):
    connected_clients.add(websocket)
    print("client connected")
    try:
        async for message in websocket:
            print(f"Received: {message}")
            response = f"Server: You sent '{message}'"
            await websocket.send(response)
    finally:
        connected_clients.remove(websocket)

start_server = websockets.serve(server_handler, "0.0.0.0", 8765)

# Explicitly create and set an event loop
loop = asyncio.new_event_loop()
asyncio.set_event_loop(loop)

# Run the server using the explicitly created event loop
loop.run_until_complete(start_server)
loop.run_forever()
