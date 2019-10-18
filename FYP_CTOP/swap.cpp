//
//  swap.cpp
//  FYP_CTOP
//
//  Created by Jiahui Zhu on 27/9/19.
//  Copyright © 2019 Jiahui Zhu. All rights reserved.
//

#include "swap.hpp"
#include <iostream>

using namespace std;

/*void swap(customer CUSTOMERS[], list<int>* unassignCustomersList, list<int>* assignedCustomersList, list<int> vehicles[]){
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
    //cout << "entering swap function " << '\n';
    
    while (swapSuccessful == false) {
        
        //cout << "entering while loop" << '\n';
        index1 = rand()%TOTALCUSTOMERS;
        //cout << "index1 is " << index1 << '\n';
        index2 = rand()%TOTALCUSTOMERS;
        //cout << "index2 is " << index2 << '\n';

        
        if (index1 == index2) { //if i and j are pointing at the same customer, choose random number again
            cout << "index1==index2 detacted, next iteration" << '\n';
            continue;
        }
        
        //processing the first customer
        if (index1 < assignedCustomersList->size()) { //index1 is in assignedCustomersList
            
            customer1InVehicleList = true;
            while (vehicles[i].size() <= index1) { //check which vehicle is it in;
                index1 = index1 - vehicles[i].size();
                i++;
            }
            tempVehicle1 = vehicles[i];
            iter1 = tempVehicle1.begin();
            advance(iter1, index1); //get customer number from the vehicle
            cout << "The first customer Chosen is " << *iter1 << '\n';
            cout << "the vehicle now look like this ";
            showTheContent(vehicles[i]);
        } else { //index1 is in unassignCustomersList
            
            customer1InVehicleList = false;
            index1 = index1 - assignedCustomersList->size();
            tempVehicle1 = *unassignCustomersList;
            iter1 = tempVehicle1.begin();
            advance(iter1, index1); //get customer number from the unassignCustomerList.
            cout << "index1 is pointing to a customer in unassignCustomerList ";
            cout << "the Customer is " << *iter1 << '\n';
            cout << "unassign list now look like this ";
            showTheContent(*unassignCustomersList);
        }
            
        if (index2 < assignedCustomersList->size()) { //index2 is in assignedCustomersList
            
            customer2InVehicleList = true;
            
            while (vehicles[j].size() <= index2) { //check which vehicle is it in;
                index2 = index2 - vehicles[j].size();
                j++;
            }
            tempVehicle2 = vehicles[j];
            iter2 = tempVehicle2.begin();
            advance(iter2, index2); //get customer number from the vehicle
            cout << "The second customer Chosen is " << *iter2 << '\n';
            cout << "the vehicle now look like this ";
            showTheContent(vehicles[j]);
        } else { //index2 is in unassignCustomerList
            
            //if both customer indexes are in unassignCustomerList, re-choose indexes.
            if (customer1InVehicleList == false) {
                cout << "both customers are in unassignCustomerList, re choose indexes" << '\n';
                continue;
            }
            
            customer2InVehicleList = false;
            index2 = index2 - assignedCustomersList->size();
            tempVehicle2 = *unassignCustomersList;
            iter2 = tempVehicle2.begin();
            advance(iter2, index2); //get customer number from the unassignCustomerList.
            cout << "index2 is pointing to a customer in unassignCustomerList ";
            cout << "the Customer is " << *iter2 << '\n';
            cout << "unassign list now look like this ";
            showTheContent(*unassignCustomersList);
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
        cout << "After swaping, customer1 is " << *iter1 << '\n';
        cout << "After swaping, customer2 is " << *iter2 << '\n';
        
            
        if (customer1InVehicleList == true && customer2InVehicleList == true) { //if both customers are in vehicles
                
            if (i == j) { //if both customers in same vehicle
                cout << "i == j detected!!!! " << '\n';
                cout << "original vehicle looks like this ";
                showTheContent(tempVehicle1);
                cout << "and this ";
                showTheContent(tempVehicle2);
                tempCustomer = *iter2;
                iter1 = tempVehicle1.begin();
                advance(iter1, index2);
                tempVehicle1.erase(iter1);
                iter1 = tempVehicle1.begin();
                advance(iter1, index2);
                tempVehicle1.insert(iter1, tempCustomer);
                tempVehicle2 = tempVehicle1;
                cout << "after modifying now vehicle look like this ";
                showTheContent(tempVehicle1);
            }
            //if both vehicle satisfied constraints
            if (getVehicleTimeTaken(CUSTOMERS, tempVehicle1) <= MAXTIME && getVehicleTimeTaken(CUSTOMERS, tempVehicle2) <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle1) <= MAXCAPACITY && getVehicleCapacity(CUSTOMERS, tempVehicle2) <= MAXCAPACITY) {
                vehicles[i] = tempVehicle1;
                vehicles[j] = tempVehicle2;
                swapSuccessful = true;
                cout << "swap successful!!" << '\n';
                cout << "vehicle i now look like this ";
                showTheContent(vehicles[i]);
                cout << "vehicle j now look like this ";
                showTheContent(vehicles[j]);
            } else {
                cout << "constraint not met, swap is not carried out 1" << '\n';
                cout << "vehicle i now look like this ";
                   showTheContent(vehicles[i]);
                   cout << "vehicle j now look like this ";
                   showTheContent(vehicles[j]);
            }
        }
        //if first customer in vehicle and second customer in unassignlist
        if (customer1InVehicleList == true && customer2InVehicleList == false) {
            if (getVehicleTimeTaken(CUSTOMERS, tempVehicle1) <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle1) <= MAXCAPACITY) {
                vehicles[i] = tempVehicle1;
                *unassignCustomersList = tempVehicle2;
                swapSuccessful = true;
                cout << "swap successful!!" << '\n';
                cout << "vehicle i now look like this ";
                   showTheContent(vehicles[i]);
                   cout << "unassignlist look like this ";
                   showTheContent(*unassignCustomersList);
                
            } else {
                cout << "constraint not met, swap is not carried out 2" << '\n';
                cout << "vehicle i now look like this ";
                showTheContent(vehicles[i]);
                cout << "unassignlist look like this ";
                showTheContent(*unassignCustomersList);
            }
        }
        //if first customer in unassignlist and second customer in vehicle
        if (customer1InVehicleList == false && customer2InVehicleList == true) {
                if (getVehicleTimeTaken(CUSTOMERS, tempVehicle2) <= MAXTIME && getVehicleCapacity(CUSTOMERS, tempVehicle2) <= MAXCAPACITY) {
                    *unassignCustomersList = tempVehicle1;
                    vehicles[j] = tempVehicle2;
                    swapSuccessful = true;
                    cout << "swap successful!!" << '\n';
                    cout << "vehicle j now look like this ";
                    showTheContent(vehicles[j]);
                    cout << "unassignlist look like this ";
                    showTheContent(*unassignCustomersList);
                } else {
                    cout << "constraint not met, swap is not carried out 3" << '\n';
                    cout << "vehicle j now look like this ";
                    showTheContent(vehicles[j]);
                    cout << "unassignlist look like this ";
                    showTheContent(*unassignCustomersList);
                }
        }
        i = 0;
        j = 0;
        cout << "swap complete " << '\n';
    }
}
*/
