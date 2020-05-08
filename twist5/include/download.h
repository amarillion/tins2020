#ifndef _DOWNLOAD_H_
#define _DOWNLOAD_H_

#include <string>
#include <memory>
#include "workerthread.h"

//TODO: re-write as Promise<Response>!
std::shared_ptr<WorkerThread> downloadWorker(const std::string &url, const std::string &fname);
std::shared_ptr<WorkerThread> postWorker(const std::string &url, const std::string &body);

#endif
