/*
Solution to the Sleeping Barber Problem:
https://github.com/thiagowinkler/shift-sleeping-barber-problem

by- Devesh Naik
*/

#include <iostream>
#include <thread>
#include <vector>		// for vector
#include <stdlib.h>     // srand, rand 
#include <time.h>       // time 


//Global Variables
std::vector<int> custIDs; //vector for the customers and their IDs
std::mutex mu; // mutex for protecting the shared data
std::condition_variable cond; // condition variable for managing the mutex
int M; //number of seats 
int id = 1; //customer IDs


//Waiting room print function
void printVec(){
	std::cout << "Waiting room: ";
	for(int j=0; j < custIDs.size(); j++){
		std::cout << custIDs[j] << " ";
	}
	std::cout << std::endl;
}

//function for creaing new customers every 3 seconds
void customer_creation(){
	while(true){
		std::unique_lock<std::mutex> locker(mu);
		if(custIDs.size() == M){
			//if all the seats are full
			std::cout << "Customer " << id << " leaving" <<std::endl;
			id++;
		}else{
			//if seats are available
			custIDs.push_back(id);
			id++;
			printVec();
		}
		locker.unlock();
		if(custIDs.size() == 1){
			cond.notify_one(); // notify the barber for the 1st customer after the wating room was empty
		}
		std::this_thread::sleep_for(std::chrono::seconds(3));
		
	}

}

void barber_functions(){
	while(true){
		std::unique_lock<std::mutex> locker(mu);
		if(custIDs.empty()){
			//if no customer available
			std::cout << "Barber is Sleeping" <<std::endl;
			printVec();
			cond.wait(locker); // waiting for the first customer
		}else{
			std::cout << "Barber cutting the hair of customer " << custIDs[0] <<std::endl;
			// remove the customer which is undergoing the haircut from waiting room and print waiting room
			custIDs.erase(custIDs.begin()); 
			printVec();
			int random_sleep = rand() % 5 + 1; // random sleep time: 1 to 5 seconds
			locker.unlock();
			std::this_thread::sleep_for(std::chrono::seconds(random_sleep));
		}
	}
}


int main(int argc, const char * argv[]){

	M = atoi(argv[1]);
	srand(time(NULL)); // for random number

	std::thread barber(barber_functions);
	std::this_thread::sleep_for(std::chrono::seconds(3)); //first 3 sec wait time for creating customers
	std::thread waiting_room(customer_creation);

	barber.join();
	waiting_room.join();

	return 0;
}	


