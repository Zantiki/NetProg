//
// Created by darea on 17/02/2020.
//
#include <thread>
#include <iostream>
#include <vector>
#include <functional>
#include <chrono>
#include <condition_variable>

using namespace std;

class Workers{
private:
    vector<thread> threads;
    vector<function<void()>> tasks;
    bool kill = false;
    mutex mtx;
    unsigned int count;
    unsigned int tasksCount = 0;
    condition_variable cv;

public:	
    Workers(int count){
        this->count = count;
    }

    void doTask(){
	cout << "start thread " << this_thread::get_id() << endl;
        while(true){
            function<void()> task;
            {
               unique_lock<mutex> lock(mtx);
                if(!tasks.empty()) {
                   task = *tasks.begin();
                    tasks.erase(tasks.begin());
                }else{
                    if(kill){
                        return;
                    }
                   cv.wait(lock);
                }
            }
            if(task){
                task();
            }

        }
    }

    void start(){
        while(threads.size() < count){
            thread temp = thread([this]{
                doTask();
            });
            threads.push_back(move(temp));
        }
    }

    void stop(){
        bool listen = true;
        while (listen) {
            {
                lock_guard<mutex> lock(mtx);
                if (tasks.empty()) {
                    kill = true;
                    listen = false;
                }
            }
            if (!listen) {
                cv.notify_all();
                for (auto &&t:threads) {
                    t.join();
                }
            }
        }

    }


    void post(function<void()> func) {
        lock_guard<mutex> lock(mtx);
        tasks.emplace_back(func);
        cv.notify_one();
    }

    void post_timeout(int time, function<void()> func) {
        post([time, func] {
            {
                chrono::milliseconds timeout(time);
                if (timeout != 0ms) {
                    cout << "Task has timeout: " << timeout.count() << endl;
                    this_thread::sleep_for(timeout);
                    func();
                } else {
                    func();
                }

            }
        });
    }
};

int main() {
    Workers worker_threads(4);
    Workers event_loop(1);
    worker_threads.start(); // Create 4 internal threads
    event_loop.start(); // Create 1 internal thread

    event_loop.post_timeout(3000, [] {
        cout << "Timeout by " << this_thread::get_id() << endl;
    });
    worker_threads.post_timeout(3000, [] {
        cout << "Timeout by " << this_thread::get_id() << endl;
    });

    worker_threads.post([] {
        cout << "a done by " << this_thread::get_id() << endl;
    });
    worker_threads.post([] {
        cout << "b done by " << this_thread::get_id() << endl;
    });
    event_loop.post([] {
        cout << "c done by " << this_thread::get_id() << endl;
    });
    event_loop.post([] {
        cout << "d done by " << this_thread::get_id() << endl;
    });
   worker_threads.stop();
   event_loop.stop();
return 0;
}


