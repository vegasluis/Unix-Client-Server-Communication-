//CPE-400 
//SECTION: 1001 
//Luis Ruiz 
//Client code

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


#define TRUE 1
#define FALSE 0

//display error messages
void error(char *msg)
{
	perror(msg);
	exit(0);
}

void getIp(struct sockaddr_in serv_addr,struct sockaddr_in cli_addr)
{
	//get the server ip and convert it to a readable string
	struct sockaddr_in* IPV4_client = (struct sockaddr_in*)&cli_addr;
	
	//string that will hold the servers ip 
	char client[INET_ADDRSTRLEN];
	inet_ntop( AF_INET, &IPV4_client, client, INET_ADDRSTRLEN );

	//get the client ip and convert it to a readable string
	struct sockaddr_in* IPV4_server = (struct sockaddr_in*)&serv_addr;
	struct in_addr server_addr = IPV4_server->sin_addr;

	//string that will hold the clients ip 
	char server_ip[INET_ADDRSTRLEN];
	inet_ntop( AF_INET, &server_addr, server_ip, INET_ADDRSTRLEN );	

	//print to stdout the two ips
	fprintf(stderr,"\nServer:%s\nClient:%s\n",server_ip,client);
}

int main (int argc, char *argv[])
{	
	int sockfd,  portno, n;							
	struct sockaddr_in serv_addr, cli_addr;
	struct hostent *server;			
	char buffer[500];				

	//check if hostname of server and port number is provided
	if (argc < 3)
	{
		fprintf(stderr, "%s\n", "ERROR:");
		fprintf(stderr,"usage %s hostname port\n", argv[0]);
		exit(0);
	}

	//get desired port connection 
	portno = atoi(argv[2]);
	
	//creating our socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);	
	if (sockfd < 0)
		error("ERROR opening socket");
	
	//get the host name
	server = gethostbyname(argv[1]);
	if (server == NULL)
	{
		fprintf(stderr,"ERROR, no such host\n");
		exit(0);
	}
	
	//zero out the address first
	bzero((char *) &serv_addr, sizeof(serv_addr));	

	//assign values to serv_addr variable	
	serv_addr.sin_family = AF_INET;		//for IPv4 communication
		//copy h_addr to s_addr 
	bcopy((char *)server->h_addr,(char *) &serv_addr.sin_addr.s_addr,server->h_length);

	//port number
	serv_addr.sin_port = htons(portno);		
	
	getIp(serv_addr,cli_addr);	

	//requesting connection to server 
	if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("ERROR connecting");
	
while(TRUE)
{
	bzero(buffer,500);	

	fprintf(stderr, "%s\n", "If you wish to exit, you can CTRL+C or simply press enter");
	fprintf(stderr, "%s","Enter string you wish to transmit:" );		

	//get string				
	fgets(buffer,500,stdin);
	
	//if the buffer is empty end the client communication
	if(strlen(buffer) == 1 )
		return 0;
	
	//sending message to host server	
	send(sockfd,buffer,sizeof(buffer),0);

	bzero(buffer,500);
	
	//recieving message from host server				
	recv(sockfd,buffer,sizeof(buffer),0);
	printf("%s\n",buffer);				

	bzero(buffer,500);							
}

	//close file descriptor
	if(close(sockfd) < 0)
		error("ERROR: on closing");

	return 0; 
}
