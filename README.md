# Pipes
## About
Pipes is a chat application with it's backend written fully in C, it's frontend written in JavaScript, CSS, and HTML, aswell as WebAssembly written in C. The purpose of Pipes is to be a faster and more secure alternative to applications like Discord, with more lighter rules.
## Building & running
### Requirements
Pipes *requires* to be ran on a UNIX-like server. This means that it can run on Linux, *BSD, and many other UNIX-like operating systems. The minimum amount of RAM needed is 32MiB, although recommended is 8GiB. You'll also need a C compiler & another C compiler that targets WebAssembly.
### Installing dependencies
You'll need to install GCC and Emscripten. To do this using Ubuntu or Debian package manager, you can run the following commands:
```
apt install gcc emscripten make
```
Then, you'll need to install the libraries:
```
apt install libjansson-dev libwebsockets-dev
```
You'll also need to install the MariaDB driver:
```
apt install libmariadb-dev
```
### Building
After all of this, you can build the server (backend):
```
make
```
### Testing
You can then test the server (backend):
```
./pipes
```
## License
This repository has no license (for now), therefore all rights are reserved to me (Dashbloxx).
