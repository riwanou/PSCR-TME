project('TME8', 'cpp', default_options: ['cpp_std=c++17'])

src = ['src/Socket.cpp', 'src/ServerSocket.cpp', 'src/TCPServer.cpp', 'src/utils_ftp.cpp']
executable('client', 'src/client.cpp', src)
executable('server', 'src/server.cpp', src)
executable('server2', 'src/server2.cpp', src)

executable('client_ftp', 'src/client_ftp.cpp', src)
executable('server_ftp', 'src/server_ftp.cpp', src)
