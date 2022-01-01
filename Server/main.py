import socket

sock=socket.socket()

sock.bind(("0.0.0.0",4456))
sock.listen()

while True:
    client,addr=sock.accept()
    data=client.recv(1024)
    print(data.decode())