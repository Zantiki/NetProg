//
// Created by Sebastian Ikin on 03/02/2020.
//
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <math.h>
#include "Timer.cpp"
using namespace std;

class Bounds{
public:
    int upper;
    int lower;
    int shift;

    Bounds(){
        upper = 2;
        lower = 2;
        shift = 0;
    }

    Bounds(int upper1, int lower1, int shift1){
        upper = upper1;
        lower = lower1;
        shift = shift1;
    }
};

vector<bool> sieve = vector<bool>();
mutex mtx;

bool isPrime(int number){
    if(number <=1)
        return false;
    for(int i=2; i <= sqrt(number); i++)
        if(number %i ==0)
            return false;
    return true;
}

//Calculate bound based on approximate runtime
Bounds calculateBounds(int currentUpper, int rangeSize, int maxSize, int shift){
    int lower = currentUpper;
    int upper = lower + rangeSize;

  /* while(!isPrime((upper)) && upper < maxSize){
        /*shift++;
        upper++;
        //cout << upper << endl;
    }*/
    while(upper > maxSize){
        upper--;
    }
    Bounds calculatedBounds(upper, lower, shift);
    return calculatedBounds;
}

void setPrimes(Bounds bounds, int startIndex){
    cout << "Thread has upper bound of: "<< bounds.upper  << endl;
    while (bounds.lower * bounds.lower <= bounds.upper) {

        if (sieve[bounds.lower-startIndex]) {
            for (int i = bounds.lower * 2; i < bounds.upper; i += bounds.lower) {
                sieve[i-startIndex] = false;
                cout << sieve[i-startIndex]<< " is not a prime"  << endl;
            }
        }

        bounds.lower++;
    }
    return;
}

void setPrimesSimple(Bounds bounds){
    for(int i = bounds.lower; i < bounds.upper; i++ ){
        //mtx.lock();
        sieve[i] = isPrime(i);
        //mtx.unlock();
    }
}


//Driver function
int main(){
    //vector<thread> threadList;
    int maxThread = 8;
    vector<Bounds> boundList;
    sieve = vector<bool>();
    thread threads[maxThread];
    int lowerBound = 0, upperBound = 0, startIndex;

    while(upperBound <= lowerBound){
        cout << "What is your lower bound?" << endl;
        cin >> lowerBound;
        startIndex = lowerBound;

        cout << "What is your upper bound?" << endl;
        cin >> upperBound;

        if(upperBound <= lowerBound){
            cout << "Please input correct bounds" << endl;
        }
    }

   /* if(lowerBound < 2){
        sieve.push_back(false);
        sieve.push_back(false);
        lowerBound = 2;
    }*/

    Timer time = Timer();
    int i = 0;
    while(i < upperBound){
        sieve.push_back(true);
        i++;
    }
    //bool sieve = new bool [upperBound-lowerBound];

    double maxrange = ceil(((double)(upperBound - lowerBound)) / maxThread);
    cout << maxrange << endl;
    Bounds boundRange = Bounds(lowerBound+maxrange, lowerBound, 0);
    //int remainder = (upperBound - lowerBound) % maxThread;

    while(!(boundList.size() == maxThread)){
        cout << "Added Bounds from " << boundRange.lower << " to " << boundRange.upper << endl;
        boundList.push_back(boundRange);
        boundRange = calculateBounds(boundRange.upper, maxrange, upperBound, boundRange.shift);
    }

    int counter = 0;
    time.set();
    for (Bounds &bounds : boundList ){
        //Bounds bounds = *i;
        cout << "Thread " << counter << " has bounds " << bounds.lower << " to " << bounds.upper << endl;
        int internalCounter = counter;
        threads[counter] = thread([&bounds, &startIndex] {
            //setPrimes(bounds, startIndex);
            setPrimesSimple(bounds);
        });
        counter++;
    }


    for(auto& th : threads) th.join();
    double runTime = time.stop();

    cout << "\nPrinting primes\n" << endl;
    for(unsigned int i = startIndex; i < sieve.size(); i++){
        if(sieve[i]){
            cout << i << endl;
        }
    }
    cout << "Current runtime " << runTime << " ms " << endl;
    return 0;
}


