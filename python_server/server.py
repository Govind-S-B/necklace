import os
import asyncio
import websockets

WS_PORT = int(os.environ.get("WS_PORT", 8888))

connected_clients = set()

async def handle_client(websocket, path):
    connected_clients.add(websocket)
    try:
        async for message in websocket:
            print(message)
            for client in connected_clients:
                if client != websocket and client.open:
                    await client.send(message)
                else:
                    connected_clients.remove(client)
    finally:
        connected_clients.remove(websocket)

async def main():
    async with websockets.serve(handle_client, "localhost", WS_PORT):
        print(f"WS server is listening at ws://localhost:{WS_PORT}")
        await asyncio.Future()  # run the server indefinitely

asyncio.run(main())
