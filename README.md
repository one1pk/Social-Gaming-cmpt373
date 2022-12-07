## Iteration 3 - Notes
* The game config and rules are now fully interpreted from json into a game object
* Complex strings from json such as 'when conditions' and deep lists (eg.- variables.winners, players.weapon, etc) are built into an expression tree in the interpreter and resolved dynamically, using a visitor, when the rule executes
* Owner can now conifgure setup
* Rock, Paper, Scissors works perfectly
* Any json file which uses the rules used in Rock, Paper, Scissors, can be interpreted and turned into a playable game without having to do anything extra
* Trivia game was done towards the end and has new rules so it is a bit flaky, but it demonstrates that interpretation, adding rules and game running is trivial at this point since we have a solid base structure.

# Social Gaming

#### Install Dependecies
* Boost (>= 1.72)
    *  `sudo apt-get install libboost-all-dev`
* nlohmann json (>= 3.2.0)
    * `sudo apt-get install nlohmann-json3-dev`
* GLog (>= 0.4.0)
    * `sudo apt-get install libgoogle-glog-dev`

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

### Terminate
* To terminate a client
    *   leave game: `leave`
    *   exit server: `exit`
* To terminate the server, press Ctrl+C
### Run Tests
Run `./bin/test/runAllTests` in the build directory
