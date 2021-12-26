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

using namespace std;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;	  //init the thread for multi-threading
void send_to_all_clients(char *messege, int current); //func declr

struct info_on_client //struct declr
{
	int socket;
	char IP_adr[INET_ADDRSTRLEN];
};

void *receive_messege(void *sock); //func declr

int n;
int clients_info[1024]; //this will store the clients unique info

int main(int argc, char *argv[])
{
	struct sockaddr_in server_addr, client_addr;
	struct info_on_client client_info;
	char IP_addr[INET_ADDRSTRLEN]; //buffer to store the ip address of clients

	if (argc < 2) //check whether the format is valid or not
	{
		cout << "Invalid format!!! Check the proper format in the readme file" << endl;
		exit(1);
	}

	if (argc > 2) //check if too many arguments provided or not
	{
		cout << "You have provided too many arguments..." << endl;
		exit(1);
	}
	int Port_Num = atoi(argv[1]); //this will convert the port_num to integer format

	if (Port_Num == '\0') //check for invalid port_num
	{
		cout << "Invalid Port number provided";
		exit(1);
	}

	int max_client; //maximum number of clients to allow in this connection
	cout << "Enter the Maximum number of Clients allowed: ";
	cin >> max_client;

	cout << "---------------------Server Started---------------------------" << endl; //signal that everything till this step was successful

	int server_socket = socket(AF_INET, SOCK_STREAM, 0); //create a socket of tcp type

	if (!server_socket) //check if there are errors during socket creation
	{
		cout << "Error creating socket" << endl;
		exit(1);
	}
	//standard procedure to connect the socket to appropriate struct construct
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(Port_Num);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	socklen_t client_addr_size; //init client_addr_size as socklen_t

	client_addr_size = sizeof(client_addr);

	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) //binding the socket to appropriate address
	{
		cout << "binding not successful" << endl;
		exit(1);
	}

	if (listen(server_socket, 5) != 0) //atmost 5 clients can be present as backlog
	{
		cout << "listening not successful" << endl;
		exit(1);
	}

	pthread_t recvt; //init receiver thread

	while (1)
	{
		int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_size); //accept the incoming connections from clients
		if (client_socket < 0)																		   //check if accept was successful or not
		{
			cout << "accept unsuccessful" << endl;
			exit(1);
		}

		if (pthread_mutex_lock(&mutex) != 0) //check if mutex was properly locked or not
		{
			cout << "Error while locking the mutex..." << endl;
			exit(1);
		}

		inet_ntop(AF_INET, (struct sockaddr *)&client_addr, IP_addr, INET_ADDRSTRLEN);
		cout << IP_addr << " Connected " << endl;
		client_info.socket = client_socket;
		strcpy(client_info.IP_adr, IP_addr);
		clients_info[n] = client_socket;
		n++;
		if (pthread_create(&recvt, NULL, receive_messege, &client_info) != 0) //create the thread
		{
			cout << "Error while creating thread..." << endl;
			exit(1);
		}
		if (pthread_mutex_unlock(&mutex) != 0) //check if any errors occured while unlocking mutex
		{
			cout << "Error while unlocking mutex..." << endl;
			exit(1);
		}

		if (n == max_client + 1)
			break;
	}
	return 0;
}

void *receive_messege(void *socket)
{
	int length;			//length of the messege
	char messege[1024]; //buffer to store the messege
	struct info_on_client client = *((struct info_on_client *)socket);

	while ((length = recv(client.socket, messege, 1024, 0)) > 0) //keep the connection alive till we are receiving messeges
	{
		messege[length] = '\0';
		send_to_all_clients(messege, client.socket); //send the messege received to all the clients
		memset(messege, 0, sizeof(messege));
	}
	if (pthread_mutex_lock(&mutex) != 0) //if there is any error during mutex locking
	{
		cout << "Error while locking mutex...";
		exit(1);
	}
	cout << client.IP_adr << " disconnected" << endl;
	int id;
	for (int i = 0; i < n; i++) //to store the client info
	{
		if (clients_info[i] == client.socket)
		{
			id = i;
			while (id <= n - 2)
			{
				clients_info[id] = clients_info[id + 1];
				id++;
			}
		}
	}
	n = n - 1;
	if (pthread_mutex_unlock(&mutex) != 0) //chekc if there is any error during mutex unlocking
	{
		cout << "Error while unlocking mutex..." << endl;
		exit(1);
	}
}

void send_to_all_clients(char *messege, int current) //this func will send the incoming messege to all the other clients
{
	if (pthread_mutex_lock(&mutex) != 0) //check if there is any error during mutex locking
	{
		cout << "Error while locking mutex...";
		exit(1);
	}
	for (int i = 0; i <= n - 1; i++) //this will send the messege to all the clients except the current client
	{
		if (clients_info[i] == current)
		{
			continue;
		}
		else
		{
			if (send(clients_info[i], messege, strlen(messege), 0) < 0)
			{
				cout << "some failure occured while sending" << endl;
				continue;
			}
		}
	}
	if (pthread_mutex_unlock(&mutex) != 0) //check if there is any error during mutex unlocking
	{
		cout << "Error while unlocking mutex.." << endl;
		exit(1);
	}
}