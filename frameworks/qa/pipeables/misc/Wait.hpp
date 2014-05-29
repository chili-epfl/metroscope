/*
 * Wait.hpp
 *
 *  Created on: May 20, 2014
 *      Author: lprisan
 */

#ifndef WAIT_HPP_
#define WAIT_HPP_

#include <unistd.h>

class Wait: public Pipeable {
    public:
        Wait(const int millis):mMillis(millis) {}
    protected:
        void run() { usleep(mMillis*1000l); }
        int mMillis;
};



#endif /* WAIT_HPP_ */
