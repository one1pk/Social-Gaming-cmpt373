# Social Gaming

#### Install Dependecies
* Boost (>= 1.72)
    *  `sudo apt-get install libboost-all-dev`
* nlohmann json (>= 3.2.0)
    * `sudo apt-get install nlohmann-json3-dev `

### Build with CMake
1) Clone the repo
`git clone git@csil-git1.cs.surrey.sfu.ca:373-22-3-flaedle/social-gaming.git`
2) Create an out-of-source build directory
`mkdir social_gaming_build`
3) Move to the build directory
`cd social_gaming_build`
4) Run CMake with the path to the project
`cmake ../social-gaming`
5) Make the build
`make`

### Run
1) Start up the server: `./bin/gameserver <port> <html>`
where `<port>` is the port number the server will listen to,
and `<html>` the html that will be served on that port in response to an index.html request
2) Run clients 
    * In a terminal: `./bin/gameclient <ip/localhost> <port>` 
    where `<ip/localhost>` is the ip address of the server, or "localhost" if the server is run locally
    and `<port>` is the port number the server is listening to
    * In a browser: `http://localhost:<port>/index.html`
    where `<port>` is the port number the server is listening to
    * Temporarily, port = 4040

### Terminate
* To terminate a client
    *   leave game: `leave`
    *   exit server: `exit`
* To terminate the server enter `shutdown` from any client while in the lobby
    (this while change in the future)