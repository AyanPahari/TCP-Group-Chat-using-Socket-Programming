#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

//above are all the essential header files

using namespace std;

void *thread(void *socker); //declaration of the thread func

int main(int argc, char *argv[])
{
	struct sockaddr_in client_addr;
	pthread_t receiveT; //init the receiver thread

	if (argc < 3) //if number of arguments are less
	{
		cout << "Invalid format!!! Check the proper format in the readme file" << endl;
		exit(1);
	}

	if (argc > 3) //if number of arguments are more than necessary
	{
		cout << "Provided too many arguments..." << endl;
		exit(1);
	}
	char messege[1024];			  //buffer to store the messeges
	int Port_Num = atoi(argv[2]); //converts the port_num to integer format

	if (Port_Num == '\0') //checking for invalid ports
	{
		cout << "Invalid Port number provided";
		exit(1);
	}

	char name_of_user[1024];	   //buffer to store the name of the user
	strcpy(name_of_user, argv[1]); //copy it from the command line to the buffer

	if (name_of_user == NULL) //check for invalid usernames
	{
		cout << "Invalid name of user provided";
		exit(1);
	}

	int socker = socket(AF_INET, SOCK_STREAM, 0); //create a socket

	if (!socker) //check for any errors during socket creation
	{
		cout << "Error creating socket" << endl;
		exit(1);
	}

	//standard procedure to connect the socket to appropriate struct construct
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(Port_Num);
	client_addr.sin_addr.s_addr = INADDR_ANY;

	if (connect(socker, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) //check for any errors during the connection phase
	{
		cout << "connection failed to establish...." << endl;
		exit(1);
	}

	char ip_addr[INET_ADDRSTRLEN]; //this will store the ip address of the client

	inet_ntop(AF_INET, (struct sockaddr *)&client_addr, ip_addr, INET_ADDRSTRLEN);
	cout << "Connected to " << ip_addr << " ,Chat initiated..." << endl;

	if (pthread_create(&receiveT, NULL, thread, &socker) != 0) //create a thread and check if any error occured during its creation
	{														   //this func returns 0 if thread is successfully created
		cout << "Error while creating thread..." << endl;
		exit(1);
	}
	while (fgets(messege, 1024, stdin) > 0) //this loop will continue as long as client is sending info
	{
		char duplicate[1024]; //this is used to store the name of other client where the messege came from
		strcpy(duplicate, name_of_user);
		strcat(duplicate, ": ");
		strcat(duplicate, messege);
		//this will display the other client messege's with their name

		int length = write(socker, duplicate, strlen(duplicate));

		if (length < 0) //check if data sent or not
		{
			cout << "your messege was not sent" << endl;
			exit(1);
		}
		//clear the buffer so that fresh info can be stored
		memset(duplicate, 0, sizeof(duplicate));
		memset(messege, 0, sizeof(messege));
	}
	if (pthread_join(receiveT, NULL) != 0) //check if there is any error while joining the threads
	{
		cout << "Error while joining thread.." << endl;
		exit(1);
	}
	close(socker); //after everything is over, close the socket
	return 0;
}

void *thread(void *socker) //thread fuction that enables multi-threading
{
	int socket = *((int *)socker);
	char messege[1024];
	int length;
	while ((length = recv(socket, messege, 1024, 0)) > 0) //loop till we are receiving some data from other clients
	{
		if (length < 0) //check if we receive any valid data or not
		{
			cout << "Error receiving messeges..." << endl;
			exit(1);
		}
		messege[length] = '\0';
		fputs(messege, stdout);
		memset(messege, 0, sizeof(messege));
		//clear the buffers
	}
}