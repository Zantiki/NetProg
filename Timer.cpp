//
// Created by Jakob on unknown date.
//

#pragma once

#include <chrono>

using namespace std;

class Timer
{
private:
    chrono::high_resolution_clock::time_point start;
    chrono::high_resolution_clock::time_point end;

public:
    void set()
    {
        start =	chrono::high_resolution_clock::now();
        return;
    }

    double stop(int scale = 1)
    {
        end = chrono::high_resolution_clock::now();
        chrono::duration<double, milli> duration = end - start;
        return duration.count()*scale;
    }
};
