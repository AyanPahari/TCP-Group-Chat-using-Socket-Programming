
# TCP Group Chat using Socket Programming

TCP group chat server where there is a single server and multiple clients and that server accepts the connections from multiple clients and enables them to chat with each other. A Client sends the message to the server and the server forwards the message to other connected clients. Everything is happening in a non-blocking way that is the client need not wait for the server to send a reply to the earlier message before sending another message.
## Instructions on running the program

Platform Used : Linux with G++

Compiling the Codes : 
    
    g++ file_name.cpp –pthread
For Server : 

    g++ server.cpp –pthread

For Client : 

    g++ client.cpp –pthread

Running the Server: 

    ./a.out Port_Num

Running the Client: 

    ./a.out User_Name Port_Num


- Ctrl+C to be used for stopping the Client and Server
- Port_Num provided to the client should be same as that provided to the Server
- User_Name is just a unique identity for the Client
## Screenshots

![Output Screenshot](output.jpg)

