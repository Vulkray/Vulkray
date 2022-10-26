/*
 * JobManager.cxx
 * Defines the JobManager class to run the user's code every frame.
 *
 * VULKRAY ENGINE SOFTWARE
 * Copyright (c) 2022, Max Rodriguez. All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license. You should have received a copy of this license along
 * with this source code in a file named "COPYING."
 */

#include "../../include/Vulkray/JobManager.h"
#include <spdlog/spdlog.h>

JobManager::JobManager() {
    // placeholder
}

JobManager::~JobManager() {
    // placeholder
}

std::vector<JobCallback> JobManager::_get_job_callbacks_vector() {
    // note: this function was made to be used by the vulkan renderer module
    return this->jobCallbacks;
}

void JobManager::set_job_callback(const char *jobName, void (*pFunction)(ShowBase *base)) {
    JobCallback jobCallback;
    jobCallback.name = jobName;
    jobCallback.pFunction = pFunction;
    this->jobCallbacks.push_back(jobCallback); // push function pointer to jobs vector
}

void JobManager::remove_job_callback(const char *jobName) {
    int index = 0;
    for (JobCallback jobCallback : this->jobCallbacks) {
        if (jobCallback.name.compare(jobName)) { // find job with matching name ID
            this->jobCallbacks.erase(this->jobCallbacks.begin() + index);
        }
        index++;
    }
    spdlog::error("Could not remove job callback! Job identifier (name) didn't match any jobs.");
    throw std::runtime_error("Failed to remove job callback! Identifier not found.\n");
}