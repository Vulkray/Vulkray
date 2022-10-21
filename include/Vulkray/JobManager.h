/*
 * JobManager.h
 * API Header - Defines the JobManager class to run the user's code every frame.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
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
    void(*pFunction)(ShowBase *base); // all jobs must return void
};

class JobManager {
private:
    std::vector<JobCallback> jobCallbacks;
public:
    JobManager();
    ~JobManager();
    std::vector<JobCallback> _get_job_callbacks_vector();
    void set_job_callback(const char *jobName, void (*pFunction)(ShowBase *base));
    void remove_job_callback(const char *jobName);
};

#endif //VULKRAY_API_JOBMANAGER_H
