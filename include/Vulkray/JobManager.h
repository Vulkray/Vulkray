/*
 * JobManager.h
 * API Header - Defines the JobManager class to run the user's code every frame.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2023, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#ifndef VULKRAY_API_JOBMANAGER_H
#define VULKRAY_API_JOBMANAGER_H

#include <string>
#include <vector>

class ShowBase; // prototype ShowBase class

struct JobCallback {
    std::string name;
    void *caller; // pointer to class that created job callback
    void(*pFunction)(void *caller, ShowBase *base); // all jobs must return void
};

class JobManager {
private:
    std::vector<JobCallback> jobCallbacks;
public:
    JobManager();
    ~JobManager();
    std::vector<JobCallback> _get_job_callbacks_vector();
    void new_job(const char *jobName, void *caller, void (*pFunction)(void *caller, ShowBase *base));
    void remove_job(const char *jobName);
};

#endif //VULKRAY_API_JOBMANAGER_H
