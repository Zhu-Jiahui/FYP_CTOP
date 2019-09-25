//
//  main.cpp
//  FYP-CTOP
//
//  Created by Jiahui Zhu on 18/9/19.
//  Copyright © 2019 Jiahui Zhu. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <ctime>
#include <cmath>

using namespace std;

//Instance Variables
struct coordinate {
    int x;
    int y;
};

struct customer {
    int x;
    int y;
    int demand;
    int serviceTime;
    double profit;
};

int MAXVEHICLES;
int MAXCAPACITY;
int MAXTIME;
int TOTALCUSTOMERS;

double Tmax = 100.0;
double T = 0.0;
int Imax = 10;
double highestProfit = 0.00;
int N_nonImproving = 5;
double Apha = 0.5;

coordinate DEPOT;


customer extractIntegerWords(string str)
{
    stringstream ss;
    
    /* Storing the whole string into string stream */
    ss << str;
    
    /* Running loop till the end of the stream */
    string temp;
    double found;
    customer tempCustomer;
    
    while (!ss.eof()) {
        
        /* extracting word by word from stream */
        ss >> temp;
        /* Checking the given word is integer or not */
        if (stringstream(temp) >> found) {
            tempCustomer.x = found;
        }
        ss >> temp;
        if (stringstream(temp) >> found) {
            tempCustomer.y = found;
        }
        ss >> temp;
        if (stringstream(temp) >> found) {
            tempCustomer.demand = found;
        }
        ss >> temp;
        if (stringstream(temp) >> found) {
            tempCustomer.serviceTime = found;
        }
        ss >> temp;
        if (stringstream(temp) >> found) {
            tempCustomer.profit = found;
        }
        /* To save from space at the end of string */
        temp = "";
    }
    return tempCustomer;
}

void extractInstanceInfo() {
    fstream inFile;
    string line;
    
    inFile.open("/Users/jiahuizhu/Desktop/SEM7/FYP/Benchmark\ Instances/DatasetsCTOP/1set/b1.txt");
    if (inFile.is_open())
    {
        while ( getline(inFile,line))
        {
            if (line.find("MAXVEHICLES") != string::npos){
                stringstream temp(line.substr(12));
                temp >> MAXVEHICLES;
            }
            
            if (line.find("MAXCAPACITY") != string::npos){
                stringstream temp(line.substr(12));
                temp >> MAXCAPACITY;
            }
            
            if (line.find("MAXTIME") != string::npos){
                stringstream temp(line.substr(8));
                temp >> MAXTIME;
            }
            
            if (line.find("CUSTOMERS") != string::npos){
                stringstream temp(line.substr(10));
                temp >> TOTALCUSTOMERS;
            }
            
            if (line.find("DEPOT") != string::npos){
                stringstream temp(line.substr(6,8));
                temp >> DEPOT.x;
                stringstream temp1(line.substr(9));
                temp1 >> DEPOT.y;
            }
        }
        inFile.close();
    }
}

void getCustomers(customer CUSTOMERS[]) {
    fstream inFile;
    string line;
    int numCheck = 0;
    int count = 0;
    
    inFile.open("/Users/jiahuizhu/Desktop/SEM7/FYP/Benchmark\ Instances/DatasetsCTOP/1set/b1.txt");
    if (inFile.is_open())
    {
        while ( getline(inFile,line))
        {
            numCheck = 0;
            stringstream temp (line);
            temp >> numCheck;
            if (numCheck != 0) {
                CUSTOMERS[count] = extractIntegerWords(line);
                count++;
            }
        }
        inFile.close();
    }
};


void showTheContent(list<int> l)
{
     list<int>::iterator it;
      for(it=l.begin();it!=l.end();it++)
   {
       cout <<  *it << " ";
   }
   cout << "\n";
}

double getTotalProfit(customer CUSTOMERS[], list<int> vehicles[]) {

    list<int> :: iterator iter;
    int i = 0;
    double totalProfit = 0.0;
    
    while (vehicles[i].empty() != true && i < MAXVEHICLES) {
        iter = vehicles[i].begin();
        //cout << "vehicle " << i << "Size is " << vehicles[i].size() << '\n';
        for (int j = 0; j < vehicles[i].size(); j++){
            totalProfit = totalProfit + CUSTOMERS[*iter].profit;
            //cout << "current profit = " << CUSTOMERS[*iter].profit << '\n';
            advance(iter, 1);
        }
        i++;
    }
    cout << "total profit = " << totalProfit << '\n';
    return totalProfit;
}

int getVehicleCapacity(customer CUSTOMERS[], list<int> vehicle){ //returns the total capacity of a vehicle
    
    int capacity = 0;
    list<int>:: iterator iter;
    iter = vehicle.begin();
    int i = 0;
    
    for (i = 0; i < vehicle.size(); i++) {
        capacity = capacity + CUSTOMERS[*iter].demand;
        //cout << "customer no." << *iter << " has demand of " << CUSTOMERS[*iter].demand << '\n';
        advance(iter, 1);
    }
    cout << "vehicle's capacity is " << capacity << '\n';
    return capacity;
}

double getEuclideanDistance(coordinate currentCoordinate, coordinate nextCoordinate){ //returns the distance between two nodes
    double distance = 0.0;
    
    distance = sqrt(pow(nextCoordinate.x-currentCoordinate.x, 2.0) + pow(nextCoordinate.y-currentCoordinate.y, 2.0));
    
    cout << "Distance between coordinate (" <<currentCoordinate.x << "," << currentCoordinate.y << ") and (" << nextCoordinate.x << "," << nextCoordinate.y << ") is " << distance << '\n';
    
    return distance;
}

double getVehicleTimeTaken(customer CUSTOMERS[], list<int> vehicle){ //returns the total distance a vehicle covers.
    
    //assuming time taken to travel 1 unit of location is also 1 unit time.
    int i = 0;
    double timeTaken = 0;
    list<int>:: iterator iter;
    coordinate currentCoordinate;
    coordinate nextCoordinate;
    
    iter = vehicle.begin();
    
    currentCoordinate.x = DEPOT.x;
    currentCoordinate.y = DEPOT.y;
    
    nextCoordinate.x = CUSTOMERS[*iter].x;
    nextCoordinate.y = CUSTOMERS[*iter].y;
    //calculate distance from depot to first customer
    timeTaken = timeTaken + getEuclideanDistance(currentCoordinate, nextCoordinate);
    for (i = 0; i < vehicle.size()-1; i++) {
        advance(iter, 1);
        currentCoordinate = nextCoordinate;
        nextCoordinate.x = CUSTOMERS[*iter].x;
        nextCoordinate.y = CUSTOMERS[*iter].y;
        timeTaken = timeTaken + CUSTOMERS[*iter].serviceTime + getEuclideanDistance(currentCoordinate, nextCoordinate); //service time plus travel time.
    }
    currentCoordinate = nextCoordinate;
    nextCoordinate.x = DEPOT.x;
    nextCoordinate.y = DEPOT.y;
    timeTaken = timeTaken + getEuclideanDistance(currentCoordinate, nextCoordinate);
    
    cout << "vehicle's total Time Taken is " << timeTaken << '\n';
    return timeTaken;
}

void initialSolution(customer CUSTOMERS[], list<int>* unassignCustomersList, list<int>* assignedCustomersList, list<int> vehicles[]) {
    
    int customer = 0;
    int i = 0;
    int vehicleNo = 0;
    bool vehicleFilled = false;
    //srand(time(NULL)); //set random seeds with time.
    list<int>::iterator iter;
    list<int> tempVehicle;
    
    //initialise the tracker array with from 0 to TOTALCUSTOMERS-1.
    for (i = 0; i < TOTALCUSTOMERS; i++){
        unassignCustomersList->push_back(i);
    }
    while (unassignCustomersList->empty() == false && vehicleNo < MAXVEHICLES) {
        cout << "Processing Vehicle no. " << vehicleNo << '\n';
        //cout << "unassignCustomerList is = ";
        //showTheContent(*unassignCustomersList);
            
        int randomCustomerIndex = rand() % unassignCustomersList->size(); //randomly generates a number from the list
        //cout << "customer index is " << randomCustomerIndex << '\n';
        iter = unassignCustomersList->begin(); //set iterator to the start.
        advance(iter, randomCustomerIndex); //move iterator to the random number generated.
        customer = *iter; //get the number that iterator is pointing to.
        //cout << "Random Number is = " << customer << '\n';
        //cout << "Customer Demand is = " << CUSTOMERS[customer].demand << '\n';
        tempVehicle.push_back(customer);
        if (getVehicleCapacity(CUSTOMERS, tempVehicle) <= MAXCAPACITY && getVehicleTimeTaken(CUSTOMERS, tempVehicle) <= MAXTIME) { //as long as adding the new demand does not exceed the car capcacity.
                
            unassignCustomersList->erase(iter);//remove the customer from the unassignCustomers list
            assignedCustomersList->push_back(customer); //add the customer into the assignedCustomers list
            vehicles[vehicleNo].push_back(customer); //add the customer into the respective vehicle list.
            //cout << "Current capacity = " << getVehicleCapacity(CUSTOMERS, vehicles[vehicleNo]) << '\n';

        } else {
            tempVehicle.clear();
            vehicleNo++;
        }
    }
    getTotalProfit(CUSTOMERS, vehicles);
    cout << "initialsolution has finished running" << '\n';
 
}

void swap(customer CUSTOMERS[], list<int>* unassignCustomersList, list<int>* assignedCustomersList, list<int> vehicles[]){
    //choose two random numbers
    //if swap does not violate constraints --> do swap
    
    bool swapSuccessful = false;
    bool capacity1Satisfied = false;
    bool capacity2Satisfied = false;
    int index1 = 0;
    int index2 = 0;
    list<int> tempVehicle;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    //cout << "entering swap function " << '\n';
    
    while (swapSuccessful == false) {
        
        //cout << "entering while loop" << '\n';
        index1 = rand()%TOTALCUSTOMERS;
        cout << "index1 is " << index1 << '\n';
        index2 = rand()%TOTALCUSTOMERS;
        cout << "index2 is " << index2 << '\n';

        
        if (index1 == index2) { //if i and j are the same customer, choose random number again
            cout << "index1==index2 detacted, next iteration" << '\n';
            continue;
        }
        if (index1 < assignedCustomersList->size()) { //index1 is in assignedCustomersList
            int i = 0;
            while (vehicles[i].size() < index1) { //check which vehicle is it in;
                index1 = index1 - vehicles[i].size();
                //cout << "current index1 is " << index1 << '\n';
                i++;
            }
            if (i > 0) {
                i = i - 1;
            }
            iter1 = vehicles[i].begin();
            advance(iter1, index1); //get customer number from the vehicle
            //cout << "The customer Chosen is " << *iter1 << '\n';
                
        } else { //index1 is in unassignCustomersList
                
            index1 = index1 - assignedCustomersList->size();
            iter1 = unassignCustomersList->begin();
            advance(iter1, index1); //get customer number from the unassignCustomerList.
            capacity1Satisfied = true; //if customer in unassignCustomersList, capacity is automatically satisfied.
            cout << "index1 is pointing to a customer in unassignCustomerList ";
            cout << "the Customer is " << *iter1 << '\n';
                
        }
            
        if (index2 < assignedCustomersList->size()) { //index2 is in assignedCustomersList
            int i = 0;
            while (vehicles[i].size() < index2) { //check which vehicle is it in;
                index2 = index2 - vehicles[i].size();
                //cout << "current index2 is " << index2 << '\n';
                i++;
            }
            if (i > 0) {
                i = i - 1;
            }
            iter2 = vehicles[i].begin();
            advance(iter2, index2); //get customer number from the vehicle
            //cout << "The customer Chosen is " << *iter2 << '\n';
                
        } else { //index2 is in unassignCustomerList
                
            index2 = index2 - assignedCustomersList->size();
            iter2 = unassignCustomersList->begin();
            advance(iter2, index2); //get customer number from the unassignCustomerList.
            capacity1Satisfied = true; //if customer in unassignCustomersList, capacity is automatically satisfied.
            cout << "index2 is pointing to a customer in unassignCustomerList";
            cout << "the Customer is " << *iter2 << '\n';
        }
        
        //check if both capacity are satisfied
        
        swapSuccessful = true;
    }
        
}
    

void insertion(){
    
    
}

void Reversion(){
    
    
}


void simulatedAnnealing(customer CUSTOMERS[], list<int>* unassignCustomersList, list<int>* assignedCustomersList, list<int> vehicles[]) {
    int I = 0;
    int N = 0;
    double p = 0.0;
    double currentProfit = 0.0;
    
    while (N < N_nonImproving) {
        for (I = 0; I < Imax; I++){
            p = (double) rand() / (RAND_MAX);
            //cout << "p is " << p << '\n';
            
            if (p <= 1.0/3.0) {
                //cout << "Choose Swap" << '\n';
                swap(CUSTOMERS, unassignCustomersList, assignedCustomersList, vehicles);
            }
            else if (p <= 2.0/3.0 && p > 1.0/3.0) {
                //cout << "Choose Insertion" << '\n';
                
            }
            else if (p <= 3.0/3.0 && p > 2.0/3.0) {
                //cout << "Choose Reversion" <<'\n';
            }
        }
        N++;
    }
}


int main() {
    
    extractInstanceInfo();
    customer CUSTOMERS[TOTALCUSTOMERS];
    getCustomers(CUSTOMERS);
    
    list<int> unassignCustomersList;
    list<int> assignedCustomersList;
    list<int> vehicles[MAXVEHICLES];
    

    initialSolution(CUSTOMERS, &unassignCustomersList, &assignedCustomersList, vehicles);
    cout << "unassignCustomerList is = ";
    showTheContent(unassignCustomersList);
    cout << "assignedCustomerList is = ";
    showTheContent(assignedCustomersList);
    for (int i = 0; i < MAXVEHICLES; i++) {
        cout << "vehicle " << i << "contains ";
        showTheContent(vehicles[i]);
        
    }
    simulatedAnnealing(CUSTOMERS, &unassignCustomersList, &assignedCustomersList, vehicles);
    
    cout << "testing github 789 " << '\n';
    
    return 0;
}



