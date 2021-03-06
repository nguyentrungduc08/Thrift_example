/* build cmd 
g++ -std=c++11 gen-cpp/UserStorage.cpp gen-cpp/user_profile_types.cpp gen-cpp/user_profile_constants.cpp testMultiConnectionsNonServer.cpp -o testMultiConnectionsNonServer -lthrift -lpthread

using 
./test [num]
[num] - number of turns request to server

*/
#include <bits/stdc++.h>
#include <sys/time.h>
#include <thread>
#include <mutex>

//Boost libraries
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

//Thrift libraries
#include <thrift/protocol/TBinaryProtocol.h>             
#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TSocket.h>                    
#include <thrift/transport/TBufferTransports.h>          
#include <thrift/transport/TTransportUtils.h>

//Including the thrift generated files 
#include "gen-cpp/UserStorage.h"


//Namespaces
using namespace apache::thrift;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;
using namespace Task1;
using boost::make_shared;

typedef unsigned long long timestamp_t;

static timestamp_t
get_timestamp ()
{
	struct timeval now;
      	gettimeofday (&now, NULL);
      	return  now.tv_usec + (timestamp_t)now.tv_sec * 1000000;
}


bool 	resultTest;

//protype function
bool 			parseInputParameter(int argc, char **argv);
int				getCMD();
int 			getAge();
int				getGender();
std::string		getName();

void 		task(const std::string &name);

int 			numTurn = 10;
int				g_countOK = 0;
std::mutex 		g_lock;
#define NumCon 	1000	
// #define NumCon 	1

#define BENCKSTEP 0

int main(int argc, char **argv){
		
	// if (!parseInputParameter(argc, argv)){
	// 	return 0;
	// }
	
	std::thread myThreads[NumCon];

	timestamp_t t0 = get_timestamp();
	
	for(int i = 0; i < NumCon; ++i) {
		std::string sname = "task" + i;
		std::shared_ptr<std::string> shname = std::make_shared<std::string>(sname); 
		myThreads[i] = std::thread(task, *shname);
	}	
	
	for(int i = 0; i < NumCon; ++i){
		 myThreads[i].join();
	}
		
	timestamp_t t1 = get_timestamp();
	double secs = (t1 - t0) / 1000000.0L;		
	std::cout << "--Turn success: " << g_countOK << std::endl;
	std::cout << "--time: " << secs << std::endl;
	
	return 0;
}

void
task(const std::string &name){
	boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
	// boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
	boost::shared_ptr<TTransport> transport(new TFramedTransport(socket));
	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	UserStorageClient client(protocol);
	std::vector<int> _listID;
	try{ 
    	transport->open();	
#if BENCKSTEP
		double totalTime = 0;
#endif

		// int id = 0;
		for(int i = 0; i < numTurn; ++i){	
			int cmd = getCMD();
			// int cmd = 2;
			std::cout << name << " case: " << i << "\n";
#if BENCKSTEP			
			timestamp_t t0 = get_timestamp();
#endif
			switch(cmd) {
				case 1: {
						UserProfile profile;
						profile.name 	= getName();
						profile.age 	= getAge();
						profile.gender	= getGender();	
						int res = client.createUser(profile);
					}		
					break;	
				case 2: {
						int id = rand() % 10 + 1;
						// if (id < 500) {
						// 	++id;
						// } else {
						// 	id = 0;
						// }
						UserProfile u;
						client.getUser(u, id);
					}
					break;
			}
#if BENCKSTEP
			timestamp_t t1 = get_timestamp();
			double secs = (t1 - t0) / 1000000.0L;
			totalTime += secs;
#endif
		}

		std::cout << "DONE" << std::endl;

#if BENCKSTEP 
		std::cout << "Time: " << (double)(totalTime / numTurn) << std::endl;
#endif
		// g_lock.lock();
		// ++g_countOK;
		// g_lock.unlock();

		transport->close();
    	
	}catch (TException &tx){
      		std::cout<<"Error: " <<tx.what() <<std::endl;
    }
}

bool
parseInputParameter(int argc, char **argv){
	if (argc < 2){
		std::cout << "Using ./testclient [num]" << std::endl;
		numTurn = 15;
		return false;
	}
	
	std::string spps = std::string(argv[1]);
        std::stringstream ss(spps);
        numTurn = 0;
        ss >> numTurn;
        return true;
}

int	
getCMD(){
	return rand() % 2 + 1;
}

int 		
getAge(){
	return rand() % 100 + 1; 
}

std::string	
getName(){
	static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        
	int len = rand()%30 + 1;
	std::string name;
	for(int i = 0; i < len; ++i){
		name += alphanum[rand() % (sizeof(alphanum) - 1)];
	}
	return name;
}

int
getGender(){
	return rand() % 3;
}
