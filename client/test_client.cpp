/* build cmd 
g++ gen-cpp/UserStorage.cpp gen-cpp/user_profile_types.cpp gen-cpp/user_profile_constants.cpp test_client.cpp -o testclient -lthrift

using 
./test [num]
[num] - number of turns request to server

*/

#include <bits/stdc++.h>

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

int numTurn;
bool resultTest;

//protype function
bool 	parseInputParameter(int argc, char **argv);
int	getCMD();

int main(int argc, char **argv){
	boost::shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    	boost::shared_ptr<TTransport> transport(new TBufferedTransport(socket));
    	boost::shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
	UserStorageClient client(protocol);
	
	if (!parseInputParameter(argc, argv)){
		return 0;
	}
	
	resultTest = true;

	try{ 
    		transport->open();
		
		for(int i = 0; i < numTurn; ++i){
			std::cout << getCMD() << std::endl; 
		} 
				
					
		std::cout << "DONE" << std::endl;
		transport->close();
    	
	}catch (TException &tx){
      		std::cout<<"Error: " <<tx.what() <<std::endl;
    	}	
	
	return 0;
}

bool
parseInputParameter(int argc, char **argv){
	if (argc < 2){
		std::cout << "Using ./testclient [num]" << std::endl;
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
	return rand() % 3 + 1;
}