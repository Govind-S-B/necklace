import asyncio
import websockets

connected_clients = set()

async def server_handler(websocket, path):
    connected_clients.add(websocket)
    print("client connected")
    try:
        async for message in websocket:
            for client in connected_clients:
                await client.send(message)
    finally:
        connected_clients.remove(websocket)

start_server = websockets.serve(server_handler, "0.0.0.0", 8765)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
