## Value of this Project
This Project intended to build an load balancer to exercise some low-level detail including http request handling, common http caching technique, multi-thread programming, socket programming, good C++ practice, e.g. RAII.

## How to Start the Project
Every build detail is containerized into docker; thus, just `sudo docker-compose up`

## Features
* Supported HTTP Request GET, POST, and CONNECT
* Implemented HTTP caching policy including rules of expiration time, re-validation with LRU cache
* Handled asynchronized request through multi-threaded strategy, and use mutex to handling race condition issue.
* Use good C++ pracice e.g. RAII for resource management
* Containerized with docker for easy build in different machine