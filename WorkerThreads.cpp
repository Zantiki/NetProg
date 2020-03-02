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

class Workers {
private:
    vector<thread> threads;
    vector<function<void()>> tasks;
    bool kill = false;
    unsigned int count;
    unsigned int tasksCount = 0;

public:
    condition_variable cv;
    mutex mtx;

    Workers(int count) {
        this->count = count;
    }

    //Task method available for all threads.
    void doTask() {
        cout << "start thread " << this_thread::get_id() << endl;
        while (true) {
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

    //Initialize all threads
    void start(){
        while(threads.size() < count){
            //Include object to make doTask() available
            thread temp = thread([this]{
                doTask();
            });
            threads.push_back(move(temp));
        }
    }


    void stop(){
        //Help-variable only present in parent thread, dependent on tasks.
        bool listen = true;
        while (listen) {
            {
                //Lock and check task-state, kill children and end loop if empty.
                lock_guard<mutex> lock(mtx);
                if (tasks.empty()) {
                    kill = true;
                    listen = false;
                }
            }
            //Join all threads on empty tasklist
            if (!listen) {
                cv.notify_all();
                for (auto &&t:threads) {
                    t.join();
                }
            }
        }

    }


    //Lock and post a new task before notifying a thread.
    void post(function<void()> func) {
        lock_guard<mutex> lock(mtx);
        tasks.emplace_back(func);
        cv.notify_one();
    }

    //Same as above, just with user-defined timeout embedded.
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

/*int main() {
    Workers worker_threads(4);
    Workers event_loop(1);
    worker_threads.start(); // Create 4 internal threads
    event_loop.start(); // Create 1 internal thread

    event_loop.post_timeout(2000, [] {
        cout << "Timeout by A " << this_thread::get_id() << endl;
    });
    worker_threads.post_timeout(3000, [] {
        cout << "Timeout by B " << this_thread::get_id() << endl;
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
}*/


