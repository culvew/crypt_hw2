//BOB,ALICE

#include <vector>
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


int main(int argc, char *argv[]){

	//argv[1]=port, argv[2]=NAME, argv[3]=requester
	short p= atoi(argv[1]);
	char* NAME=argv[2];
	bool requester;
	if(argv[3]=="true")
		requester=true;
	else
		requester=false;

	struct sockaddr_in mysock;
	bzero(&mysock, sizeof(mysock));
	mysock.sin_family= AF_INET;
	mysock.sin_port= htons(p);
	if(inet_pton(AF_INET,argv[4],&mysock.sin_addr)<=0){
		std::cout<<"UH OH"<<std::endl;
		return -1;
	}
	int fd=socket(AF_INET,SOCK_STREAM,0);
	if(connect(fd,(struct sockaddr*) &mysock,sizeof(mysock))){
		perror("uhoh");
		return -1;
	}
//both USERS
	//USER sends KDC (NAME)
	//WRITE CALL
	write(KDC_FD,NAME,sizeof(NAME));

	//USER recieves,(prime, prim_root, Y_KDC)
	char nums[12];
	//READ CALL
	read(KDC_FD,nums,sizeof(nums));
	

	char prime_c[3];
	char prime_root_c[3];
	char Y_KDC_c[3];
	for (int i = 0; i < 4; ++i){
		prime_c[i]=nums[i];
		prime_root_c[i]=nums[i+4];
		Y_KDC_c[i]=nums[i+8];
	}
	int prime=*(int*)prime_c;
	int prim_root=*(int*)prime_root_c;
	int Y_KDC=*(int*) Y_KDC_c;


	//USER chooses X_A. 
	int X_A=rand()%prime;
	//USER sends KDC (Y_A)
	//WRITE CALL
	int Y_A=mod_exp(prim_root,X_A,prime);
	int key_num=mod_exp(Y_KDC,X_A,prime);
	std::vector<int> key_KDC=make_key(key_num);


	//USERS now have keys with KDC


//if REQUESTER
	while(1){
		char frnd_name[32];
		//IF a user cins a name(B), 
		std::cin >> frnd_name;

		time_t rawtime;
		struct tm * timeinfo;
		time (&rawtime );
		timeinfo = localtime ( &rawtime );

		char r_msg[25+32+32];
		strcpy(r_msg,NAME);
		strcat(r_msg,frnd_name);
		char TIME[25];
		strcpy(TIME,asctime(timeinfo));
		strcat(r_msg,TIME);
		//sends KDC (NAME, B, TIME)
		//WRITE CALL
		write(KDC_FD,r_msg,sizeof(r_msg));




		int l=10+32+sizeof(TIME)+32+10+sizeof(TIME);
		char ET_a[l];
		char DT_a[l];
		//USER recieves  E_KA(KEY, B, TIME, Message for B)
		//READ CALL
		read(KDC_FD,ET_a,sizeof(ET_a));

		//decrypt the Message(KEY,B,Time,MessageB)

		plain_to_crypt(ET_a,false,key_KDC,DT_a);
		std::vector<int> KEY;
		//set the key.
		for (int i = 0; i < 10; ++i){
			KEY.push_back(DT_a[i]);
		}
		for (int i = 32; i < 64; ++i){
			if(DT_a[i]!=frnd_name[i]){
				perror("ERROR MISMATCH NAMES");
			}
		}
		for (int i = 64; i < 64+sizeof(TIME); ++i){
			if(DT_a[i]!=TIME[i]){
				perror("ERROR MISMATCH TIMES");
			}
		}
		char bmsg[32+25+10];
		for (int i = 0; i < 87; ++i){
			bmsg[i]=DT_a[25+32+32+i];
		}
		//USER SENDS (Message for B) to B.
		//WRITE CALL (TO B THIS TIME!!!)
		write(B_FD,bmsg,sizeof(bmsg));

	} 
	

//if NON-REQUESTER
	//wait to recieve message from REQUESTER
	
	int l=32+25+10;
	char ET_b[l];
	char DT_b[l];
	//READ CALL

	read(KDC_FD, ET_b,sizeof(ET_b));
	//receives E_KB(REQUESTER_NAME, KEY, TIME)

	//decrypt

	plain_to_crypt(ET_b, false, key_KDC,DT_b);



	//sends E_KEY(NAME, TIME2) to REQUESTER.

	char confirm[25+32];


	time_t rawtime;
	struct tm * timeinfo;
	time (&rawtime );
	timeinfo = localtime ( &rawtime );
	strcpy(confirm,NAME);
	char TIME2[25];
	strcpy(TIME2,asctime(timeinfo));
	strcat(confirm,TIME2);
	//WRITE CALL(TO A THIS TIME!!!)
	write(A_FD,confirm,sizeof(confirm));




//they both now have the KEY and know they are talking to each other.


	return 0;
}