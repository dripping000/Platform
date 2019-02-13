//////////////////////////////////////////////////////////////////////////////
// c_timer.h
// =======
// High Resolution Timer.
// This timer is able to measure the elapsed time with 1 micro-second accuracy
// in both Windows, Linux and Unix system 
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2003-01-13
// UPDATED: 2006-01-13
//
// Copyright (c) 2003 Song Ho Ahn
//////////////////////////////////////////////////////////////////////////////

#pragma once

#define WIN
#ifdef WIN   // Windows system specific
#include <windows.h>
#else        // Unix based system specific
#include <sys/time.h>
#endif

class CTimer
{
public:
    CTimer();
    ~CTimer();

    //QT信号
    //QT槽

    //公有变量
    //公有函数
public:
    void start();                                   // start timer
    void stop();                                    // stop the timer
    double getElapsedTime();                        // get elapsed time in second
    double getElapsedTimeInSec();                   // get elapsed time in second (same as getElapsedTime)
    double getElapsedTimeInMilliSec();              // get elapsed time in milli-second
    double getElapsedTimeInMicroSec();              // get elapsed time in micro-second

    //私有变量
    double startTimeInMicroSec;                     // starting time in micro-second
    double endTimeInMicroSec;                      // ending time in micro-second
    int stopped;                                    // stop flag 
#ifdef WIN
    LARGE_INTEGER frequency;                        // ticks per second
    LARGE_INTEGER startCount;                       //
    LARGE_INTEGER endCount;                         //
#else
    timeval startCount;                             //
    timeval endCount;                               //
#endif
    //私有函数

    //保护函数和变量
};

