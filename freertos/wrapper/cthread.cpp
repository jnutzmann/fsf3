/****************************************************************************
 *
 *  Copyright (c) 2017, Michael Becker (michael.f.becker@gmail.com)
 *
 *  This file is part of the FreeRTOS Add-ons project.
 *
 *  Source Code:
 *  https://github.com/michaelbecker/freertos-addons
 *
 *  Project Page:
 *  http://michaelbecker.github.io/freertos-addons/
 *
 *  On-line Documentation:
 *  http://michaelbecker.github.io/freertos-addons/docs/html/index.html
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a
 *  copy of this software and associated documentation files
 *  (the "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so,subject to the
 *  following conditions:
 *
 *  + The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *  + Credit is appreciated, but not required, if you find this project
 *    useful enough to include in your application, product, device, etc.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *  OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ***************************************************************************/


#include <cstring>
#include "freertos/thread.hpp"


using namespace freertos;


volatile bool Thread::SchedulerActive = false;
MutexStandard Thread::StartGuardLock;


Thread::Thread( const std::string pcName,
                uint16_t usStackDepth,
                UBaseType_t uxPriority)
    :   Name(pcName), 
        StackDepth(usStackDepth), 
        Priority(uxPriority),
        ThreadStarted(false)
{
#if (INCLUDE_vTaskDelayUntil == 1)
    delayUntilInitialized = false;
#endif
}


Thread::Thread( uint16_t usStackDepth,
                UBaseType_t uxPriority)
    :   Name("Default"), 
        StackDepth(usStackDepth), 
        Priority(uxPriority),
        ThreadStarted(false)
{
#if (INCLUDE_vTaskDelayUntil == 1)
    delayUntilInitialized = false;
#endif
}

bool Thread::Start()
{
    //
    //  If the Scheduler is on, we need to lock before checking
    //  the ThreadStarted variable. We'll leverage the LockGuard 
    //  pattern, so we can create the guard and just forget it. 
    //  Leaving scope, including the return, will automatically 
    //  unlock it.
    //
    if (SchedulerActive) {

        LockGuard guard (StartGuardLock);

        if (ThreadStarted)
            return false;
        else 
            ThreadStarted = true;
    }
    //
    //  If the Scheduler isn't running, just check it.
    //
    else {

        if (ThreadStarted)
            return false;
        else 
            ThreadStarted = true;
    }


    BaseType_t rc = xTaskCreate(TaskFunctionAdapter,
                                Name.c_str(),
                                StackDepth,
                                this,
                                Priority,
                                &handle);


    return rc != pdPASS ? false : true;
}


#if (INCLUDE_vTaskDelete == 1)

//
//  Deliberately empty. If this is needed, it will be overloaded.
//
void Thread::Cleanup()
{
}


Thread::~Thread()
{
    vTaskDelete(handle);
    handle = (TaskHandle_t)-1;
}

#else

Thread::~Thread()
{
    configASSERT( ! "Cannot actually delete a thread object "
                    "if INCLUDE_vTaskDelete is not defined.");
}

#endif


void Thread::TaskFunctionAdapter(void *pvParameters)
{
    Thread *thread = static_cast<Thread *>(pvParameters);

    thread->Run();

#if (INCLUDE_vTaskDelete == 1)

    thread->Cleanup();

    vTaskDelete(thread->handle);

#else
    configASSERT( ! "Cannot return from a thread.run function "
                    "if INCLUDE_vTaskDelete is not defined.");
#endif
}


#if (INCLUDE_vTaskDelayUntil == 1)

void Thread::DelayUntil(const TickType_t Period)
{
    if (!delayUntilInitialized) {
        delayUntilInitialized = true;
        delayUntilPreviousWakeTime = xTaskGetTickCount();
    }

    vTaskDelayUntil(&delayUntilPreviousWakeTime, Period);
}


void Thread::ResetDelayUntil()
{
    delayUntilInitialized = false;
}

#endif



#ifdef CPP_FREERTOS_CONDITION_VARIABLES

bool Thread::Wait(  ConditionVariable &Cv,
                    Mutex &CvLock,
                    TickType_t Timeout)
{
    Cv.AddToWaitList(this);

    //
    //  Drop the associated external lock, as per cv semantics.
    //
    CvLock.Unlock();

    //
    //  And block on the internal semaphore. The associated Cv
    //  will call Thread::Signal, which will release the semaphore.
    //
    bool timed_out = ThreadWaitSem.Take(Timeout);
    
    //
    //  Grab the external lock again, as per cv semantics.
    //
    CvLock.Lock();

    return timed_out;
}


#endif


