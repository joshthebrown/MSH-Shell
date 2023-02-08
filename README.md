
#### Header

* Name: Edgar Navarro, Josh Brown
* Username: cssc0831 ,cssc0805
* Class Info: CS 480
* Assigment: Programming Assignment #2
* Filename: README.md

#### File Manifest

* main.cpp
* Makefile
* README.md

#### Compile Instructions

* g++

#### Operating Instructions

* Our script works similar to a linux/bash script, ours is scaled down to accept the following
    I. A single executable file to be ran by MSH
    II. A single executable followed by an argument to be ran by MSH
    III. 'n' amount of executables connected by 'n-1' amount of pipes

#### Design Decisions. 

* An infinite loop is declared in our msh_parse_line() function to always wait for input from the user, at least until they type in "exit". We use a 2d file descriptor array and a pid array to handle the case of n amount executables with n-1 amount of pipes, as trying to assign a file descriptor for each would be tedious and ineffecient.

#### Extra Features

* If the user types in "help" they will be shown all acceptable input forms in the msh shell

#### Deficiencies/Bugs

MAKEFILE DOES NOT WORK COMPILE MANUALLY

</p> While the final version of msh works as expected, our greatest obstacle was getting mulitple processes to pipe to one another. Once we learned about the array implementation of holding multipe file descriptors, we were able to get our script working properly

#### Lessons Learned

</p> Through this program we learned a lot about processes and pipes in C/C++ and their uses. We also gained valuable insight into how a shell script operates.

#### Partner Roles

</p> Edgar Navarro: built msh_readline()  built the help function, helped on other functions

     Josh Brown: built main(), handler() and exit functions, helped out on all other fucntions