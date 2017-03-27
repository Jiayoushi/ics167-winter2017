# ICS167 (Multiplayer Game Systems)
Winter 2017 | Group 42

# Team Members

https://github.com/TTaiN

https://github.com/Jiayoushi

https://github.com/chayuso

https://github.com/suitensenka

# About
A networked Snake game developed for University of California, Irvine's ICS 167 (Multiplayer Game Systems) course. The game client is implemented using a combination of HTML, CSS, and JavaScript, and is a thin client. The game server is implemented in C++ and acts as an authorative centralized server. The client utilizes web sockets to communicate to the server. 

The game was implemented with artificial latency in order to provide an environment in which we could implement latency mitigation techniques to combat simulated network lag and congestion, and improve player experience. Such techniques used in this project included a variant of lockstep and frame interpolation. This repository acts as a portfolio for our work.

# Dependencies
-> C++11 JSON parser by dropbox: https://github.com/dropbox/json11
-> C++ socket implementation by ICS 167's Class TA.