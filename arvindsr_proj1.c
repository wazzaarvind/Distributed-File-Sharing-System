#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/nameser.h>
#include<netdb.h>
#include<unistd.h>
#include<stdlib.h>
#include<net/if.h>
#include<netinet/in.h>
#include<sys/ioctl.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<sys/wait.h>
#include<sys/time.h>

#define LENGTH 512

// Structure used for SERVER IP List as well as Active Connections List on Client Side

struct list_client
{
int id;
char hostname[40];
char ip_address[40];
char port_no[20];
int sock_no;
};




char sip[INET6_ADDRSTRLEN];


struct list_client l[10],ac[10];


// Error Function

void error(char *message)
{
	perror(message);
	exit(1);
}


//Function to Display Menu List

void help()
{
	printf("\n1.HELP : Gives Information about the List of Options \n2.CREATOR : Gives details about the Creator of this project \n3.DISPLAY : Displays the IP Address and Port Number of the Server/Client\n4.REGISTER : Register a Client with the Server\n5.CONNECT : Connect between clients that are connected to a common Server\n6.LIST : List out the Server IP List on the Server Side or the list of Active Connections on the Client Side\n7.TERMINATE : Terminate an Active Connection \n8.QUIT : Quit the program\n9.GET : Get a file from a destionation host mapped to an ID\n10.PUT : Upload a file onto a particular host\n");
}


// Function to display IP Address and Port Number of Server/Client
void display(char port_no[])
{	
	

	char *ip_version;
	char s[INET6_ADDRSTRLEN];
	char host_n[30];
	gethostname(host_n,sizeof host_n);
	struct addrinfo *p,*info_server,hints;
	void *addr;
	int rev;
	memset(&hints, 0, sizeof hints);
  	hints.ai_family=AF_UNSPEC;
  	hints.ai_socktype=SOCK_STREAM;
  	hints.ai_flags=AI_PASSIVE; 
  	rev=getaddrinfo(host_n,port_no,&hints,&info_server);

    if(rev!=0)
   	{
    
    	fprintf(stderr,"getaddrinfo:%s\n", gai_strerror(rev));
    
    	//return 1;
   	}
   	
   	p=info_server;

  	while(p!=NULL)
  	{
   		
   		if(p->ai_family==AF_INET)
   		{

    		struct sockaddr_in *ipv4=(struct sockaddr_in *)p->ai_addr;
    		addr = &(ipv4->sin_addr);
    		ip_version = "IPv4";
   		
   		}

   		inet_ntop(p->ai_family,addr,sip,sizeof sip);
   		p=p->ai_next;

   	}
   	

   	printf("\nThe IP %s\n", inet_ntop(info_server->ai_family,addr,s, sizeof s));
   	printf("\nThe Port Number is %s\n",port_no);




}

//Function to Display details about the creator of this Project

void creator()
{
	
	printf("\nStudent's Name : Arvind Srinivass Ramanathan \nUBIT Name : arvindsr \nUB Email Address : arvindsr@buffalo.edu\n");
}


//Function to obtain pointer to address

void *get_in_addr(struct sockaddr *sa)
{

    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


// Main Server Code

int server(char port_no[])
{ 
	
	char msg2[500];
	printf("\nHey! This is a Server process!\n");
	display(port_no);
	char message[500];
	struct addrinfo *info_client,*info_server,*info_peer,hints,*p;
    int fd,max_fd,c=0,k=0,return_val,len=0,z=0,m;
	fd_set fds,readfds,kfds;
	struct sockaddr_storage cl_address;
	int connc=0;
	char reg_msg[2500];
	struct timeval tv;
	int i,n,sock_fd,client_length,newsockfd,check_read,num_socks=0,max_socks=10;
	char buff[256],buffer[256],port_name[6],host_name[120],temp_ip[100];
	struct sockaddr_in server_address,client_address;
	char client_name[INET6_ADDRSTRLEN];
	char s[INET6_ADDRSTRLEN];
	char word[30]="\0";
	char word1[5];
	socklen_t size;
	void *addr;
	char *ip_version;
	char host_n[30];
  	char *token,*token1,*token2,*token3;


  	memset(&hints, 0, sizeof hints);
  

  	hints.ai_family=AF_UNSPEC;
  	hints.ai_socktype=SOCK_STREAM;
  	hints.ai_flags=AI_PASSIVE; 

  	l[0].id=1;
  	strcpy(l[0].port_no,port_no);

   	int rev;
   	gethostname(host_n,sizeof host_n);
   	strcpy(l[0].hostname,host_n);

   	//Getting Address information of the given host

  	rev=getaddrinfo(host_n,port_no,&hints,&info_server);

    if(rev!=0)
   	{
    
    	fprintf(stderr,"getaddrinfo:%s\n", gai_strerror(rev));
    
    	//return 1;
   	}

   	p=info_server;

  	
  	while(p!=NULL)
  	{
   		
   		if(p->ai_family==AF_INET)
   		{

    		struct sockaddr_in *ipv4=(struct sockaddr_in *)p->ai_addr;
    		addr = &(ipv4->sin_addr);
    		ip_version = "IPv4";
   		
   		}
   		
   		inet_ntop(p->ai_family,addr,s,sizeof s);
   		p=p->ai_next;
   	}

   	strcpy(l[0].ip_address,s);
   	c=0;
   	fflush(stdout);

   	//Creating a socket(Main Server Socket)
  	sock_fd=socket(info_server->ai_family,info_server->ai_socktype,info_server->ai_protocol);

   
	
 
	if(sock_fd<0)
	{
		error("\nCannot Open Socket : Error\n");
		
	}
	

	l[0].sock_no=sock_fd;
	
	

	//Refreshing

	if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&(int){ 1 },sizeof(int))==-1)
  	{
   	error("\nError while setting socketopt\n");
   	exit(1);
  	}


  	//Binding the Socket

	if((bind(sock_fd,info_server->ai_addr,info_server->ai_addrlen))<0)
   		error("\nEncountered error while binding!\n");
   
   	//Listening for connections
	if(listen(sock_fd,SOMAXCONN)==-1)
		perror("\nEncountered error while trying to Listen!\n");



	client_length=sizeof(client_address);
	
	//Setting all File Descriptors to 0
	FD_ZERO(&fds);
	FD_ZERO(&readfds);	

	// Setting the Master Socket to main socket and 0 for Keyboard input

	FD_SET(sock_fd,&fds);
	FD_SET(0,&fds);
	max_fd=sock_fd;

	//Keep looping

	while(1)
	{	
		
		readfds=fds;
		tv.tv_sec=2;
		tv.tv_usec=0;
		
		check_read=select(max_fd+1,&readfds,NULL,NULL,NULL);
		if(check_read==-1)
		{
			//check
			perror("\nSelect causes an error!\n");
			continue;
		}
		
		// Loop through all the file descriptors

		for(i = 0; i <= max_fd; i++) 
		{	
			//Check if the readfds file descriptor is set

            if (FD_ISSET(i, &readfds)) 
            	{ 
            		//There are 3 cases to Consier
            		//Case 1 : New Client
            		//Case 2 : Standard INPUT
            		//Case 3 : Data from Clients
                	 	//Case 1
                	 	if(i==sock_fd) // Handle New Connections from Clients

                	 	{
	                    	
	                    	size=sizeof cl_address;
	                    	//check
	                    	newsockfd = accept(sock_fd,(struct sockaddr *)&cl_address,&size);

	                    	if (newsockfd == -1)
	                    	{
	                        	perror("\nError while accepting Client connection\n");
	                    	}
	                    	if(c>=6)
	                    	{
	                    		printf("\nMaximum Connections exceeded!\n");
	                    		close(newsockfd);
	                    		continue;
	                    	}
	                    	
	                        FD_SET(newsockfd, &fds); // add to master set
	                        if (newsockfd > max_fd) 
	                        {    // keep track of the max
	                           	max_fd = newsockfd;
	                        }
	                        
	                        //Store the client details on Server IP List
	                        c++;
	                        l[c].id=c+1;
	                        l[c].sock_no=newsockfd;
	                        strcpy(l[c].ip_address,inet_ntop(cl_address.ss_family,get_in_addr((struct sockaddr*)&cl_address),s, INET6_ADDRSTRLEN));
	                        printf("\nThere is a new Client connection from the client %s on the socket %d\n",inet_ntop(cl_address.ss_family,get_in_addr((struct sockaddr*)&cl_address),s, INET6_ADDRSTRLEN),newsockfd);
	                        //check
	                        //check c
	                    }



	                    // Case 2 // Keyboard Input
	                    else if(i==0)

	                    {	int v=0;
	                    	fgets(buff,sizeof(buff),stdin);
							len = strlen(buff) - 1;
				     		if (buff[len] == '\n')
				            buff[len] = '\0';
				        	for(v=0;buff[v]!=' ';v++)
				        	{
				        	
				        	buff[v]=toupper(buff[v]);
				        	

				        	}
				        	



				         	if(strcmp("HELP",buff)==0)
				         	{	
				         		help();
				         	}
				         	

				         	else if(strcmp("CREATOR",buff)==0)
				         		creator();
				         	
				         	
				         	//Display IP and port
				         	else if(strcmp("DISPLAY",buff)==0)
				         	{
				         		display(port_no);
				         		
         					}	
         					
         					
         					//Below commands not available for Server

         					else if(strcmp("REGISTER",buff)==0)
         						printf("\nREGISTER command not available for server\n");

         					

         					else if(strcmp("CONNECT",buff)==0)
         						printf("\nCONNECT command not available for server\n");
         					

         					else if(strcmp("GET",buff)==0)
         						printf("\nGET command not available for server\n");
         					

         					else if(strcmp("PUT",buff)==0)
         						printf("\nPUT command not available for server\n");

         					else if(strcmp("TERMINATE",buff)==0)
         						printf("\nTERMINATE command not available for server\n");

         					//QUIT Server
         					else if(strcmp("QUIT",buff)==0)
         						exit(1);
         					
         					//Display SERVER IP List
         					else if(strcmp("LIST",buff)==0)
         					{
         						printf("\nID \t Host Name \t\t\t      IP Address \t Port \t    Socket \n");
									for(k=0;k<=c;k++)
									{	
													
										printf("\n%d \t", l[k].id);
										printf("%s\t", l[k].hostname);
									
										strcat(reg_msg,l[k].hostname);
									
										printf("   %s\t", l[k].ip_address);
										
								
										printf("\t%s\t",l[k].port_no);
										printf("\t   %d\n",l[k].sock_no);
									}
         					}
         					





	                    }


	                    //Case 3  //Data Received from Clients
	                    else
	                    {
	                    	
	                    	// Look for a message from Clients
	                    	n=recv(i,message,sizeof message,0);
	                    	
	                    	// Proper Message not received
	                    	if(n<=0)
	                    	{
	                    		//Some Client closed or Quit
	                    		if(n==0)
	                    		{
	                    	
	                    			int p=0;
	                    	
	                    			if(getpeername(i,(struct sockaddr *)&cl_address,&size)<0)
									{
										printf("\nError with the Server\n");
										return 0;
									}

	                    			printf("\nSERVER : Connection hung up or Quit at socket %d!\n",i);
	                    			fflush(stdout);
	                    			
	                    			

	                    			// Shift records up in the SERVER IP List(Structure)
	                    			for(k=0;k<c;k++)
	                    			{
	                    				
	                    				if(l[k].sock_no==i)
	                    				{
	                    					
	                    					
	                    					for(p=k;p<=c-1;p++)
	                    					{
	                    					
	                    						l[p].sock_no=l[p+1].sock_no;
	                    						strcpy(l[p].ip_address,l[p+1].ip_address);
	                    						strcpy(l[p].hostname,l[p+1].hostname);
	                    						strcpy(l[p].port_no,l[p+1].port_no);
	                    						
	                    					}


	                    				}

	                    			}
	                    				                    			
	                    			
	                    			strcpy(reg_msg,"List of Registerd Clients \n");
									strcat(reg_msg,"\nID \t Host Name \t\t\t  IP Address \tPort \t   Socket \n");

									// Constructing message to send back

									for(k=0;k<c;k++)
									{	
													
										
										sprintf(word1,"%d",l[k].id);
										strcat(reg_msg,word1);
										
										strcat(reg_msg,"\t");
										strcat(reg_msg,l[k].hostname);
										strcat(reg_msg,"\t");
										
										strcat(reg_msg,l[k].ip_address);
										strcat(reg_msg,"\t");
										
										strcat(reg_msg,l[k].port_no);
										strcat(reg_msg,"\t\t");
										sprintf(word1,"%d",l[k].sock_no);
										strcat(reg_msg,word1);
										strcat(reg_msg,"\n\n");

									}


									printf("\n%s\n",reg_msg);
									
									//Sending back message to all connected clients through socket, notifying them of changes

									for(k=l[1].sock_no;k<=l[1].sock_no+c-1;k++)
									{
										
										
										
											n=send(k,reg_msg,strlen(reg_msg),0);
											//if(n==-1)
												//perror("\nSending failed\n");
										
	                    		}

	                    		bzero(reg_msg,sizeof reg_msg);
	                    	}

	                    		else
	                    			perror("\nError or Receive\n");

	                    		//Closing and clearing the socket that just quit

	                    		close(i);
	                    		FD_CLR(i,&fds); 
	                    		
	                    		c--;

	                    		//No Connections remaining
	                    		if(c==0)
	                    			printf("\nAll connections are closed!\n");


	                    		else
	                    		{
	                    			strcpy(reg_msg,"List of Registerd Clients \n");
									strcat(reg_msg,"\nID \t Host Name \t\t\t  IP Address \tPort \t   Socket \n");
									
									for(k=0;k<=c;k++)
									{	
													
										
										sprintf(word1,"%d",l[k].id);
										strcat(reg_msg,word1);
										
										strcat(reg_msg,"\t");
										strcat(reg_msg,l[k].hostname);
										strcat(reg_msg,"\t");
										strcat(reg_msg,l[k].ip_address);
										strcat(reg_msg,"\t");
										strcat(reg_msg,l[k].port_no);
										strcat(reg_msg,"\t\t");
										sprintf(word1,"%d",l[k].sock_no);
										strcat(reg_msg,word1);
										strcat(reg_msg,"\n\n");


									}
									
	                    		}
	                    		// Remove from Master Set

	                    	}



	                    	else
	                    	{	

	                    		// Registered Client sends port, processing done

	                    		token=strtok(message," ");
	                    		if(strcmp(token,"Port")==0)
	                    		{
	                    			token=strtok(NULL," ");
		                    		strcpy(l[c].port_no,token);
		                    		token=strtok(NULL," ");
		                    		token=strtok(NULL," ");
		                    		strcpy(l[c].hostname,token);
		                    		// Data is Received from Client
		                    		strcpy(reg_msg,"List of Registerd Clients \n");
									strcat(reg_msg,"\nID \t Host Name \t\t\t  IP Address \tPort \t   Socket \n");
									
									

									//Constructing and sending message to all other old clients about the new client

									for(k=0;k<=c;k++)
									{	
													
										
										sprintf(word1,"%d",l[k].id);
										strcat(reg_msg,word1);
										
										strcat(reg_msg,"\t");
										strcat(reg_msg,l[k].hostname);
										strcat(reg_msg,"\t");
										
										strcat(reg_msg,l[k].ip_address);
										strcat(reg_msg,"\t");
										
										strcat(reg_msg,l[k].port_no);
										
										strcat(reg_msg,"\t\t");
										sprintf(word1,"%d",l[k].sock_no);
										strcat(reg_msg,word1);
										strcat(reg_msg,"\n");

									}
									
									
									printf("\n%s\n",reg_msg);
									for(k=l[1].sock_no;k<=l[1].sock_no+c-1;k++)
									{
										
									n=send(k,reg_msg,strlen(reg_msg),0);
									//if(n==-1)
										//perror("\nSending failed\n");
									}

								}

	                    	}

	                    	bzero(message,sizeof message);
	                    	bzero(reg_msg,sizeof reg_msg);

	                    }
	                }
	            }
	      	}
	    }




// Main Client Function

int client(char port_no[])
{
	printf("\nHey! This is a Client process!\n");
	time_t now;
	char tempfile[150];
	display(port_no);
	char reg_msg[250];
	char message[500],msg2[1000],ipc_temp[100];
	int max_fd,fd_new;
	int count_reg=0,sock_fd,client_length,fd,newsockfd,r_sock_fd,csock_fd,cls,conn_sock;
	struct hostent *server;
	int connc,r=0,z=0;
	char buffer[256];
	int fs_block_sz;
	
	int k=0;
	char *token,*token1,*token2;
	char buff[256],host_name[50];
	struct sockaddr_in server_address,client_address;
	struct sockaddr_storage cl_address;
	struct addrinfo *info_client,*info_server,*info_peer,hints,*p;
	int n,return_val,len=0;
	gethostname(host_name,500);
	fd_set fds,kfds,readfds;
	struct timeval tv;
	socklen_t size;
	void *addr;
	char chostname[50];
	char *ip_version;
	char sdbuf[LENGTH];
	char client_name[INET6_ADDRSTRLEN];	
	char s[INET6_ADDRSTRLEN];
	int check_read,i;
	int rev,y=0;
	int fst=0;
	FILE *f;
	char msg1[500];
	int fstart=0;
	

	

	connc=0;


	memset(&hints, 0, sizeof hints);
  	hints.ai_family=AF_UNSPEC;
  	hints.ai_socktype=SOCK_STREAM;
  	hints.ai_flags=AI_PASSIVE; 



  	//Getaddrinfo

  	rev=getaddrinfo(NULL,port_no,&hints,&info_client);
  	if(rev!=0)
  	{
  		fprintf(stderr,"getaddrinfo:%s\n", gai_strerror(rev));
    	return 1;
  	}
	
	p=info_client;


	while(p!=NULL)
   	{
   		if(p->ai_family==AF_INET)
    	{
     		struct sockaddr_in *ipv4=(struct sockaddr_in *)p->ai_addr;
     		addr=&(ipv4->sin_addr);
     		ip_version="IPv4";
    	}

   		inet_ntop(p->ai_family,addr,s,sizeof s);
   		p=p->ai_next;
    }
 
     
    //Creating Main Socket on Client

    sock_fd=socket(info_client->ai_family,info_client->ai_socktype,info_client->ai_protocol);
    

    if(sock_fd<0)
    {
   		printf("\nEncountered an error while connecting socket!\n");
   		exit(0);
    }

    if(setsockopt(sock_fd,SOL_SOCKET,SO_REUSEADDR,&(int){ 1 },sizeof(int))==-1)
  	{
   		perror("\nSetsockopt error\n");
   		exit(1);
  	}


	if((bind(sock_fd,info_client->ai_addr,info_client->ai_addrlen))<0)
    {
    	printf("\nEncountered an error while binding!\n");
    	exit(0);
    }

	
	

    //Listening for connections from Client as it is a Server in itself
	if(listen(sock_fd,SOMAXCONN)==-1)
		perror("\nEncountered error while trying to Listen!\n");

	
	
	//Set file descriptors to zero
	FD_ZERO(&fds);
	FD_ZERO(&readfds);
	//FD_ZERO(&kfds);
	//FD_SET(fd,&kfds);

	//Look for Keyboard and clients and messages
	FD_SET(0,&fds);
	FD_SET(sock_fd,&fds);
	max_fd=sock_fd;


	//Loop through

	while(1)
	{	
		gethostname(host_name, sizeof host_name);
		
	    readfds=fds;
		tv.tv_sec=2;
		tv.tv_usec=0;
		check_read=select(max_fd+1,&readfds,NULL,NULL,NULL);



		if(check_read==-1)
		{
			perror("\nSelect causes an error!\n");
			continue;
		}


		for(i = 0; i <= max_fd; i++) 
		{
            if (FD_ISSET(i, &readfds)) 
            	{ 
                	
                	 	if(i==sock_fd) // New Client connection has arrived
                	 	{
                	 		if(z>=4)
                	 		{
                	 			printf("\nNo more peers allowed!\n");
                	 			
                	 		}
                	 	else
                	 	{

               	 		
                    	
                    	size=sizeof cl_address;

                    	//Accpet the New Client connection
                    	//check
                    	newsockfd = accept(sock_fd,(struct sockaddr *)&cl_address,&size);

                    	if (newsockfd == -1)
                    	{
                        	perror("\nError while accepting connection\n");
                    	}
                    	//else
                    	//{
                        	FD_SET(newsockfd, &fds); // add to master set
                        	if (newsockfd > max_fd) 
                        	{    // keep track of the max
                            	max_fd = newsockfd;
                        	}
                        	strcpy(ipc_temp,inet_ntop(cl_address.ss_family,get_in_addr((struct sockaddr*)&cl_address),s, INET6_ADDRSTRLEN));
                        	printf("\nThere is a new Client connection from the client %s on the socket %d\n",inet_ntop(cl_address.ss_family,get_in_addr((struct sockaddr*)&cl_address),s, INET6_ADDRSTRLEN),newsockfd);
                        	
                        	
							}
						}

						



				
			
		

		//else

			//Keyboard Input

			else if(i==0)
			{
			int v=0;
			char rip1[40]="",rport1[40]="";
			char ipp[30];
			int idd;
			int u=0,j=0;
			fgets(buff,sizeof(buff),stdin);
			len = strlen(buff) - 1;
     		if (buff[len] == '\n')
            buff[len] = '\0';
        	for(v=0;buff[v]!=' ';v++)
        	buff[v]=toupper(buff[v]);
        	


        	for(j=0;j<strlen(buff);j++)
        	{
        		if(buff[j]==' ')
        			u++;
        		

        	}
        	        		
        	//Checking for parameters with function

        	//One parameter
        	if(u==1)
        		{

        			token=strtok(buff," ");
        			strcpy(buff,token);
        			token=strtok(NULL," ");
        			strcpy(ipp,token);
        			idd=atoi(ipp);
        			//printf("\nIDD : %d\n",idd);

        		}
        	//Two Parameters
        	else if(u==2)
        		{

        			token=strtok(buff," ");
        			strcpy(buff,token);
        			token=strtok(NULL," ");
        			strcpy(rip1,token);		
        			token=strtok(NULL," ");
        			strcpy(rport1,token);
        			

        		}

            
         	if(strcmp("HELP",buff)==0)
         	{	
         		help();
         		continue;
         	}



         	else if(strcmp("CREATOR",buff)==0)
         		{
         		creator();
         		continue;
         		}



         	else if(strcmp("DISPLAY",buff)==0)
         	{
         		display(port_no);
         		continue;
         		
         	}	
         	

         	//Send SERVER IP and Port as parameters
         	else if(strcmp("REGISTER",buff)==0)
         	{
         		char rip[50],rport[50],message[100];
         		//fflush(stdout);
         		
         		if(u!=2)
				{	printf("\nInvalid number of arguments. Kindly Enter the Server IP and Port to connect to.\n");

				}
					else
					{
		         		fflush(stdout);

		         	
		         		if(count_reg!=0)
		         		{
		         			printf("\nThis Client is already registered with the server\n");
		         			//fflush(stdout);
		         			//break;
		         		}
		         		else
		         		{
		         			
			         		memset(&hints,0,sizeof hints);
							
							hints.ai_family=AF_UNSPEC;
							hints.ai_socktype=SOCK_STREAM;
							hints.ai_flags=AI_PASSIVE; 

							rev=getaddrinfo(rip1,rport1,&hints,&info_server);

							if(rev!=0)
								printf("\nError!\n");

							else
							{

								csock_fd=socket(info_server->ai_family,info_server->ai_socktype,info_server->ai_protocol);
								
								if(csock_fd<0)
									printf("\nError while creating Socket with server!\n");

			         			//Connect with Server
			         			if((connect(csock_fd,info_server->ai_addr,info_server->ai_addrlen))==-1)
				                  	printf("\nError while connecting to Server!\n");
				         		
				                else
				                {
				                	FD_SET(csock_fd,&fds);
				                	if(csock_fd>max_fd)
				                		max_fd=csock_fd;
				                
				                	count_reg=1;



				                	ac[0].id=1;
				                	strcpy(ac[0].port_no,rport1);
				                	strcpy(ac[0].hostname,"timberlake.cse.buffalo.edu");
				                	strcpy(ac[0].ip_address,rip1);
				                	ac[0].sock_no=csock_fd;
				                	z++;
				                	//Send Port and Host details to Server for registering to Server IP List

				                	strcpy(message,"Port ");
				                	strcat(message,port_no);
				                	
				                	strcat(message," Host ");
				                	strcat(message,host_name);
				                	
				                	n=write(csock_fd,message,500);
				                	
				                	if(n==-1)
				                		printf("\nMessage not sent!\n");
				                	
				                	//break;
				                }
				                
	                		}
	            		}

	            		bzero(message,sizeof message);
                	}
                	
            	}

            
            //Connect between peers
            else if(strcmp("CONNECT",buff)==0)
			{

				char messagesend[501];
				

				//Two parameters needed
				if(u!=2)
				{	
					printf("\nInvalid number of arguments. Kindly Enter the Client IP and Port to connect to.\n");
				}
				else
				{	
						
					char cln[30],clp[30];
					int ispresent=0;
					int isconnect=0;

					//To check if the two clients are already connected
					for(r=0;r<z;r++)
					{
						if(strcasecmp(ac[r].hostname,rip1)==0||strcmp(ac[r].ip_address,rip1)==0)
						{
							
							isconnect=1;
							break;
						}
					}

					//Register before you Connect

					if(count_reg==0)
					{
							printf("\nClient is not yet registered to the Server!\n");
							continue;
					}

					//No more than three peers
					else if(z>=4)
					{
						printf("\nNumber of maximum peers exceeded\n");
						
					}

					//No Self Connections

					else if(strcmp(rip1,sip)==0||strcasecmp(rip1,host_name)==0)
					{
						printf("\nSelf connections are not allowed!\n");
						continue;
					}



					else if(isconnect==1)
					{
						printf("\nYou are already connected to this client! No duplicates allowed!\n");
						continue;
						
					}

					//Cannot Connect to Timberlake

					else if(strcasecmp(rip1,"timberlake.cse.buffalo.edu")==0||strcmp(rip1,"128.205.36.8")==0)
					{
						printf("\nYou cannot connect to the Main Server Timberlake!\n");
						continue;
					}

				

					else
					{
					fflush(stdout);
					memset(&hints,0,sizeof hints);
	                hints.ai_family=AF_UNSPEC;
	                hints.ai_socktype=SOCK_STREAM;
	                hints.ai_flags=AI_PASSIVE; 


	                rev=getaddrinfo(rip1,rport1,&hints,&info_peer);

	               	if(rev!=0)
	                {
	                    fprintf(stderr,"getaddrinfo:%s\n", gai_strerror(rev));
	    				return 1;
	                }
	                conn_sock=socket(info_peer->ai_family,info_peer->ai_socktype,info_peer->ai_protocol);
	                if(conn_sock<0)
	                		error("\nCould not create socket!\n");

	                if((connect(conn_sock,info_peer->ai_addr,info_peer->ai_addrlen))==-1)
	                {
	                    printf("\nConnect error with client!\n");
	                    continue;
	                }

	                FD_SET(conn_sock,&fds);
	                if(conn_sock>max_fd)
	                	max_fd=conn_sock;

	                printf("\nClient just got connected!\n");


	                //Storing the details of the client connected to in Structure(Peer LIST)

	                strcpy(ac[z].port_no,rport1);
	                
	                ac[z].id=z+1;
	                
	                ac[z].sock_no=conn_sock;
	                

	                //Send a Message with port and host to the peer connected to
	                strcpy(messagesend,"Porty ");
	                strcat(messagesend,port_no);
	                strcat(messagesend," Host ");
	                
	                strcat(messagesend,host_name);
	                
	                n=send(conn_sock,messagesend,strlen(messagesend),0);
	                if(n==-1)
	                	error("\nSend error\n");
	               	

	            }

	            }

	            bzero(messagesend,sizeof messagesend);
	            
	                
	        }


	        //List out Active Peer Connections

	        else if(strcmp("LIST",buff)==0)
	        {
	        	printf("\nList of Active Peer Connections\n");
	        	printf("\nID\tHost Name\t\t\t IP \t Port Number\t Socket\n");
	                for(r=0;r<z;r++)
	           		{
            			printf("\n%d\t%s\t%s\t%s\t%d\n",ac[r].id,ac[r].hostname,ac[r].ip_address,ac[r].port_no,ac[r].sock_no);
               		}
	        }

	        //Client Quits the process itself

	        else if(strcmp("QUIT",buff)==0)
         	{
         		exit(1);
         	}

         	else if(strcmp("GET",buff)==0)
         	{
         		int getid;
				char fname[30];
				getid=atoi(rip1);
				strcpy(fname,rport1);
         		if(u!=2)
				{	
					printf("\nInvalid number of arguments. Kindly Enter the Connection ID and File Name!\n");
				}
				else if(count_reg==0)
					printf("\nUnregistered client!\n");
				else if((getid==1)&&(strcasecmp(ac[1].hostname,"timberlake.cse.buffalo.edu")==0))
						printf("\nYou cannot GET a file from Timberlake(Servfer)!\n");
				else if(getid>z+1)
					printf("\nThere exists no connection with this ID!\n");
				else
				{
					strcpy(message,"Get ");
					strcat(message,fname);
					n=send(ac[getid-1].sock_no,message,sizeof message,0);
					if(n==-1)
						printf("\nSend failure!\n");
				}

         	}


         	
         	//check
	        else if(strcmp("TERMINATE",buff)==0)
         	{
         		int idpresent=0,sno;

         		if(u!=1)
         			printf("\nEnter the connection ID of the connection you want to delete!\n");

         		else if(count_reg==0)
         			printf("\nClient is not registered!\n");
         		else
         		{	
         			
         		for(r=0;r<z;r++)
                {
                   	if(ac[r].id==idd)
	                {
	                	if(strcasecmp(ac[r].hostname,"timberlake.cse.buffalo.edu")==0)
	                		printf("\nCannot terminate connection with Main Server Timberlake!\n");
	                	else
	                	{
	                  	sno=ac[r].sock_no;
	                  	idpresent=1;
	          			for(y=r;y<z;y++)
	              		{
	                  					
	          				ac[y].sock_no=ac[y+1].sock_no;
                   			strcpy(ac[y].ip_address,ac[y+1].ip_address);
	          				strcpy(ac[y].hostname,ac[y+1].hostname);
	   						strcpy(ac[y].port_no,ac[y+1].port_no);
       						
       	       			}
       	       			}
	              	}
	           	}

         		if(idpresent==0)
         			continue;
         		else
         		{
         			z--;
         			close(sno);
         			FD_CLR(sno,&fds);
	            }

	            printf("\nNew List of Active Connections\n");
	            printf("\nID\tHost Name\t\t IP Address\t Port Number\t Socket\n");
	                    		for(r=0;r<z;r++)
	                    		{
	                    			printf("\n%d %s \t%s \t\t%s \t%d\n",ac[r].id,ac[r].hostname,ac[r].ip_address,ac[r].port_no,ac[r].sock_no);
	                    		} 					
         		}

         		
         	}










        }
        
            else  // Connection with other clients as server 
            	//for client
			{
				if ((n = recv(i, msg2, sizeof msg2, 0)) <= 0) 

				{
                        // Peer Quits or shuts
                        if (n== 0)
                        {
                            // connection closed
                            printf("\nPeer connection on the socket %d hung up or Quit\n", i);

                            //Update Peer List Structure

                            for(r=0;r<z;r++)
                            {
                            	if(l[r].sock_no==i)
	                    		{
	                    					
	                    					
	                    			for(y=r;y<z;y++)
	              					{
	                  					
	              						l[y].sock_no=l[y+1].sock_no;
                   						strcpy(l[y].ip_address,l[y+1].ip_address);
	               						strcpy(l[y].hostname,l[y+1].hostname);
	               						strcpy(l[y].port_no,l[y+1].port_no);
                 						
	                    			}


	                    		}
	                    	}


	                    	//New Connection list being printed

                            printf("\nYour new active connetion list is : \n");
                            printf("\nID\tHost Name\t\t IP Address\t Port Number\t Socket\n");

                            for(r=0;r<z-1;r++)
                            {

	                    		printf("\n%d %s \t%s \t\t%s \t%d\n",ac[r].id,ac[r].hostname,ac[r].ip_address,ac[r].port_no,ac[r].sock_no);
	                    			
                            }


                        }

                        else 
                        {
                            perror("\nError while receiving!\n");
                        }

                        //CLose the peer connection that quit
                        close(i); 
                        FD_CLR(i, &fds); // remove from master set
                        z--;
                        
                      }

                      //Checking for various different types of messages

                      else
                      {

                      	char temp_message[500];
                      	strcpy(temp_message,msg2);
                      	token=strtok(temp_message," ");
                      	
                      	if(strcmp(token,"List")==0)
                      	{
                      		printf("\n%s\n",msg2); 
                      		bzero(msg2,sizeof msg2);
                      	}

                      	//Store receiving client details and send own Host

                      	else if(strcmp(token,"Porty")==0)
                      	{
                      		
                      		ac[z].id=z+1;
                      		
                      		ac[z].sock_no=newsockfd;
                      		
                      		strcpy(ac[z].ip_address,ipc_temp);
                      		
                      		token=strtok(NULL," ");
                      		
                      		strcpy(ac[z].port_no,token);
                      		
                      		token=strtok(NULL," ");
                      		token=strtok(NULL," ");
                      		strcpy(ac[z].hostname,token);
                      		
                      		z++;
 	                  		printf("\nID\tHost Name\t\t IP Address\t Port Number\t Socket\n");
	                    	for(r=0;r<z;r++)
	                    	{
	                    		
	                    		printf("\n%d \t%s \t%s \t%s \t%d\n",ac[r].id,ac[r].hostname,ac[r].ip_address,ac[r].port_no,ac[r].sock_no);
	                    	}
	                    	bzero(msg2,sizeof msg2);
	                    	bzero(message,sizeof message);
	                    	
	                    	strcpy(message,"Host ");
	                    	strcat(message,host_name);
	                    	strcat(message," IP ");
	                    	strcat(message,sip);
	                    	n=send(i,message,500,0);
	                		if(n==-1)
	                			error("\nSend error\n");
	                		bzero(message,sizeof message);

	                		

                      	}

                      	//Receive Host Details from Client connected to

                      	else if(strcmp(token,"Host")==0)
							{ 
								token=strtok(msg2," ");
								token=strtok(NULL," ");
	                    		strcpy(ac[z].hostname,token);
	                    		token=strtok(NULL," ");
	                    		token=strtok(NULL," ");
	                    		strcpy(ac[z].ip_address,token);
	                    		
	                    		z++;
	                    		
	                    		
	                    		printf("\nID\tHost Name\t\t IP Address\t Port Number\t Socket\n");
	                    		for(r=0;r<z;r++)
	                    		{
	                    			printf("\n%d \t%s \t%s \t%s \t%d\n",ac[r].id,ac[r].hostname,ac[r].ip_address,ac[r].port_no,ac[r].sock_no);
	                    		}
	                    		bzero(msg2,sizeof msg2);


                      		}
                      			// Receive Error or Connection List or Server Shutdown

                      		else if(strcmp(token,"Get")==0)
                      		{	
                      			
                      			token=strtok(NULL," ");
                      			strcpy(tempfile,token);
                      			f=fopen(tempfile,"r");
                      			if(f == NULL) 
                      			{
                         			printf("\nERROR: File %s not found.\n", tempfile);
                         			continue;
                         		}
                        		strcpy(msg1,"FILE ");
                        		strcat(msg1,tempfile);
                        		if (send(i,msg1, sizeof msg1, 0) == -1)
                            	perror("\nSend error encountered!\n");
                        		memset(sdbuf,0, LENGTH);
                        		//
                        		while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, f)) > 0)
                         		{
                         		if(send(i, sdbuf, fs_block_sz, 0) < 0) {
                          		printf("\nERROR: Failed to send file %s. (errno = %d)\n", tempfile, errno);
                          		continue;
                          		}
                         		memset(sdbuf,0, LENGTH);
                         		} //while
                        		fclose(f);
                       		

                      		}

                      		else if (strcmp(token,"FILE")==0 && fstart == 0)
                       		{
                       			fstart = 1;
                       			//strcpy(getfile,args[1]);
                       			strcpy(tempfile,"sample.txt");
                       			f = fopen(tempfile, "w");

                       			if(f== NULL)
                       			{
                         			printf("File %s Cannot be opened for write\n", tempfile);
                         			continue;
                         		}
                       			//printf("\nStart %s at %s\n",tempfile,ctime(&now));
                       		}




					}
				}		
			}
		}
	}
}
	


             


	
//Main Function to see if Client is Running or Server is running

int main(int argc, char *argv[])
{	
	fflush(stdout);
	char hn[40];
	//printf("\n %s",argv[0]);
	if(strcasecmp(argv[1],"s")==0)
	{	
		server(argv[2]);
	}	
	else if(strcasecmp(argv[1],"c")==0)
	{
		
		//strcpy(hn,argv[2]);
		client((argv[2]));
	}
}

                    
	


