/*******************************************************************************
*   Copyright 2013 EPFL                                                        *
*                                                                              *
*   This file is part of metroscope.                                           *
*                                                                              *
*   Metroscope is free software: you can redistribute it and/or modify         *
*   it under the terms of the GNU General Public License as                    *
*   published by the Free Software Foundation, either version 3 of the         *
*   License, or (at your option) any later version.                            *
*                                                                              *
*   Metroscope is distributed in the hope that it will be useful,              *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*   GNU General Public License for more details.                               *
*                                                                              *
*   You should have received a copy of the GNU General Public License          *
*   along with Metroscope.  If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************/

#include "../../frameworks/qa/pipeables/Pipeable.hpp"
#include <iostream>
#include <sys/time.h>
#include <unistd.h>

// A minimal Pipeable, printing a given message.
// A Pipeable needs to inherit from the Pipeable abstract class
// and implement its run() method.
class Print: public Pipeable {

    public:
        Print(const char *message):mMessage(message) {}

    protected:
        void run() { std::cout << mMessage << std::endl; }
        const char *mMessage;
};

// Another Pipeable, generating data. The data is simply a number that gets
// incremented at every run of the pipeline.
class Counter: public Pipeable {
    public:
        Counter():mCount(0) {}
        const int *GetCount(){ return &mCount; }
    protected:
        void run() { ++mCount; }
        int mCount;
};

// Another Pipeable, using data. The principle of a Pipeable is that it can be
// run without giving it parameters at every call. Of course, it will need
// access to variables. To do so, it can store references to external data.
// In this case, we want to print a variable integer, whose reference we give
// at construction time.
class ShowCount: public Pipeable {

    public:
        ShowCount(const int *count):mCount(count) {}

    protected:
        void run() { std::cout << *mCount << std::endl; }
        const int *mCount;
};

// One more Pipeable, stalling the pipeline for a given time
class Wait: public Pipeable {
    public:
        Wait(const int millis):mMillis(millis) {}
    protected:
        void run() { usleep(mMillis*1000l); }
        int mMillis;
};


int main(int, char*[])
{
    // We start with a simple pipeline, displaying 3 messages:
    Print tPrint1("I'm first");
    Print tPrint2("I'm second");
    Print tPrint3("I'm third");
    // The | operator is used to set which Pipeable follows another
    tPrint1 | tPrint2 | tPrint3;
    // The start() method calls run() on the first Pipeable, and the following ones. The first Pipeable is thus used to start the pipeline.
    tPrint1.start();
    //We should see:
    //I'm first
    //I'm second
    //I'm third

    // Now let's see how to pass data between elements of the pipeline.

    // This Pipeable will produce data:
    Counter tCounter;
    // And this one will use it:
    ShowCount tShowCount(tCounter.GetCount());

    // We combine them
    tCounter | tShowCount;
    // and run the pipeline 5 times
    for (int i=0; i<5; ++i) tCounter.start();
    //We should see:
    //1
    //2
    //3
    //4
    //5

    // Let's complicate things a bit. We will use the previous
    // pipelines, add a delay, and loop them for ever.
    // Then we will run them concurrently.

    // First, we create additional Pipeables for the delays.
    Wait tWait1(1000);
    Wait tWait2(300);
    // We add it at the end of the previous pipelines.
    tPrint1 | tPrint2 | tPrint3 | tWait1;
    tCounter | tShowCount | tWait2;
    // For the sake of clarity, we repeated the whole pipeline, but 
    // tPrint3 | tWait1 and tShowCount | tWait2 whould have been
    // enough to tell that tWait* follow the last Pipeable of the
    // pipelines

    // We create a loop by appending the firt Pipeable to the last
    // Again, we repeat the whole pipeline for clarity
    tPrint1 | tPrint2 | tPrint3 | tWait1 | tPrint1;
    tCounter | tShowCount | tWait2 | tCounter;
    
    // Now we start the counting pipeline asynchronously
    tCounter.startNoWait();
    // And the other pipeline synchronously
    tPrint1.start();

    // tPrint1 will never stop, but if it did, we could use the
    // following to stop and terminate the background pipeline.
    tCounter.stop();
    tCounter.join();

	return 0;
}
