//KDC
#include <vector>
#include <time.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <string>
#include "hw2.h"
struct person{
	char* NAME;
	int FD;
	struct sockaddr_in address;
	std::vector<int> KEY;

};


int main(int argc, char const *argv[]){

	int max_clients=7;
	int max_fd=5;
	int port=7777;

	std::vector<struct person> clients;

	srand(time(NULL));
//KDC chooses Prime, prim_root, X_KDC, Y_KDC(based off previous)
	int prime=generate_prime();
	char prime_c[4];
	
	int prime_root=find_prime_root(prime);
	char prime_r_c[4];
	int_to_chars(prime_root,prime_r_c);

	int X_KDC=rand()%prime;
	int Y_KDC=mod_exp(prime_root,X_KDC,prime);
	char Y_KDC_c[4];
	int_to_chars(Y_KDC,Y_KDC_c);

//socket stuff
	//select stuff
	fd_set readfds; 
	std::vector<struct sockaddr_in> theresocks;
	struct sockaddr_in mysock;

	bzero(&mysock, sizeof(mysock));
	mysock.sin_family= AF_INET;
	mysock.sin_port= htons(port);
	mysock.sin_addr.s_addr=htonl(INADDR_ANY);
	std::vector<int> fds;
	int fd;
	int master_fd=socket(AF_INET,SOCK_STREAM,0);
	if(master_fd<=0){
		perror("SOCKET ERROR");
	}
	int e=bind(master_fd,(struct sockaddr *) &mysock,sizeof(mysock));
	if(e<0){
		perror("BIND ERROR");
		return -1;
	}
	if(listen(master_fd,10)==-1){
		perror("LISTEN ERROR");
		return -1;
	}
//end generic socket and now start talking to BOB and ALICE
	int theresize;
	int new_socket;
	while(1){
		FD_ZERO(&readfds); 
		FD_SET(master_fd, &readfds); 
		max_fd =master_fd;
		for (int i = 0; i < clients.size(); ++i){
			fd=clients[i].FD;
			if(fd>0)
				FD_SET(fd, &readfds);
			if(fd>max_fd)
				max_fd=fd;
		}
		//choose a client to accept, and figure out if they are new or not.
		//SELECT CALL
		int act=select(max_fd+1,&readfds,NULL,NULL,NULL);
		// IF STATEMENT FOR IF NEW CONNECTION 
		
		if(act<0 && errno!=EINTR){
			perror("SELECT");
		}

		//it's a new person with there name.
		if(FD_ISSET(master_fd, &readfds)){
			//ACCEPT CALL
			int q=sizeof(mysock);
			if ((new_socket = accept(master_fd,(struct sockaddr *)&mysock, (socklen_t*) &q))<0)   
            {     
                perror("accept");   
                exit(EXIT_FAILURE);   
            }



			char msg1[32];
			//READ CALL
			int new_fd;
			read(new_fd,msg1,sizeof(msg1));
			struct person p;

			p.NAME=msg1;
			p.FD=new_fd;/*return value of accept*/
 
			char msg[4+4+4];
			strcpy(msg,prime_c);
			strcat(msg,prime_r_c);
			strcat(msg,Y_KDC_c);
			//WRITE CALL
			write(new_fd,msg,sizeof(msg));


			// Wait to receive there public key. 
			//figure out shared key, and add them to list of clients.
			char Y_c[4];
			//READ CALL
			read(new_fd,Y_c,sizeof(Y_c));

			int Y= *(int*)Y_c;
			p.KEY=int_to_key(X_KDC,Y,prime);
			clients.push_back(p);
		}

		//else its a previous user
		//check eac client to see FD_ISSET
		for(int i=0; i< clients.size();++i) {
			int SD=clients[i].FD;
			if(FD_ISSET(SD,&readfds)){
				//it's a request to talk to a different user 
				
				//(client_A,client_B, Timestamp). 
				char msg[64+sizeof(time_t)];
				//READ CALL
				read(SD,msg,sizeof(msg));
				std::string client_A;
				std::string client_B;
				for (int i = 0; i < 32; ++i){
					client_A+=msg[i];
					client_B+=msg[i+32];
				}

				struct person client_A_p;

				for (int i = 0; i < clients.size(); ++i){
					if(clients[i].NAME==(client_A)){
						client_A_p=clients[i];
						break;
					}
				}


				char TIME[25];
				for (int i = 0; i < 25; ++i){
					TIME[i]=msg[64+i];
				}

				struct person current_c;
				bool found=false;
				for (int i = 0; i < clients.size(); ++i){
					if (clients[i].NAME==client_B){
						current_c=clients[i];
						found=true;
						break;
					}
				}
				//check if Client_B is in the database
				if(found){

					char new_msg[32+25+10+32];
					// generate the message for B and encrypt it
					//E_KB(User_A,KEY,TIME)
					char b_msg[32+10+25];
					char b_msg_e[32+10+25];
					//generate a random session key
					char KEY[10];
					for (int i = 0; i < 10; ++i){
						KEY[i]=rand()%2;
					}
					//msg for b.
					strcpy(b_msg,client_A_p.NAME);
					strcat(b_msg,KEY);
					strcat(b_msg,TIME);
					plain_to_crypt(b_msg,true,current_c.KEY,b_msg_e);
					//generate the message for A and encrypt it
					//[KEY, User_B, TIME, b_msg_e]
					char a_msg[sizeof(b_msg_e)+25+32+10];
					char a_msg_e[sizeof(b_msg_e)+25+32+10];
					strcpy(a_msg,KEY);
					strcat(a_msg,current_c.NAME);
					strcat(a_msg,TIME);
					strcat(a_msg,b_msg_e);

					//encrypt the message to a.
					plain_to_crypt(a_msg,true,client_A_p.KEY,a_msg_e);
					//WRITE CALL
					write(SD,a_msg,sizeof(a_msg));


				}
			}
			//DONE			
		}
	}

	return 0;
}