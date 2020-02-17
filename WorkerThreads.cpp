//
// Created by darea on 17/02/2020.
//
#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <condition_variable>

//TODO: Implement condition variable and stop method.

using namespace std;
//Worker classes:
class Workers{
public:
    vector<thread> threads = vector<thread>();
    vector<function<void()>> tasks = vector<function<void()>>();
    int count;
    //condition_variable cv;
    chrono::duration<double, milli> timeout;

    Workers(){
        cout << "no arguments "<< endl;
    }

    Workers(int count){
        this->count = count;
    }

    void start(){
        condition_variable cv;

        while(threads.size() < count){
            if(!tasks.empty()){
                auto task =  *tasks.begin();
                tasks.erase(tasks.begin());
                thread temp = thread([&task, &cv]{
                    task();
                });
                threads.push_back(temp);
            }else{
                thread temp = thread([]{
                    cout << "Thread initialized" << endl;
                });
                threads.push_back(temp);
            }

        }
    }
    void stop(){

    }

    void post(function<void()> func ){
        std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
        chrono::duration<double, milli> duration = end-start;
        tasks.push_back(func);

        while( duration <= timeout){
            for(auto &&threadI:threads){
                if(threadI.joinable()) {
                    thread temp = thread(func);
                    threadI.swap(temp);
                    return;
                }
            }
        end = std::chrono::steady_clock::now();
        duration = end-start;
        }
        cout<< "timed out" <<endl;
        return;
        //Replace finished thread

    }

    void post_timeout(int ms){
        timeout = chrono::milliseconds(ms);
    }
};

int main(){
    Workers worker_threads(4);
    Workers event_loop(1);
    worker_threads.start(); // Create 4 internal threads
    event_loop.start(); // Create 1 internal thread
    worker_threads.post([] {
                    // Task A
    });
    worker_threads.post([] {
// Task B
// Might run in parallel with task A
    });
    event_loop.post([] {
// Task C
// Might run in parallel with task A and B
    });
    event_loop.post([] {
// Task D
// Will run after task C
// Mig
    });
return 0;
}


