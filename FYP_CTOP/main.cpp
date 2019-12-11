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
//#include "swap.hpp"

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
    double ratio;
};

int MAXVEHICLES;
int MAXCAPACITY;
int MAXTIME;
int TOTALCUSTOMERS;

double T = 1000.0;
int Imax = 100;
int N_nonImproving = 10;
double coolingRatio = 0.5;
string path = "/Users/jiahuizhu/Desktop/SEM7/FYP/Benchmark\ Instances/DatasetsCTOP/1set/b8.txt";

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
    
    inFile.open(path);
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
    
    inFile.open(path);
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
    
    for (int i = 0; i < TOTALCUSTOMERS; i++){
        CUSTOMERS[i].ratio = CUSTOMERS[i].profit / CUSTOMERS[i].demand;
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


int getVehicleCustomerSize(list<int> vehicles[]) {
    
    int totalCustomers = 0;
    
    for (int i = 0; i < MAXVEHICLES; i++) {
        
        totalCustomers = totalCustomers + vehicles[i].size();
    }
    
    return totalCustomers;
}

double getTotalProfit(customer CUSTOMERS[], list<int> vehicles[]) {

    list<int> :: iterator iter;
    int i = 0;
    double totalProfit = 0.0;
    
    while (i < MAXVEHICLES) {
        iter = vehicles[i].begin();
        //cout << "vehicle " << i << "Size is " << vehicles[i].size() << '\n';
        for (int j = 0; j < vehicles[i].size(); j++){
            totalProfit = totalProfit + CUSTOMERS[*iter].profit;
            //cout << "current profit = " << CUSTOMERS[*iter].profit << '\n';
            advance(iter, 1);
        }
        i++;
    }
    //cout << "total profit = " << totalProfit << '\n';
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
    //cout << "vehicle's capacity is " << capacity << '\n';
    return capacity;
}

double getEuclideanDistance(coordinate currentCoordinate, coordinate nextCoordinate){ //returns the distance between two nodes
    double distance = 0.0;
    
    distance = sqrt(pow(nextCoordinate.x-currentCoordinate.x, 2.0) + pow(nextCoordinate.y-currentCoordinate.y, 2.0));
    //cout << "Distance between coordinate (" <<currentCoordinate.x << "," << currentCoordinate.y << ") and (" << nextCoordinate.x << "," << nextCoordinate.y << ") is " << distance << '\n';
    
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
    
    if (vehicle.empty() == true) {
        return 0;
    }
    
    timeTaken = timeTaken + getEuclideanDistance(currentCoordinate, nextCoordinate);
    for (i = 0; i < vehicle.size()-1; i++) {
        advance(iter, 1);
        currentCoordinate = nextCoordinate;
        nextCoordinate.x = CUSTOMERS[*iter].x;
        nextCoordinate.y = CUSTOMERS[*iter].y;
        timeTaken = timeTaken + CUSTOMERS[*iter].serviceTime + getEuclideanDistance(currentCoordinate, nextCoordinate); //service time plus travel time.
    }
    //calculate distance from last customer to depot.
    currentCoordinate = nextCoordinate;
    nextCoordinate.x = DEPOT.x;
    nextCoordinate.y = DEPOT.y;
    timeTaken = timeTaken + getEuclideanDistance(currentCoordinate, nextCoordinate);
    
    //cout << "vehicle's total Time Taken is " << timeTaken << '\n';
    return timeTaken;
}

void testTotalCustomers(customer CUSTOMERS[], list<int> vehicles[], list<int>* unassignCustomerList) {
    
    int total = 0;
    list<int> templist;
    list<int>::iterator iter;
    
    for (int i = 0; i < MAXVEHICLES; i++) {
        iter = vehicles[i].begin();
        cout << "vehicle " << i << " has capacity of " << getVehicleCapacity(CUSTOMERS, vehicles[i]) << " . and duration of " << getVehicleTimeTaken(CUSTOMERS, vehicles[i]) << " and contain customers: ";
        showTheContent(vehicles[i]);
        
        //for(int j = 0; j < vehicles[i].size(); j++) {
            //templist.push_back(*iter);
            //advance(iter, 1);
        //}
        //total = total + vehicles[i].size();
        //cout << " total is " << total << '\n';
    }
    cout << "unassign vehicle contains: ";
    showTheContent(*unassignCustomerList);
    
    //iter = unassignCustomerList->begin();
    //for (int k = 0; k < unassignCustomerList->size(); k++) {
        //templist.push_back(*iter);
        //advance(iter, 1);
    //}
    //cout << "total number of customer is: " << templist.size() << '\n';
    //templist.sort();
    //cout << "sorted customer is ";
    //showTheContent(templist);
}

void rankCustomer(customer CUSTOMERS[], list<int>* unassignCustomersList) {
    
    int tempCustomer;
    list<int> templist;
    list<int>::iterator iter;
    
    //if unassignCustomersList is empty
    if (unassignCustomersList->empty() == true) {
        //cout << "unassign Customer list is empty, exit rankCustomer!" << '\n';
        return;
    }
    
    //initialise the ranking system
    templist.push_back(*unassignCustomersList->begin());
    unassignCustomersList->pop_front();
    //cout << "the first customer " << *templist.begin() << " has ratio " << CUSTOMERS[*templist.begin()].ratio << '\n';
    while (unassignCustomersList->empty() == false) {
        
        tempCustomer = *unassignCustomersList->begin();
        //cout << "next customer " << tempCustomer << " has ratio " << CUSTOMERS[tempCustomer].ratio << '\n';
        iter = templist.begin();
        while (CUSTOMERS[*iter].ratio < CUSTOMERS[tempCustomer].ratio && *iter != templist.back()) {
            advance(iter, 1);
            //cout << "stop at customer " << *iter << " with ratio of " << CUSTOMERS[*iter].ratio << '\n';
        }
        
        //cout << "iter is now " << *iter << " and templist.end is now " << templist.back() << '\n';
        if (*iter == templist.back() && CUSTOMERS[tempCustomer].ratio > CUSTOMERS[*iter].ratio) { //if iter is already at the last position, use push_back function
            templist.push_back(tempCustomer);
            //cout << "customer " << *iter << " is the last customer, use push_back" <<'\n';
        } else { //if iter is not at the last position, move iter to the next position and use insert
            templist.insert(iter, tempCustomer);
            //cout << "not the last customer, stop at customer " << *iter << " has ratio of " << CUSTOMERS[*iter].ratio << '\n';
        }
        //showTheContent(templist);
        //remove the customer that has been added into the temp list.
        unassignCustomersList->pop_front();
        //cout << "unassignCustomersList now look like this ";
        //showTheContent(*unassignCustomersList);
        //cout << "templist now look like this ";
        //showTheContent(templist);
    }
    //after arranging
    *unassignCustomersList = templist;
    //cout << "ranking complete, replacing unassignCustomerList with templist";
    //showTheContent(*unassignCustomersList);
    
    //cout << "size of unassignCustomerlist is " << unassignCustomersList->size() << '\n';
    unassignCustomersList->reverse();
    iter = unassignCustomersList->begin();
    //for (int i = 0; i < unassignCustomersList->size(); i++) {
        //cout << CUSTOMERS[*iter].ratio <<'\n';
        //advance(iter, 1);
    //}
    
}



void initialSolution(customer CUSTOMERS[], list<int>* unassignCustomersList, list<int> vehicles[]) {
    
    int customer = 0;
    int i = 0;
    int vehicleNo = 0;
    //bool vehicleFilled = false;
    //srand(time(NULL)); //set random seeds with time.
    list<int>::iterator iter;
    list<int> tempVehicle;
    
    //initialise the tracker array with from 0 to TOTALCUSTOMERS-1.
    for (i = 0; i < TOTALCUSTOMERS; i++){
        unassignCustomersList->push_back(i);
    }
    
    //rank the customers according to profit/demand
    rankCustomer(CUSTOMERS, unassignCustomersList);
    
    int customerIndex = 0;
    while (unassignCustomersList->empty() == false && vehicleNo < MAXVEHICLES) {
        //cout << "try customer index number " << customerIndex << '\n';
        //cout << "Processing Vehicle no. " << vehicleNo << '\n';
        //cout << "unassignCustomerList is = ";
        //showTheContent(*unassignCustomersList);
            
        //int randomCustomerIndex = rand() % unassignCustomersList->size(); //randomly generates a number from the list
        
        //cout << "customer index is " << customerIndex << '\n';
        iter = unassignCustomersList->begin(); //set iterator to the start.
        advance(iter, customerIndex); //move iterator to the random number generated.
        customer = *iter; //get the number that iterator is pointing to.
        //cout << "Chosen Number is = " << customer << '\n';
        //cout << "Customer Demand is = " << CUSTOMERS[customer].demand << '\n';
        tempVehicle.push_back(customer);
        //cout << "temp vehicle now has ";
        //showTheContent(tempVehicle);
        //cout << "vehicle capacity is " << getVehicleCapacity(CUSTOMERS, tempVehicle) << '\n';
        //cout << "vehicle time taken is " << getVehicleTimeTaken(CUSTOMERS, tempVehicle) << '\n';
        //cout << "max capacity is " << MAXCAPACITY << '\n';
        //cout << "max time taken " << MAXTIME << '\n';
        if (getVehicleCapacity(CUSTOMERS, tempVehicle) <= MAXCAPACITY && getVehicleTimeTaken(CUSTOMERS, tempVehicle) <= MAXTIME) { //as long as adding the new demand does not exceed the car capcacity.
            //cout << "erasing customer " << customer << '\n';
            
            if (unassignCustomersList->size() == 1) {
                unassignCustomersList->clear();
            } else {
                unassignCustomersList->erase(iter);//remove the customer from the unassignCustomers list
            }
            if (customerIndex != 0) {
                customerIndex--;
            }
            vehicles[vehicleNo].push_back(customer); //add the customer into the respective vehicle list.
            
            //cout << "Current capacity = " << getVehicleCapacity(CUSTOMERS, vehicles[vehicleNo]) << '\n';

        } else {
            //remove the previously added customer
            tempVehicle.pop_back();
        }
        
        
        if (customerIndex == unassignCustomersList->size() - 1){
            //cout << "end of unassign customer list, changing vehicle. temp vehicle now consists of ";
            //showTheContent(vehicles[vehicleNo]);
            //cout << "unassign list contains ";
            //showTheContent(*unassignCustomersList);
            tempVehicle.clear();
            vehicleNo++;
            customerIndex = 0;
            
        }
        else {
            customerIndex++;
        }
    }
    //getTotalProfit(CUSTOMERS, vehicles);
    cout << "initialsolution has finished running" << '\n';
    //cout << "vehicle has " << getVehicleCustomerSize(vehicles) << " total customers" << '\n';
    
}

void swap(customer CUSTOMERS[], list<int>* unassignCustomersList, list<int> vehicles[]){
    //choose two random numbers
    //if swap does not violate constraints --> do swap
    
    bool swapSuccessful = false;
    bool customer1InVehicleList = false;
    bool customer2InVehicleList = false;
    int index1 = 0;
    int index2 = 0;
    int tempCustomer = 0;
    int i = 0;
    int j = 0;
    list<int> tempVehicle1;
    list<int> tempVehicle2;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    cout << "entering swap function " << '\n';
    
    if (getVehicleCustomerSize(vehicles) == 0) {
        cout << "vehicle is empty in swap" << '\n';
        return;
    }
    
    if (unassignCustomersList->empty() == true) {
        cout << "unassignCustomerList is empty" << '\n';
        return;
    }

    while (swapSuccessful == false) {
        
        //cout << "entering while loop" << '\n';
        //index1 = rand()%TOTALCUSTOMERS;
        //index2 = rand()%TOTALCUSTOMERS;
        index1 = rand()%getVehicleCustomerSize(vehicles);
        index2 = (rand()%unassignCustomersList->size())+getVehicleCustomerSize(vehicles);
        //cout << "index1 is " << index1 << '\n';
        
        //cout << "index2 is " << index2 << '\n';

        
        if (index1 == index2) { //if i and j are pointing at the same customer, choose random number again
            //cout << "index1==index2 detacted, next iteration" << '\n';
            continue;
        }
        
        //processing the first customer
        if (index1 < getVehicleCustomerSize(vehicles)) { //index1 is in assignedCustomersList
            
            customer1InVehicleList = true;
            while (vehicles[i].size() <= index1) { //check which vehicle is it in;
                index1 = index1 - vehicles[i].size();
                i++;
            }
            tempVehicle1 = vehicles[i];
            iter1 = tempVehicle1.begin();
            advance(iter1, index1); //get customer number from the vehicle
            //cout << "The first customer Chosen is " << *iter1 << '\n';
            //cout << "the vehicle now look like this ";
            //showTheContent(vehicles[i]);
        } else { //index1 is in unassignCustomersList
            
            customer1InVehicleList = false;
            index1 = index1 - getVehicleCustomerSize(vehicles);
            tempVehicle1 = *unassignCustomersList;
            iter1 = tempVehicle1.begin();
            advance(iter1, index1); //get customer number from the unassignCustomerList.
            //cout << "index1 is pointing to a customer in unassignCustomerList ";
            //cout << "the Customer is " << *iter1 << '\n';
            //cout << "unassign list now look like this ";
            //showTheContent(*unassignCustomersList);
        }
            
        if (index2 < getVehicleCustomerSize(vehicles)) { //index2 is in assignedCustomersList
            
            customer2InVehicleList = true;
            
            while (vehicles[j].size() <= index2) { //check which vehicle is it in;
                index2 = index2 - vehicles[j].size();
                j++;
            }
            tempVehicle2 = vehicles[j];
            iter2 = tempVehicle2.begin();
            advance(iter2, index2); //get customer number from the vehicle
            //cout << "The second customer Chosen is " << *iter2 << '\n';
            //cout << "the vehicle now look like this ";
            //showTheContent(vehicles[j]);
        } else { //index2 is in unassignCustomerList
            
            //if both customer indexes are in unassignCustomerList, re-choose indexes.
            if (customer1InVehicleList == false) {
                //cout << "both customers are in unassignCustomerList, re choose indexes" << '\n';
                continue;
            }
            
            customer2InVehicleList = false;
            index2 = index2 - getVehicleCustomerSize(vehicles);
            tempVehicle2 = *unassignCustomersList;
            iter2 = tempVehicle2.begin();
            advance(iter2, index2); //get customer number from the unassignCustomerList.
            //cout << "index2 is pointing to a customer in unassignCustomerList ";
            //cout << "the Customer is " << *iter2 << '\n';
            //cout << "unassign list now look like this ";
            //showTheContent(*unassignCustomersList);
        }
        
        //swapping
        tempCustomer = *iter1;
        //cout << "iter1 is " << *iter1 << '\n';
        tempVehicle1.erase(iter1);
        iter1 = tempVehicle1.begin();
        advance(iter1, index1);
        //cout << "*iter1 now is " << *iter1 << '\n';
        tempVehicle1.insert(iter1, *iter2);
        tempVehicle2.erase(iter2);
        iter2 = tempVehicle2.begin();
        advance(iter2, index2);
        tempVehicle2.insert(iter2, tempCustomer);
        iter1 = tempVehicle1.begin();
        advance(iter1, index1);
        iter2 = tempVehicle2.begin();
        advance(iter2, index2);
        //cout << "After swaping, customer1 is " << *iter1 << '\n';
        //cout << "After swaping, customer2 is " << *iter2 << '\n';
        
            
        if (customer1InVehicleList == true && customer2InVehicleList == true) { //if both customers are in vehicles
                
            if (i == j) { //if both customers in same vehicle
                //cout << "i == j detected!!!! " << '\n';
                //cout << "original vehicle looks like this ";
                //showTheContent(tempVehicle1);
                //cout << "and this ";
                //showTheContent(tempVehicle2);
                tempCustomer = *iter2;
                iter1 = tempVehicle1.begin();
                advance(iter1, index2);
                tempVehicle1.erase(iter1);
                iter1 = tempVehicle1.begin();
                advance(iter1, index2);
                tempVehicle1.insert(iter1, tempCustomer);
                tempVehicle2 = tempVehicle1;
                //cout << "after modifying now vehicle look like this ";
                //showTheContent(tempVehicle1);
            }
            //if both vehicle satisfied constraints
            if (getVehicleTimeTaken(CUSTOMERS, tempVehicle1) <= MAXTIME && getVehicleTimeTaken(CUSTOMERS, tempVehicle2) <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle1) <= MAXCAPACITY && getVehicleCapacity(CUSTOMERS, tempVehicle2) <= MAXCAPACITY) {
                vehicles[i] = tempVehicle1;
                vehicles[j] = tempVehicle2;
                swapSuccessful = true;
                //cout << "swap successful!!" << '\n';
                //cout << "vehicle i now look like this ";
                //showTheContent(vehicles[i]);
                //cout << "vehicle j now look like this ";
                //showTheContent(vehicles[j]);
            } else {
                //cout << "constraint not met, swap is not carried out 1" << '\n';
                //cout << "vehicle i now look like this ";
                   //showTheContent(vehicles[i]);
                   //cout << "vehicle j now look like this ";
                   //showTheContent(vehicles[j]);
            }
        }
        //if first customer in vehicle and second customer in unassignlist
        if (customer1InVehicleList == true && customer2InVehicleList == false) {
            if (getVehicleTimeTaken(CUSTOMERS, tempVehicle1) <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle1) <= MAXCAPACITY) {
                vehicles[i] = tempVehicle1;
                *unassignCustomersList = tempVehicle2;
                swapSuccessful = true;
                //cout << "swap successful!!" << '\n';
                //cout << "vehicle i now look like this ";
                //showTheContent(vehicles[i]);
                //cout << "unassignlist look like this ";
                //showTheContent(*unassignCustomersList);
                
            } else {
                //cout << "constraint not met, swap is not carried out 2" << '\n';
                //cout << "vehicle i now look like this ";
                //showTheContent(vehicles[i]);
                //cout << "unassignlist look like this ";
                //showTheContent(*unassignCustomersList);
            }
        }
        //if first customer in unassignlist and second customer in vehicle
        if (customer1InVehicleList == false && customer2InVehicleList == true) {
            if (getVehicleTimeTaken(CUSTOMERS, tempVehicle2) <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle2) <= MAXCAPACITY) {
                *unassignCustomersList = tempVehicle1;
                vehicles[j] = tempVehicle2;
                swapSuccessful = true;
                //cout << "swap successful!!" << '\n';
                //cout << "vehicle j now look like this ";
                //showTheContent(vehicles[j]);
                //cout << "unassignlist look like this ";
                //showTheContent(*unassignCustomersList);
            } else {
                //cout << "constraint not met, swap is not carried out 3" << '\n';
                //cout << "vehicle j now look like this ";
                //showTheContent(vehicles[j]);
                //cout << "unassignlist look like this ";
                //showTheContent(*unassignCustomersList);
            }
        }
        i = 0;
        j = 0;
    }
    cout << "swap complete " << '\n';
    
}
    

void insertion(customer CUSTOMERS[], list<int>* unassignCustomersList, list<int> vehicles[]){
    
    bool insertSuccessful = false;
    bool customer1InVehicleList = false;
    bool customer2InVehicleList = false;
    int index1 = 0;
    int index2 = 0;
    int tempCustomer = 0;
    int i = 0;
    int j = 0;
    list<int> tempVehicle1;
    list<int> tempVehicle2;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    
    cout << "entering insert function " << '\n';
    if (getVehicleCustomerSize(vehicles) == 0) {
        cout << "vehicle is empty in insertion" << '\n';
        return;
    }
    if (unassignCustomersList->empty() == true) {
        cout << "unassignCustomerList is empty" << '\n';
        return;
    }
    while (insertSuccessful == false) {
        
        //cout << "entering while loop" << '\n';
        //index1 = rand()%TOTALCUSTOMERS;
        //cout << "index1 is " << index1 << '\n';
        //index2 = rand()%TOTALCUSTOMERS;
        //cout << "index2 is " << index2 << '\n';
        index1 = rand()%getVehicleCustomerSize(vehicles);
        index2 = (rand()%unassignCustomersList->size())+getVehicleCustomerSize(vehicles);
        
        if (index1 == index2) { //if i and j are pointing at the same customer, choose random number again
            //cout << "index1==index2 detacted, next iteration" << '\n';
            continue;
        }
        
        //processing the first customer
        if (index1 < getVehicleCustomerSize(vehicles)) { //index1 is in vehicle
            
            while (vehicles[i].empty() == true) { //if found a empty vehicle, fill up the empty vehicle first.
                i++;
            }
            customer1InVehicleList = true;
            while (vehicles[i].size() <= index1) { //check which vehicle is it in;
                index1 = index1 - vehicles[i].size();
                i++;
            }
            tempVehicle1 = vehicles[i];
            iter1 = tempVehicle1.begin();
            advance(iter1, index1); //get customer number from the vehicle
            //cout << "The first customer Chosen is " << *iter1 << '\n';
            //cout << "the vehicle now look like this ";
            //showTheContent(vehicles[i]);
        } else { //index1 is in unassignCustomersList
            
            customer1InVehicleList = false;
            index1 = index1 - getVehicleCustomerSize(vehicles);
            tempVehicle1 = *unassignCustomersList;
            iter1 = tempVehicle1.begin();
            advance(iter1, index1); //get customer number from the unassignCustomerList.
            //cout << "index1 is pointing to a customer in unassignCustomerList ";
            //cout << "the Customer is " << *iter1 << '\n';
            //cout << "unassign list now look like this ";
            //showTheContent(*unassignCustomersList);
        }
        
        //processing second customer
        if (index2 < getVehicleCustomerSize(vehicles)) { //index2 is in vehicle
            
            customer2InVehicleList = true;
            
            while (vehicles[j].empty() == true) {
                j++;
            }
            
            
            while (vehicles[j].size() <= index2) { //check which vehicle is it in;
                index2 = index2 - vehicles[j].size();
                j++;
            }
            tempVehicle2 = vehicles[j];
            iter2 = tempVehicle2.begin();
            advance(iter2, index2); //get customer number from the vehicle
            //cout << "The second customer Chosen is " << *iter2 << '\n';
            //cout << "the vehicle now look like this ";
            //showTheContent(vehicles[j]);
        } else { //index2 is in unassignCustomerList
            
            //if both customer indexes are in unassignCustomerList, re-choose indexes.
            if (customer1InVehicleList == false) {
                //cout << "both customers are in unassignCustomerList, re choose indexes" << '\n';
                continue;
            }
            
            customer2InVehicleList = false;
            index2 = index2 - getVehicleCustomerSize(vehicles);
            tempVehicle2 = *unassignCustomersList;
            iter2 = tempVehicle2.begin();
            advance(iter2, index2); //get customer number from the unassignCustomerList.
            //cout << "index2 is pointing to a customer in unassignCustomerList ";
            //cout << "the Customer is " << *iter2 << '\n';
            //cout << "unassign list now look like this ";
            //showTheContent(*unassignCustomersList);
        }
/*===============================================================================================================*/
        //insertion
        tempCustomer = *iter2;
        //cout << "temp customer is now " << *iter2;
        
        if (i != j) { //at least 1 customer is in vehicle and they are in different lists.
            tempVehicle1.insert(iter1, *iter2);
            tempVehicle2.erase(iter2);
        }
            
        if (customer1InVehicleList == true && customer2InVehicleList == true) { //if both customers are in vehicles
                
            if (i == j) { //if both customers in same vehicle
                //cout << "i == j detected!!!! " << '\n';
                //cout << "index 1 and 2 is " << index1 << "and " << index2 << '\n';
                //cout << "original vehicle looks like this ";
                //showTheContent(tempVehicle1);
                //cout << "and this ";
                //showTheContent(tempVehicle2);
                iter2 = tempVehicle1.begin();
                advance(iter2, index2);
                //insertion within same vehicle
                if (index1 < index2) {
                    tempVehicle1.erase(iter2);
                    tempVehicle1.insert(iter1, tempCustomer);
                } else {
                    tempVehicle1.insert(iter1, tempCustomer);
                    tempVehicle1.erase(iter2);
                }
                tempVehicle2 = tempVehicle1;
                //cout << "after modifying now vehicle look like this ";
                //showTheContent(tempVehicle1);
            }
            //if both vehicle satisfied constraints
            if (getVehicleTimeTaken(CUSTOMERS, tempVehicle1) <= MAXTIME && getVehicleTimeTaken(CUSTOMERS, tempVehicle2) <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle1) <= MAXCAPACITY && getVehicleCapacity(CUSTOMERS, tempVehicle2) <= MAXCAPACITY) {
                vehicles[i] = tempVehicle1;
                vehicles[j] = tempVehicle2;
                insertSuccessful = true;
                //cout << "swap successful!!" << '\n';
                //cout << "vehicle" << i << " now look like this ";
                //showTheContent(vehicles[i]);
                //cout << "vehicle " << j << " now look like this ";
                //showTheContent(vehicles[j]);
            } else {
                //cout << "constraint not met, swap is not carried out 1" << '\n';
                //cout << "vehicle i now look like this ";
                //showTheContent(vehicles[i]);
                //cout << "vehicle j now look like this ";
                //showTheContent(vehicles[j]);
            }
        }
        //if first customer in vehicle and second customer in unassignlist
        if (customer1InVehicleList == true && customer2InVehicleList == false) {
            
            
            
            
            if (getVehicleTimeTaken(CUSTOMERS, tempVehicle1) <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle1) <= MAXCAPACITY) {
                vehicles[i] = tempVehicle1;
                *unassignCustomersList = tempVehicle2;
                insertSuccessful = true;
                //cout << "swap successful!!" << '\n';
                //cout << "vehicle " << i << " now look like this ";
                //showTheContent(vehicles[i]);
                //cout << "unassignlist look like this ";
                //showTheContent(*unassignCustomersList);
                
            } else {
                //cout << "constraint not met, swap is not carried out 2" << '\n';
                //cout << "vehicle i now look like this ";
                //showTheContent(vehicles[i]);
                //cout << "unassignlist look like this ";
                //showTheContent(*unassignCustomersList);
            }
        }
        //if first customer in unassignlist and second customer in vehicle
        /*if (customer1InVehicleList == false && customer2InVehicleList == true) {
                if (getVehicleTimeTaken(CUSTOMERS, tempVehicle2) <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle2) <= MAXCAPACITY) {
                    *unassignCustomersList = tempVehicle1;
                    vehicles[j] = tempVehicle2;
                    insertSuccessful = true;
                    cout << "swap successful!!" << '\n';
                    cout << "vehicle j now look like this ";
                    showTheContent(vehicles[j]);
                    cout << "unassignlist look like this ";
                    showTheContent(*unassignCustomersList);
                } else {
                    //cout << "constraint not met, swap is not carried out 3" << '\n';
                    //cout << "vehicle j now look like this ";
                    //showTheContent(vehicles[j]);
                    //cout << "unassignlist look like this ";
                    //showTheContent(*unassignCustomersList);
                }
        }*/
        i = 0;
        j = 0;
    }
    cout << "insertion complete " << '\n';
}

void Reversion(customer CUSTOMERS[], list<int>* unassignCustomersList, list<int> vehicles[]){
    
    bool indexCheck = false;
    int index = 0;
    int index1 = 0;
    int index2 = 0;
    int vehicleNo = 0;
    int customer = 0;
    int vehicleSize = 0;
    int customerIndex = 0;
    int i = 0;
    int j = 0;
    list<int> currentArrangement;
    list<int> templist;
    list<int> tempVehicle;
    list<int>::iterator iter;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    
    
    vehicleSize = getVehicleCustomerSize(vehicles);
    
    if (getVehicleCustomerSize(vehicles) == 0) {
        cout << "vehicle is empty in reversion" << '\n';
        return;
    }
    
    cout << "entering reversion function" << '\n';
    while (indexCheck == false){
        
        //index1 = rand()%TOTALCUSTOMERS;
        index1 = rand()%getVehicleCustomerSize(vehicles);
        //cout << "index1 is " << index1 << '\n';
        index2 = rand()%TOTALCUSTOMERS;
        //cout << "index2 is " << index2 << '\n';
        
        if (index1 == index2) { //if both indexes are pointing at the same customer, choose random number again
            //cout << "index1==index2 detacted, next iteration" << '\n';
            continue;
        } else if (index1 > index2) { //make sure index1 is alway smaller than index2
            int temp = index2;
            index2 = index1;
            index1 = temp;
            //cout << "index1 is bigger than index2, changed them. Now index1 = " << index1 << " index2 = " << index2 << '\n';
        }
        
        if (index1 >= vehicleSize) { // both numbers are in unassignCustomersList
                continue;
        } else {
            indexCheck = true;
           // cout << "indexes preparation complete. " << '\n';
        }
    }
    
    //store all the customers from vehicle to currentArrangement list
    for (vehicleNo = 0; vehicleNo < MAXVEHICLES; vehicleNo++) {
        iter = vehicles[vehicleNo].begin();
        for (j = 0; j < vehicles[vehicleNo].size(); j++){
            currentArrangement.push_back(*iter);
            advance(iter, 1);
        }
        vehicles[vehicleNo].clear();
    }
    //showTheContent(currentArrangement);
    //store all the customers from unassignCustomerList into currentArrangement list
    iter = unassignCustomersList->begin();
    for (i = 0; i < unassignCustomersList->size(); i++){
        currentArrangement.push_back(*iter);
        advance(iter, 1);
    }
    
    //cout << "Current arrangement is ";
    //showTheContent(currentArrangement);
    //cout << "currentArrangement size is " << currentArrangement.size() << '\n';
    //Reverse customer order from index1 to index2
    iter = currentArrangement.begin();
    advance(iter, index1);
    for (index = index1; index <= index2; index++){
        templist.push_front(*iter);
        advance(iter, 1);
    }
    //cout << "The reversed customers are ";
    //showTheContent(templist);
    
    //erase the customer from currentArrangement from index1 to index2
    iter1 = currentArrangement.begin();
    advance(iter1, index1);
    iter2 = currentArrangement.begin();
    advance(iter2, index2+1);
    currentArrangement.erase(iter1, iter2);
    //cout << "after erasing the content, current arrangement look like this ";
    //showTheContent(currentArrangement);
    
    //insert back the reversed customer one by one
    iter1 = currentArrangement.begin();
    advance(iter1, index1);
    iter2 = templist.begin();
    for (i = 0; i < templist.size(); i++) {
        currentArrangement.insert(iter1, *iter2);
        advance(iter2, 1);
    }
    //cout << "after inserting the reverion back it look like this ";
    //showTheContent(currentArrangement);
    
    //Assign the customer back to vehicle and unassignCustomerList
    unassignCustomersList->clear();
    vehicleNo = 0;
    customerIndex = index1;
    while (currentArrangement.empty() == false && vehicleNo < MAXVEHICLES && index1 < currentArrangement.size()) {
        //cout << "Processing Vehicle no. " << vehicleNo << '\n';
        iter = currentArrangement.begin(); //set iterator to the start.
        advance(iter, customerIndex);
        //cout << "current customer is " << *iter << '\n';
        customer = *iter; //get the number that iterator is pointing to.
        tempVehicle.push_back(customer);
        if (getVehicleCapacity(CUSTOMERS, tempVehicle) <= MAXCAPACITY && getVehicleTimeTaken(CUSTOMERS, tempVehicle) <= MAXTIME) { //as long as adding the new demand does not exceed the car capcacity.
             //currentArrangement.erase(iter);//remove the customer from the unassignCustomers list
             //vehicles[vehicleNo].push_back(customer); //add the customer into the respective vehicle list
            //cout << "enter here " << currentArrangement.size() <<'\n';
            //cout << "customer index is " << customerIndex << '\n';
///////////////////////////////////////////
            if (currentArrangement.size() == 1) {
                    currentArrangement.clear();
            } else {
                if (customerIndex == currentArrangement.size()) { //if the iter is pointing at the last customer in currentArrangement
                    currentArrangement.pop_back();
                } else {
                    currentArrangement.erase(iter);//remove the customer from the unassignCustomers list
                }
                    
            }
            
            if (customerIndex != 0) {
                customerIndex--;
            }
            vehicles[vehicleNo].push_back(customer); //add the customer into the respective vehicle list.
            //cout << "Current capacity = " << getVehicleCapacity(CUSTOMERS, vehicles[vehicleNo]) << '\n';

        } else {
            //remove the previously added customer
            tempVehicle.pop_back();
        }
        //cout << "customerIndex is " << customerIndex << '\n';
        //cout << "currentArrangement size is " << currentArrangement.size() << '\n';
        if (customerIndex == currentArrangement.size() - 1){ //check if end of currentArrange is reached
            //cout << "end of unassign customer list, changing vehicle. temp vehicle now consists of ";
            //showTheContent(vehicles[vehicleNo]);
            //cout << "unassign list contains ";
            //showTheContent(*unassignCustomersList);
            tempVehicle.clear();
            vehicleNo++;
            customerIndex = index1;
        } else {
            customerIndex++;
        }
    }
    if (currentArrangement.empty() == false) {
        *unassignCustomersList = currentArrangement;
    }
    
    for (vehicleNo = 0; vehicleNo < MAXVEHICLES; vehicleNo++) {
           //cout << "vehicle " << vehicleNo << " contains ";
           //showTheContent(vehicles[vehicleNo]);
    }
    //cout << "unassignCustomer now contains ";
    //showTheContent(*unassignCustomersList);
    
    cout << "Reversion Completed" << '\n';
}

void localSwapWithinOnePath(customer CUSTOMERS[], list<int> vehicles[]) {
    //all possible combination of selecting two different nodes within the route with least remaining amount of time. only executed if it is able to increase remaining travel time and there is no constraint violation. Terminated once all possible swaps have been done.
    
    int currentTravelTime = 0;
    int remainingTime = 0;
    int leastRemainingTime = 0;
    int leastRemainingVehicle = 0;
    int tempCustomer = 0;
    int i = 0;
    int j = 0;
    list<int> currentVehicle;
    list<int> tempVehicle;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    
    //getting the vehicle with the least remaining time
    
    for (i = 0; i < MAXVEHICLES; i++) {
        
        remainingTime = getVehicleTimeTaken(CUSTOMERS, vehicles[i]);
        //cout << "vehicle " << i << " has remaining time of " << remainingTime << '\n';
        if (remainingTime >= leastRemainingTime) {
            leastRemainingVehicle = i;
            leastRemainingTime = remainingTime;
        }
    }
    //cout << "least remaining vehicle is " << leastRemainingVehicle << '\n';
    //cout << "least Remaininng Time " << leastRemainingTime << '\n';
    
    tempVehicle = vehicles[leastRemainingVehicle];
    
    //swap
    for (i = 0; i < tempVehicle.size(); i++) {
        for (j = i+1; j < tempVehicle.size(); j++){
            
            tempVehicle = vehicles[leastRemainingVehicle];
            
            //cout << "before swapping list look like this ";
            //showTheContent(tempVehicle);
            
            iter1 = tempVehicle.begin();
            advance(iter1, i);
            iter2 = tempVehicle.begin();
            advance(iter2, j);
            tempCustomer = *iter1;
            tempVehicle.erase(iter1);
            iter1 = tempVehicle.begin();
            advance(iter1, i);
            tempVehicle.insert(iter1, *iter2);
            tempVehicle.erase(iter2);
            iter2 = tempVehicle.begin();
            advance(iter2, j);
            tempVehicle.insert(iter2, tempCustomer);
            
            
            //cout << "after swapping list look like this ";
            //showTheContent(tempVehicle);
            
            currentTravelTime = getVehicleTimeTaken(CUSTOMERS, tempVehicle);
            
            if (currentTravelTime < leastRemainingTime) {
                //cout << "current travel time is smaller than the least remaining time " << '\n';
                leastRemainingTime = currentTravelTime;
                vehicles[leastRemainingVehicle] = tempVehicle;
            }
        }
    }
    cout << "local swap within one path is done " << '\n';
    //cout << "the vehicle now look like this ";
    //showTheContent(vehicles[leastRemainingVehicle]);
    //cout << "the travel time now is " << leastRemainingTime << '\n';
}

void localSwapWithinTwoPath(customer CUSTOMERS[], list<int> vehicles[]) {
    // two schedule nodes in two different paths are chosen to do the swap. (base on lowest and second lowest remaining travel time. only accepted if the total remaining travel times from both paths in increased. terminated once all possible swaps have been done.
    int currentTotalTravelTime = 0;
    int nextTotalTravelTime = 0;
    int remainingTime = 0;
    int leastRemainingTime = 0;
    int leastRemainingVehicle = 0;
    int secondLeastRemainingVehicle = 0;
    int secondLeastRemainingTime = 0;
    int tempCustomer = 0;
    int i = 0;
    int j = 0;
    list<int> currentVehicle;
    list<int> tempVehicle1;
    list<int> tempVehicle2;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    
    //getting the vehicle with the least remaining time
    for (i = 0; i < MAXVEHICLES; i++) {
        
        remainingTime = getVehicleTimeTaken(CUSTOMERS, vehicles[i]);
        //cout << "vehicle " << i << " has remaining time of " << remainingTime << '\n';
        
        //if it is better than the least remaining vehicle
        if (remainingTime > leastRemainingTime) {
            
            //if second least remaining vehicle is less than least remaining vehicle
            if (leastRemainingTime > secondLeastRemainingTime) {
                secondLeastRemainingTime = leastRemainingTime;
                secondLeastRemainingVehicle = leastRemainingVehicle;
            }
            //let the current vehicle be the least remaining vehicle
            leastRemainingVehicle = i;
            leastRemainingTime = remainingTime;
        
        // if it is worst than least remaining time but better than second least remaining time.
        } else if (remainingTime > secondLeastRemainingTime) {
            secondLeastRemainingVehicle = i;
            secondLeastRemainingTime = remainingTime;
        }
        //cout << "least remaining vehicle is " << leastRemainingVehicle << '\n';
        //cout << "second least remaining vehicle is " << secondLeastRemainingVehicle << '\n';
    }
    //cout << "least remaining vehicle is " << leastRemainingVehicle << '\n';
    //cout << "least Remaininng Time " << leastRemainingTime << '\n';
    //cout << "second least remaining vehicle is " << secondLeastRemainingVehicle << '\n';
    //cout << "second least Remaininng Time " << secondLeastRemainingTime << '\n';
    tempVehicle1 = vehicles[leastRemainingVehicle];
    tempVehicle2 = vehicles[secondLeastRemainingVehicle];
    
    currentTotalTravelTime = leastRemainingTime + secondLeastRemainingTime;
    //cout << "current total travel time is " << currentTotalTravelTime << '\n';
    
    //swap
    for (i = 0; i < tempVehicle1.size(); i++) {
        for (j = 0; j < tempVehicle2.size(); j++){
            
            tempVehicle1 = vehicles[leastRemainingVehicle];
            tempVehicle2 = vehicles[secondLeastRemainingVehicle];
            //cout << "before swapping list look like this ";
            //showTheContent(tempVehicle1);
            //showTheContent(tempVehicle2);
            iter1 = tempVehicle1.begin();
            advance(iter1, i);
            iter2 = tempVehicle2.begin();
            advance(iter2, j);
            tempCustomer = *iter1;
            tempVehicle1.erase(iter1);
            iter1 = tempVehicle1.begin();
            advance(iter1, i);
            tempVehicle1.insert(iter1, *iter2);
            tempVehicle2.erase(iter2);
            iter2 = tempVehicle2.begin();
            advance(iter2, j);
            tempVehicle2.insert(iter2, tempCustomer);
            
            
            //cout << "after swapping list look like this ";
            //showTheContent(tempVehicle1);
            //showTheContent(tempVehicle2);
            
            leastRemainingTime = getVehicleTimeTaken(CUSTOMERS, tempVehicle1);
            secondLeastRemainingTime = getVehicleTimeTaken(CUSTOMERS, tempVehicle2);
            nextTotalTravelTime = leastRemainingTime + secondLeastRemainingTime;
            
            //if the swap reduces the total travel time and does not violate maxtime constraint
            if (nextTotalTravelTime < currentTotalTravelTime && leastRemainingTime <= MAXTIME && secondLeastRemainingTime <= MAXTIME) {
                //cout << "current total travel time is smaller than the current total travel time " << '\n';
                currentTotalTravelTime = nextTotalTravelTime;
                vehicles[leastRemainingVehicle] = tempVehicle1;
                vehicles[secondLeastRemainingVehicle] = tempVehicle2;
            }
        }
    }
    //cout << "the vehicle now look like this ";
    //showTheContent(vehicles[leastRemainingVehicle]);
    //showTheContent(vehicles[secondLeastRemainingVehicle]);
    //cout << "the total travel time now is " << currentTotalTravelTime << '\n';
    cout << "local swap between two pathes is done " << '\n';
}

void localReversion(customer CUSTOMERS[], list<int> vehicles[]) {
    //select one path with the lowest remaining travel time. all possible combinations of selecting two different nodes are enumerated and the sequence of scheduled nodes is reversed as long as there is no constraint violation. It has to increase the remaining travel time of the selected path. only terminated once all possible reversion of sequence has been done.
    int currentTravelTime = 0;
    int remainingTime = 0;
    int leastRemainingTime = 0;
    int leastRemainingVehicle = 0;
    int i = 0;
    int j = 0;
    list<int> currentVehicle;
    list<int> tempVehicle;
    list<int> tempList;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    
    //getting the vehicle with the least remaining time
    for (i = 0; i < MAXVEHICLES; i++) {
        remainingTime = getVehicleTimeTaken(CUSTOMERS, vehicles[i]);
        //cout << "vehicle " << i << " has remaining time of " << remainingTime << '\n';
        if (remainingTime >= leastRemainingTime) {
            leastRemainingVehicle = i;
            leastRemainingTime = remainingTime;
        }
    }
    //cout << "least remaining vehicle is " << leastRemainingVehicle << '\n';
    //cout << "least Remaininng Time " << leastRemainingTime << '\n';
    
    tempVehicle = vehicles[leastRemainingVehicle];
    
    //reversion
    for (i = 0; i < tempVehicle.size(); i++) {
        for (j = i+1; j < tempVehicle.size(); j++){
            
            tempVehicle = vehicles[leastRemainingVehicle];
            
            //cout << "before reversion list look like this ";
            //showTheContent(tempVehicle);
            
            iter1 = tempVehicle.begin();
            advance(iter1, i);
            for (int k=i; k<=j; k++){
                tempList.push_front(*iter1);
                advance(iter1, 1);
            }
            
            //cout << "The reversed customers are ";
            //showTheContent(templist);
            
            //erase the customer from currentArrangement from index1 to index2
            iter1 = tempVehicle.begin();
            advance(iter1, i);
            iter2 = tempVehicle.begin();
            advance(iter2, j+1);
            tempVehicle.erase(iter1, iter2);
            //cout << "after erasing the content, current arrangement look like this ";
            //showTheContent(currentArrangement);
            
            //insert back the reversed customer one by one
            iter1 = tempVehicle.begin();
            advance(iter1, i);
            iter2 = tempList.begin();
            for (int num = 0; num < tempList.size(); num++) {
                tempVehicle.insert(iter1, *iter2);
                advance(iter2, 1);
            }
            tempList.clear();
            
            //cout << "after reversion list look like this ";
            //showTheContent(tempVehicle);
            
            currentTravelTime = getVehicleTimeTaken(CUSTOMERS, tempVehicle);
            
            if (currentTravelTime < leastRemainingTime) {
                //cout << "current travel time is smaller than the least remaining time " << '\n';
                leastRemainingTime = currentTravelTime;
                vehicles[leastRemainingVehicle] = tempVehicle;
            }
        }
    }
    //cout << "the vehicle now look like this ";
    //showTheContent(vehicles[leastRemainingVehicle]);
    //cout << "travel time is now " << leastRemainingTime << '\n';
    cout << "local reversion is done " << '\n';
    
}

void localInsertBetweenPaths(customer CUSTOMERS[], list<int> vehicles[]) {
    //start with first schedule node from the first path. Define set F as the set of feasible insertion of node n in position p of path m. move node n to the first position that would increase total remaining travel of all paths. Otherwise, move on to the next node. Terminate when the last node of the last path is reached.
    int currentRemainingTime = 0;
    int nextRemainingTime = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int num = 0;
    int tempCustomer = 0;
    list<int> firstVehicle;
    list<int> secondVehicle;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    bool insertComplete = false;
    
    //search ENTIRE vehicle for customers in ALL path
    for (i = 0; i < MAXVEHICLES-1; i++){
        //cout << "first Vehicle  " << i << " has these customers ";
        //showTheContent(vehicles[i]);
        j = 0;
        //search ENTIRE vehicle for customers in ONE path
        //must use while loop cause for loop does not take factor in the situation when reducing the size of the vehicle
        while (j < vehicles[i].size()) {
            
            firstVehicle = vehicles[i];
            //cout << "first vehicle before removing customer time taken: " << getVehicleTimeTaken(CUSTOMERS, firstVehicle) << '\n';
            
            iter1 = firstVehicle.begin();
            advance(iter1, j); //move on to the next
            tempCustomer = *iter1;
            //cout << "trying with customer: " << tempCustomer << '\n';
            firstVehicle.erase(iter1); //erase it now since the following loops only works on this customer
            k = 0;
            
            //search ENTIRE vehicle for ONE customer insertion
            while (k < MAXVEHICLES && insertComplete == false) {
                
                //skip if k = i
                if (k == i) {
                    k++;
                    continue;
                }
                
                secondVehicle = vehicles[k];
                
                currentRemainingTime = getVehicleTimeTaken(CUSTOMERS, vehicles[i]) + getVehicleTimeTaken(CUSTOMERS, vehicles[k]);
                
                //cout << "current remaining time is " << currentRemainingTime << '\n';
                
                iter2 = secondVehicle.begin();
                num = 0;
                
                //search ONE path for ONE customer insertion
                while (num < secondVehicle.size() && insertComplete == false) {
                    //get current Remaining time
                    
                    secondVehicle.insert(iter2, tempCustomer);
                    //cout << "after inserting the customer, second vehicle now look like this: ";
                    //showTheContent(secondVehicle);
                    //cout << "first vehicle after removing customer time taken: " << getVehicleTimeTaken(CUSTOMERS, firstVehicle) << '\n';
                    //cout << "second vehicle after insertion customer time taken: " << getVehicleTimeTaken(CUSTOMERS, secondVehicle) << '\n';
                    nextRemainingTime = getVehicleTimeTaken(CUSTOMERS, firstVehicle) + getVehicleTimeTaken(CUSTOMERS, secondVehicle);
                    //cout << "next remaining time is " << nextRemainingTime << '\n';
                    
                    // if does not violate path capacity and duration
                    if (getVehicleCapacity(CUSTOMERS, secondVehicle) <= MAXCAPACITY && getVehicleTimeTaken(CUSTOMERS, secondVehicle) <= MAXTIME && nextRemainingTime < currentRemainingTime){
                        //cout << "does not violate any constraints because capacity now is " << getVehicleCapacity(CUSTOMERS, secondVehicle) << " and time taken is " << getVehicleTimeTaken(CUSTOMERS, secondVehicle) << " and current remaining time is " << currentRemainingTime << " but next remaining time is " << nextRemainingTime << '\n';
                        //do the insertion
                        vehicles[i] = firstVehicle;
                        vehicles[k] = secondVehicle;
                        //cout << "first vehicle " << i << " is now: ";
                        //showTheContent(vehicles[i]);
                        //cout << "second vehicle " << k << " is now: ";
                        //showTheContent(vehicles[k]);
                        currentRemainingTime = nextRemainingTime;
                        insertComplete = true;
                    } else {
                        //reset first and second vehicle
                        //move to next insertion point on the second vehicle
                        //cout << "Constraints violated. capacity now is " << getVehicleCapacity(CUSTOMERS, secondVehicle) << " and time taken is " << getVehicleTimeTaken(CUSTOMERS, secondVehicle) << " and current remaining time is " << currentRemainingTime << " and next remaining time is " << nextRemainingTime << '\n';
                        secondVehicle = vehicles[k];
                        //cout << "reset second vehicle: ";
                        //showTheContent(secondVehicle);
                        insertComplete = false;
                        iter2 = secondVehicle.begin();
                        num++;
                        advance(iter2, num);
                    }
                }
                k++;//increase k
            }
            
            if (insertComplete == true){
                //do not j++ since the customer from the first vehicle is erased
                //cout << "insertion is true " << '\n';
                insertComplete = false; //reset insert complete
            } else {
                j++;
                //cout << "insertion is false, j++. j now is " << j << '\n';
            }
        }
    }
    
    cout << "local insertion Between paths is done " << '\n';
}


void localInsertUnassign(customer CUSTOMERS[], list<int> vehicles[], list<int>* unassignCustomerList) {
    //unscheduled nodes are sorted in descending order of their valuation just like in the initial solution generation stage. Starting from the first unscheduled nodes with the highest valuation, insert node to the position that gives the least addition of the total travel time. Repeat until all unscheduled nodes are inserted.
    int currentRemainingTime = 0;
    int nextRemainingTime = 0;
    int increaseInTime = 0;
    int i = 0;
    int j = 0;
    int k = 0;
    int bestTime = 999;
    //int bestUnassignCustomer = 0;
    int bestPath = 0;
    int bestInsertLocation = 0;
    int tempCustomer = 0;
    list<int> tempUnassignCustomers;
    list<int> tempVehicle;
    //list<int> bestVehicle;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    bool potentialInsert = false;
    
    if (unassignCustomerList->empty() == true) {
        cout << "unassignCustomerList is empty" << '\n';
        return;
    }
    //search ENTIRE vehicle for customers in ONE path
    //must use while loop cause for loop does not take factor in the situation when reducing the size of the vehicle
    rankCustomer(CUSTOMERS, unassignCustomerList);
    while (unassignCustomerList->empty() != true && i < unassignCustomerList->size()) {
        //cout << "unassign list is ";
        //showTheContent(*unassignCustomerList);
        iter1 = unassignCustomerList->begin();
        advance(iter1, i); //move on to the next
        tempCustomer = *iter1;
        //cout << "trying with customer: " << tempCustomer << '\n';
        j = 0;
        //testTotalCustomers(vehicles, unassignCustomerList);
        //search ENTIRE vehicle for ONE customer insertion
        while (j < MAXVEHICLES) {
            
            //cout << "processing vehicle " << j << '\n';
            tempVehicle = vehicles[j];
            currentRemainingTime = getVehicleTimeTaken(CUSTOMERS, tempVehicle);
            //cout << "current remaining time is " << currentRemainingTime << '\n';
            
            if (tempVehicle.empty() == true) {
                tempVehicle.push_back(tempCustomer);
                nextRemainingTime = getVehicleTimeTaken(CUSTOMERS, tempVehicle);
                increaseInTime = nextRemainingTime - currentRemainingTime;
                if (increaseInTime < bestTime) {
                    bestTime = increaseInTime;
                    bestPath = j;
                    bestInsertLocation = k;
                    potentialInsert = true;
                    continue;
                }
            }
               
            iter2 = tempVehicle.begin();
            k = 0;
              
            //search ONE path for ONE customer insertion
            while (k < tempVehicle.size()) {
                //get current Remaining time
                
                tempVehicle.insert(iter2, tempCustomer);
                //cout << "after inserting the customer, temp vehicle now look like this: ";
                //showTheContent(tempVehicle);
                nextRemainingTime = getVehicleTimeTaken(CUSTOMERS, tempVehicle);
                increaseInTime = nextRemainingTime - currentRemainingTime;
                //cout << "next remaining time is " << nextRemainingTime << '\n';
                  
                // if does not violate path capacity and duration
                if (getVehicleCapacity(CUSTOMERS, tempVehicle) <= MAXCAPACITY && getVehicleTimeTaken(CUSTOMERS, tempVehicle) <= MAXTIME && increaseInTime < bestTime){
                    //cout << "does not violate any constraints because capacity now is " << getVehicleCapacity(CUSTOMERS, tempVehicle) << " and time taken is " << getVehicleTimeTaken(CUSTOMERS, tempVehicle) << " and increase in time is  " << increaseInTime << " best time is " << bestTime << '\n';
                    //do the insertion
                    bestTime = increaseInTime;
                    bestPath = j;
                    bestInsertLocation = k;
                    potentialInsert = true;
                }
                k++;
                tempVehicle = vehicles[j];
                iter2 = tempVehicle.begin();
                advance(iter2, k);
            }
            j++;
        }
          
        if (potentialInsert == true){
            iter2 = vehicles[bestPath].begin();
            advance(iter2, bestInsertLocation);
            vehicles[bestPath].insert(iter2, tempCustomer);
            unassignCustomerList->erase(iter1);
            
            //cout << "insertion is true " << " best path is << " << bestPath << " best insertion location is " << bestInsertLocation << '\n';
            
            
            potentialInsert = false; //reset insert complete
        } else {
            i++;
            
        }
        bestTime = 999;
        
        
    }
    cout << "local insertion from unassign list is done " << '\n';
}
    
    
    

void localReplaceAssignWithUnassign(customer CUSTOMERS[], list<int> vehicles[], list<int>* unassignCustomerList) {
    //select path m with the highest remaining travel time, followed by selecting one unscheduled node j with the highest profit, check if j can replace any node in the selected path whose profit is lower than j. If j can replace multiple nodes, replace the node such that this replacement would result in the least addition of operation time. This operation would then move on to the next unscheduled node should replacement be infeasible to any position or a replacement is successful. Repeat operation until all unschedule nodes have been checked.
    
    int i = 0;
    int j = 0;
    int remainingTime = 0;
    int highestRemainingTime = 999;
    int highestRemainingVehicle = 0;
    int bestRemainingTime = 999;
    int unassignCustomer;
    int customerToReplace;
    int bestCustomerToReplace;
    list<int> tempVehicle;
    list<int>::iterator iter1;
    list<int>::iterator iter2;
    bool potentialReplace = false;
    
    if (unassignCustomerList->empty() == true) {
        cout << "unassignCustomerList is empty" << '\n';
        return;
    }
    
    //getting the vehicle with the least remaining time
    rankCustomer(CUSTOMERS, unassignCustomerList);
    for (i = 0; i < MAXVEHICLES; i++) {
        remainingTime = getVehicleTimeTaken(CUSTOMERS, vehicles[i]);
        //cout << "vehicle " << i << " time taken is " << remainingTime << '\n';
        if (remainingTime < highestRemainingTime) {
            highestRemainingVehicle = i;
            highestRemainingTime = remainingTime;
        }
    }
    //cout << "lowest time taken vehicle is " << highestRemainingVehicle << '\n';
    //cout << "lowest time taken is " << highestRemainingTime << '\n';
   
    tempVehicle = vehicles[highestRemainingVehicle];
    i = 0;
    
    while (unassignCustomerList->empty() != true && i < unassignCustomerList->size()) {
        //cout << "unassign list is ";
        //showTheContent(*unassignCustomerList);
        iter1 = unassignCustomerList->begin();
        advance(iter1, i); //move on to the next
        unassignCustomer = *iter1;
        //cout << "trying with customer: " << unassignCustomer << '\n';
        j = 0;
        iter2 = tempVehicle.begin();
        //search ENTIRE vehicle for ONE customer insertion
        while (j < tempVehicle.size()) {
            
            iter2 = tempVehicle.begin();
            advance(iter2, j);
            
            //cout << "before replacement tempVehicle: ";
            //showTheContent(tempVehicle);
            customerToReplace = *iter2;
            replace(tempVehicle.begin(), tempVehicle.end(), customerToReplace, unassignCustomer);
            //cout << "after replacement tempVehicle: ";
            //showTheContent(tempVehicle);
            remainingTime = getVehicleTimeTaken(CUSTOMERS, tempVehicle);
            //cout << "time taken is " << remainingTime << '\n';
            //cout << "unassign customer is " << unassignCustomer << " its profit is " << CUSTOMERS[unassignCustomer].profit << '\n';
            //cout << "replacement customer is " << customerToReplace << " its profit is " << CUSTOMERS[customerToReplace].profit << '\n';
            //cout << "new capacity is " << getVehicleCapacity(CUSTOMERS, tempVehicle) << '\n';
            //if the replacement customer has higher profit than the customer in the vehicle && constraint met
            if (CUSTOMERS[unassignCustomer].profit > CUSTOMERS[customerToReplace].profit && remainingTime <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle) <= MAXCAPACITY && remainingTime < bestRemainingTime) {
                bestRemainingTime = remainingTime;
                bestCustomerToReplace = customerToReplace;
                potentialReplace = true;
                //cout << "condition met! " << "best time taken is " << bestRemainingTime << " customer to replace is " << bestCustomerToReplace << '\n';
            }
            //reset tempVehicle
            tempVehicle.clear();
            tempVehicle = vehicles[highestRemainingVehicle];
            j++;
        }
         
        if (potentialReplace == true){
            //replace the customer
            replace(vehicles[highestRemainingVehicle].begin(), vehicles[highestRemainingVehicle].end(), bestCustomerToReplace, unassignCustomer);
            replace(unassignCustomerList->begin(), unassignCustomerList->end(), unassignCustomer, bestCustomerToReplace);
            tempVehicle = vehicles[highestRemainingVehicle];
            potentialReplace = false; //reset insert complete
            //testTotalCustomers(vehicles, unassignCustomerList);
        }
        i++;
        bestRemainingTime = 999;
    }
    cout << "local replace of assign list with unassign list is done " << '\n';
}



void localSearch(customer CUSTOMERS[], list<int> bestVehicle[], list<int>* bestUnassignList){
    
    // local search does not generate additional profit
    
    localSwapWithinOnePath(CUSTOMERS, bestVehicle);
    //cout << "local swap within one path done" << '\n';
    localSwapWithinTwoPath(CUSTOMERS, bestVehicle);
    //cout << "local swap within two path done" << '\n';
    localReversion(CUSTOMERS, bestVehicle);
    //cout << "local reversion done" << '\n';
    localInsertBetweenPaths(CUSTOMERS, bestVehicle);
    //cout << "local insertion between paths done" <<'\n';
    localInsertUnassign(CUSTOMERS, bestVehicle, bestUnassignList);
    //cout << "local insert unassign list done" << '\n';
    localReplaceAssignWithUnassign(CUSTOMERS, bestVehicle, bestUnassignList);
    //cout << "local replace assign with unassign" << '\n';
}

void simulatedAnnealing(customer CUSTOMERS[], list<int>* unassignCustomersList, list<int> vehicles[]) {
    int I = 0;
    int N = 0;
    double p = 0.0;
    double r = 0.0;
    double currentProfit = 0.0;
    double nextProfit = 0.0;
    double bestProfit = 0.0;
    double bestProfitAfterLocalSearch = 0.0;
    double offsetProfit = 0.0;
    double equationValue = 0.0;
    bool bestProfitUpdated = false;
    
    list<int> currentVehicle[MAXVEHICLES];
    list<int> nextVehicle[MAXVEHICLES];
    list<int> bestVehicle[MAXVEHICLES];
    list<int> bestVehicleAfterLocalSearch[MAXVEHICLES];
    list<int> currentUnassignList;
    list<int> nextUnassignList;
    list<int> bestUnassignList;
    list<int> bestUnassignListAfterLocalSearch;
    
    for (int i = 0; i < MAXVEHICLES; i++) {
        currentVehicle[i] = vehicles[i];
        bestVehicle[i] = vehicles[i];
        nextVehicle[i] = vehicles[i];
    }
    currentUnassignList = *unassignCustomersList;
    bestUnassignList = *unassignCustomersList;
    nextUnassignList = *unassignCustomersList;
    
    
    currentProfit = getTotalProfit(CUSTOMERS, currentVehicle);
    //cout << "current profit is " << currentProfit << '\n';
    bestProfit = getTotalProfit(CUSTOMERS, currentVehicle);
    //cout << "best profit is " << bestProfit << '\n';
    
    while (N < N_nonImproving) {
        
        
        
        for (I = 0; I < Imax; I++){
            p = (double) rand() / (RAND_MAX);
            //cout << "p is " << p << '\n';
            
            if (p <= 1.0/3.0) {
                //cout << "Choose Swap" << '\n';
                swap(CUSTOMERS, &nextUnassignList, nextVehicle);
            }
            else if (p <= 2.0/3.0 && p > 1.0/3.0) {
                //cout << "Choose Insertion" << '\n';
                insertion(CUSTOMERS, &nextUnassignList, nextVehicle);
            }
            else if (p <= 3.0/3.0 && p > 2.0/3.0) {
                //cout << "Choose Reversion" <<'\n';
                Reversion(CUSTOMERS, &nextUnassignList, nextVehicle);
            }
            
            //if F(Y) better than F(X)?
            //cout << "current profit is " << currentProfit << '\n';
            nextProfit = getTotalProfit(CUSTOMERS, nextVehicle);
            //cout << "next Profit is " << nextProfit << '\n';
            offsetProfit = nextProfit - currentProfit;
            //cout << "offset profit is " << offsetProfit << '\n';
            if (offsetProfit >= 0) { // replace with current vehicle
                //cout << "offset profit is positive, currentVehicle = next Vehicle " << '\n';
                for (int i = 0; i < MAXVEHICLES; i++) {
                    currentVehicle[i].clear();
                    currentVehicle[i] = nextVehicle[i];
                }
                currentProfit = nextProfit;
                currentUnassignList.clear();
                currentUnassignList = nextUnassignList;
            } else {
                //cout << "offset profit is negative" << '\n';
                r = (double) rand() / (RAND_MAX);
                //cout << "r is " << r << '\n';
                equationValue = exp((offsetProfit)/T);
                //cout << "equation value is " << equationValue << '\n';
                if (r < equationValue) {
                    //cout << "r is smaller than equation value, accept current vehicle" << '\n';
                    for (int i = 0; i < MAXVEHICLES; i++) {
                        currentVehicle[i].clear();
                        currentVehicle[i] = nextVehicle[i];
                        
                    }
                    currentUnassignList.clear();
                    currentUnassignList = nextUnassignList;
                    currentProfit = nextProfit;
                } else {
                    //cout << "r is bigger than equation value, does not accept current vehicle" << '\n';
                    for (int i = 0; i < MAXVEHICLES; i++) {
                        nextVehicle[i].clear();
                        nextVehicle[i] = currentVehicle[i];
                    }
                    nextProfit = currentProfit;
                    nextUnassignList.clear();
                    nextUnassignList = currentUnassignList;
                    
                    continue;
                }
            }
            //cout << "current vehicle " << '\n';
            
            //is F(X) better than F(best)?
            if (currentProfit > bestProfit) {
                //cout << "current profit is better than best profit " << '\n';
                //cout << "best profit previously is " << bestProfit << '\n';
                bestProfit = currentProfit;
                //cout << "best profit now is " << bestProfit << '\n';
                for (int i = 0; i < MAXVEHICLES; i++) {
                    bestVehicle[i].clear();
                    bestVehicle[i] = currentVehicle[i];
                    //showTheContent(bestVehicle[i]);
                }
                bestUnassignList.clear();
                bestUnassignList = currentUnassignList;
                bestProfitUpdated = true;
            } else {
                //cout << "current profit is not better than best profit " << '\n';
                //cout << "current profit is " << currentProfit << '\n';
            }
            
            //cout << "best profit before local search is " << bestProfit << '\n';
            //cout << "I is " << I << '\n';
            //cout << "vehicle has " << getVehicleCustomerSize(vehicles) << "total customers" << '\n';
        }
        
        T = T*coolingRatio;
        //local search for the best vehicle
        if (bestProfitUpdated == true) {
            localSearch(CUSTOMERS, bestVehicle, &bestUnassignList);
            N = 0;
        } else {
            N++;
        }
        
        testTotalCustomers(CUSTOMERS, bestVehicle, &bestUnassignList);
        bestProfitAfterLocalSearch = getTotalProfit(CUSTOMERS, bestVehicle);
        cout << "best profit after local search is " << bestProfitAfterLocalSearch << '\n';
        
        bestProfitUpdated = false;
    }
    
}

int main() {
    
    //string tempPath = "/Users/jiahuizhu/Desktop/SEM7/FYP/Benchmark\ Instances/DatasetsCTOP/";
    //for (int i=1; i<=1; i++) {
    //    path = tempPath + to_string(i) + "set/b1.txt";
    //}
    
    extractInstanceInfo();
    customer CUSTOMERS[TOTALCUSTOMERS];
    getCustomers(CUSTOMERS);
    
    list<int> unassignCustomersList;
    list<int> vehicles[MAXVEHICLES];
    

    initialSolution(CUSTOMERS, &unassignCustomersList, vehicles);
    //cout << "unassignCustomerList is = ";
    //showTheContent(unassignCustomersList);
    
    //for (int i = 0; i < MAXVEHICLES; i++) {
        //cout << "vehicle " << i << "contains ";
        //showTheContent(vehicles[i]);
    //}
    
    simulatedAnnealing(CUSTOMERS, &unassignCustomersList, vehicles);
    
    return 0;
}



