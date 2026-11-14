# redis-clone

A Redis-compatible server written in C from scratch, as a learning project.

## Features
- TCP server using raw POSIX sockets
- RESP protocol parser (handles partial reads and pipelined commands)
- Commands: PING, ECHO, SET, GET
- Hash table with chaining

## Build and run
    make
    ./redis_clone
    redis-cli set name <anything>   # in another terminal

## Roadmap
- [ ] DEL
- [ ] Multiple clients with poll()
- [ ] Key expiry (TTL)
