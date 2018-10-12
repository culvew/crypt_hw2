//hw2
#include <math.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <time.h> 
#include <set>
	
//GLOBAL S boxes
	int S1[4][4]={{1,0,3,2},{3,2,1,0},{0,2,1,3},{3,1,3,2}};
	int S2[4][4]={{0,1,2,3},{2,0,1,3},{3,0,1,0},{2,1,0,3}};

//diffy helman stuff
	std::vector<int> divis_2(int num){
		// std::cout<<"IN DIVIS_2"<<std::endl;
		std::vector<int> ret;
		int r=0;
		while (num%2==0){
			// std::cout<<num<<std::endl;
			r++;
			num=num/2;

		}
		// std::cout<<std::endl;
		ret.push_back(r);
		ret.push_back(num);
		return ret;
	}
	int mod_exp(int a, int e, int n){
		int c=a;
		int e2=e;
		while(e2>1){
			// std::cout<<e2<<std::endl;
			if(e2%2==0){
				c=(c*c)% n;
				e2=e2/2;
			}
			else{
				return (c*mod_exp(c,e2-1,n))%n;
			}
		}
		return c;
	}
	bool check_prime(int n){
		//2,7,61 test these 3.
		// std::cout<<"IN CHECK PRIME"<<std::endl;
		std::vector<int> wit={2,3,5,7,31,61,73};
		std::vector<int> rd=divis_2(n-1);
		int r=rd[0];
		int d=rd[1];
		for (int i = 0; i < wit.size(); ++i){
			// int a=rand()%(n-4)+2;
			int a=wit[i];
			int x=mod_exp(a,d,n);
			if(x==1 || x==(n-1)){
				continue;
			}
			bool good=false;
			for (int j = 0; j < r; ++j)	{
				x=mod_exp(x,2,n);
				if (x==n-1){
					good=true;
				}
			}
			if (good){
				continue;
			}
			
			return false;
		}
		return true;
	}
	int generate_prime(){
		int a= rand();
		// std::cout<<"HERE"<<std::endl;
		while(!check_prime(a)){
			// std::cout<<"CHECKING "<<a<<" FOR PRIME"<<std::endl;
			a=rand();
		}
		return a;
	}
	int find_prime_root(int prime){
		std::vector<int> factors;
		int n=prime-1;
		int check=2;
		//get all factors.
		while(!check_prime(n)){
			if(n%check==0){
				factors.push_back(check);
				n=n/check;
			}
			else{
				check++;
			}
		}
		std::vector<int> factors2;
		factors2.push_back(factors[0]);

		for (int i = 0; i < factors.size(); ++i){

			if(factors[i]!=factors2[factors2.size()-1]){
				factors2.push_back(factors[i]);
			}
		}




		for (int i=0; i<factors2.size(); ++i){
			factors2[i]=(prime-1)/factors2[i];
		}
		int i=2;
		while(i<prime){
			bool is_root=true;
			int j=0;
			for (int j = 0; j < factors2.size(); ++j){
				int a=mod_exp(i,factors2[j],prime);
				if(a==1){
					is_root=false;
					break;
				}
			}
			if(is_root){
				return i;
			}
			i++;
		}
	}


//DES STUFF
	int vector_to_int(std::vector<int> vec){
		int val=0;
		for (int i = 0; i < 8; ++i){
			val+=pow(2,7-i)*vec[i];
		}
		return (char)  val;
	}
	std::vector<int> int_to_vector(){}
	std::vector<int> make_key(int num){
		std::vector<int>fin(10);
		for (int i = 0; i < 10; ++i){
			fin[i]=num%2;
			num/=2;
		}
		return fin;
	}
	std::vector<int> char_to_vector(char c){
		std::vector<int> out(8);
		for (int i = 7; i >= 0; --i){
	        out[i]=c&1;
	        c=c>>1;
	    }
	   return out;
	}
	char vector_to_char(std::vector<int> vec){
		// 0,1,0,0,1,0,0,0

		int val=0;
		for (int i = 0; i < 8; ++i){
			val+=pow(2,7-i)*vec[i];
		}
		return (char)  val;
	}
	std::vector<std::vector<int>> string_to_matrix(std::string str){
		std::vector<std::vector<int>>out;


		for (int i = 0; i < str.length(); ++i){
			out.push_back(char_to_vector(str[i]));
		}

		return out;
	}
	//THE DIFFERENT PERMUTATIONS
	void ExP(std::vector<int>&in){
		in.push_back(in[1]);
		in.push_back(in[2]);
		in.push_back(in[3]);
		in.push_back(in[0]);

		in[0]=in[6];
		in[1]=in[7];
		in[2]=in[4];
		in[3]=in[5];
	}
	void P10(std::vector<int>&in){
		//2 4 1 6 3 9 0 8 7 5
		//0 1 2 3 4 5 6 7 8 9
		int temp=0;
		temp=in[0];
		in[0]=in[2];
		in[2]=in[1];
		in[1]=in[4];
		in[4]=in[3];
		in[3]=in[6];
		in[6]=temp;
		//5,7,8,9
		temp=in[5];
		in[5]=in[9];
		in[9]=temp;
		temp=in[7];
		in[7]=in[8];
		in[8]=in[7];
	}
	void P8(std::vector<int>&in){
		//5 2 6 3 7 4 9 8
		//0 1 2 3 4 5 6 7 8 9
		//NO 0 or 1
		int temp=0;
		in[0]=in[5];
		in[5]=in[4];
		in[4]=in[7];
		in[7]=in[8];
		in[1]=in[2];
		in[2]=in[6];
		in[6]=in[9];

		in.pop_back();
		in.pop_back();
	}
	void IP(std::vector<int>& in){
		//15203746
		//01234567
		int temp=0;

		//swap all the spots around in a set order

		temp=in[0];
		in[0]=in[1];
		in[1]=in[5];
		in[5]=in[7];
		in[7]=in[6];
		in[6]=in[4];
		in[4]=in[3];
		in[3]=temp;
	}
	void iIP(std::vector<int>& in){
		//30246175
		//01234567
		int temp=0;
		//swap all the spots back to where they were.
		temp=in[0];
		in[0]=in[3];
		in[3]=in[4];
		in[4]=in[6];
		in[6]=in[7];
		in[7]=in[5];
		in[5]=in[1];
		in[1]=temp;
	}
	void left_shift(std::vector<int>&in){
		int temp=in[0];
		for (int i = 0; i < in.size()-1; ++i){
			in[i]=in[i+1];
		}
		in[in.size()]=temp;
	}
	std::vector<int> convert_Sbox(std::vector<int>input, int S[][4]){
		int r=input[0]*2+input[3];
		int c=input[1]*2+input[2];

		int v= S[r][c];

		std::vector<int> out(2);
		out[0]=v%2;
		v/=2;
		out[1]=v%2;

		return out;
	}
	std::vector<int> Key_gen(std::vector<int> key,int k1_k2){
		//p10 of 10 bit key
		P10(key);
		//break into 2 5 bit pieces
		std::vector<int> left(5);
		std::vector<int> right(5);
		for (int i = 0; i < 5; ++i){
			left[i]=key[i];
			right[i]=key[i+5];
		}

		//Left shift
		left_shift(left);
		left_shift(right);

		//recombine for key 1.
		std::vector<int>k1(10);
		for (int i = 0; i < 5; ++i){		
			k1[i]=left[i];
			k1[i+5]=right[i];
		}

		//send into P8 for k1
		P8(k1);


		//left shift again and send into p8 for k2
		left_shift(left);
		left_shift(right);

		//recombine for key 1.
		std::vector<int>k2(10);
		for (int i = 0; i < 5; ++i){		
			k2[i]=left[i];
			k2[i+5]=right[i];
		}
		P8(k2);

		if(k1_k2==1){
			return k1;
		}
		else return k2;
	}
	std::vector<int> F_box(std::vector<int> input, std::vector<int> key, int S1[][4], int S2[][4]){

		//starts as 4 bit

		//expansion permutation to make into 8 bit
		ExP(input);
		//xor with 8 bit key
		std::vector<int> xor1(8);
		for (int i = 0; i < 8; ++i){
			if(input[i]==key[i]){
				xor1[i]=0;
			}
			else
				xor1[i]=1;
		}
		//break into 4 bit and 4 bit
		std::vector<int> left(4);
		std::vector<int> right(4);
		for (int i = 0; i < 4; ++i){
			left[i]=xor1[i];
			right[i]=xor1[i+4];
		}

		//send into S-box1 and S-box2
		std::vector<int> out1=convert_Sbox(left,S1);
		std::vector<int> out2=convert_Sbox(right,S2);

		//combine these 2 bit outputs using P4
		std::vector<int>final={left[1],right[1],left[0],right[0]};

		//return the 4 bit solution	
		return final;
	}
	std::vector<int> encrypt(std::vector<int> PT,std::vector<int> key,int S1[][4], int S2[][4],bool encrypt){
		std::vector<int>K1;
		std::vector<int>K2;

		if(encrypt==true){
			K1=Key_gen(key,1);
			K2=Key_gen(key,2);
		}
		else{
			K1=Key_gen(key,2);
			K2=Key_gen(key,1);
		}
		IP(PT);
		std::vector<int> Left(4);
		std::vector<int> Right(4);
		//split them
		for (int i = 0; i < 4; ++i){
			Left[i]=PT[i];
			Right[i]=PT[i+4];
		}

		std::vector<int> out1=F_box(Right,K1,S1,S2);
		std::vector<int>xor1(4);
		//xor the 2
		for (int i = 0; i < 4; ++i){
			if(Left[i]==out1[i]){
				xor1[i]=0;
			}
			else
				xor1[i]=1;
		}
		
		std::vector<int> out2=F_box(xor1,K2,S1,S2);
		std::vector<int>xor2(4);
		//second xor
		for (int i = 0; i < 4; ++i){
			if(Right[i]==out2[i]){
				xor2[i]=0;
			}
			else
				xor2[i]=1;
		}

		//recombine to 8 bit
		std::vector<int>CT(8);
		for (int i = 0; i < 4; ++i){
			CT[i]=xor2[i];
			CT[i+4]=xor1[i];
		}
		//run through iIP
		iIP(CT);
		return CT;
	}

	void plain_to_crypt(char *pt, bool ptc, std::vector<int> key, char*ct){	//or visa versa
		if(!ptc){
			std::string ET_s(pt);
			std::vector<std::vector<int>> ET_m=string_to_matrix(ET_s);
			std::vector<std::vector<int>> DT_m;
			for (int i = 0; i < ET_m.size(); ++i){
				ct[i]=vector_to_char(encrypt(ET_m[i],key,S1,S2,false));
			}
		}
		
	}
	std::vector<int> int_to_key(int private_key,int public_key,int prime){
		std::vector<int> KEY;
		int shared_key=mod_exp(public_key,private_key,prime);
		for (int i = 0; i < 10; ++i){
			KEY.push_back(shared_key%2);
			shared_key/=2;
		}
		return KEY;
	}
	void int_to_chars(int num, char array[]){
		for (int i = 0; i < 4; ++i){
			array[i]=num <<(8*i);
		}
	}