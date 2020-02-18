//
// Created by darea on 17/02/2020.
//
#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <condition_variable>
#include <atomic>

//TODO: Implement condition variable and stop method.

using namespace std;
//Worker classes:
class Workers{
public:
    vector<thread> threads = vector<thread>();
    vector<function<void()>> tasks = vector<function<void()>>();
    atomic_bool kill = ATOMIC_VAR_INIT(false);
    mutex mtx;
    unsigned int count;
    condition_variable cv;
    chrono::duration<double, milli> timeout;

    Workers(){
        cout << "no arguments "<< endl;
    }

    Workers(int count){
        this->count = count;
    }

    void doTask(){
        while(!kill){
            unique_lock<mutex> lock(mtx);
            if(!tasks.empty()) {
                auto task = *tasks.begin();
                tasks.erase(tasks.begin());
                task();
            }else{
                if(kill){
                    return;
                }
                cv.wait(lock);
            }
        }
    }

    /*void start(){
        while(threads.size() < count){
            if(!tasks.empty()){
                auto task =  *tasks.begin();
                tasks.erase(tasks.begin());
                thread temp = thread([this]{
                    doTask();
                });
                threads.push_back(temp);
            }else{
                thread temp = thread([]{
                    cout << "Thread initialized" << endl;
                });
                threads.push_back(temp);
            }

        }
    }*/

    void start(){
        while(threads.size() < count){
            thread temp = thread([this]{
                doTask();
            });
            threads.push_back(move(temp));
        }
    }

    void stop(){
        cv.notify_all();
        kill = true;
        for(auto &&t:threads){
            t.join();
        }
    }

    /*void post(function<void()> func ){
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

    }*/

    void post(function<void()> func){
        cout << "Task posted" << endl;
        tasks.push_back([&func, this]{

            this_thread::sleep_for(timeout);
            func();

        });
    }

    void post_timeout(int ms){
        cout << "Timeout set to" << ms << " milliseconds" << endl;
        timeout = chrono::milliseconds(ms);
    }
};

int main(){
    Workers worker_threads(4);
    Workers event_loop(1);
    worker_threads.start(); // Create 4 internal threads
    event_loop.start(); // Create 1 internal thread
    worker_threads.post([] {
        cout << "task A started" << endl;
    });
    worker_threads.post([] {
        cout << "task B started" << endl;
    });
    event_loop.post([] {
        cout << "task C started" << endl;
    });
    event_loop.post([] {
        cout << "task D started" << endl;
    });
    worker_threads.stop();
    event_loop.stop();
return 0;
}


