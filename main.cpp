#include <iostream>
#include <string>
#include <cstring>
#include <wait.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <fcntl.h>

using namespace std;

/*
 *  Name: Edgar Navarro, Josh Brown
 *  Username: cssc0831
 *  Class Info: CS 480
 *  Assigment: Programming Assignment #2
 *  Filename: main.cpp
 */

void myhandler(int signum);

void msh_readline();

// Handles exit
void msh_exit() {
    exit(0);
}

void msh_parse_line(char *line) {
    // Struct used for use in stat() to check for executable
    struct stat args;

    // Create a vector with all the input from user.
    char *segment = strtok(line, " |");
    vector<string> input;
    while (segment != NULL) {
        input.push_back(segment);
        segment = strtok(NULL, " |");
    }


    // condition check
    // 1: a single executable
    //   * run the executable
    // 2: executable and an argument
    //   * run the executable and pass it the argument
    // 3: n amount of executables are passed
    //   * create n amount of processes with n-1 amount of pipes

    // Loop only terminates if 'exit' is inputted and asks for input from user
    while (true) {
        // Keeps count of number of individual commands inputted and number of pipes needed
        int num_cmd = input.size();
        int num_pipes = num_cmd - 1;

        // Checks if anything is inputted if not we ask for help
        if (input.size() < 1){
            cout << "Input not valid. For Help enter 'Help'";
            msh_readline();
            break;
        }

        // Checks if 'exit' is inputted then exits program if it was
        if (input.size() == 1 && input.at(0) == "exit") {
            msh_exit();
            break;
        }

        // Extra feature implemented to give helpful information to user
        if (input.size() == 1 && input.at(0) == "Help") {
            cout << "Commands must be of the following format:"<<endl;
            cout << "executable/file/path" << endl;
            cout << "executable/file/path argument" << endl;
            cout << "executable/file/path | executable/file/path | ...";
            msh_readline();
            break;
        }

        // if our first argument is an executable status is set to 0
        // status = if first thing is an executable
        // status2 = if second thing is an executable
        int status = stat(input.at(0).c_str(), &args);
        int status2;

        // Checks if our first input is not an executable since we already checked for other conditions
        if (status != 0){
            cout << "Input not valid. For Help enter 'Help'";
            msh_readline();
            break;
        }

        // Checks if a second command is inputted and determines if it's an argument or executable
        if (input.size() > 1) {
            status2 = stat(input.at(1).c_str(), &args);
        }

        // pid1 keeps track of a single executable
        pid_t pid1;

        // If we only have 1 executable inputted
        if (input.size() == 1 && status == 0) {
            // only executable
            pid1 = fork();
            if (pid1 == 0) {
                execl(input.at(0).c_str(), NULL);
            }
            // We wait for our other processes to finish
            for (;;) {
                pid_t tmp;
                tmp = wait(NULL);
                if (tmp == pid1) {
                    break;
                }
            }
        } else if (input.size() == 2 && status == 0 && status2 != 0) {
            // executable and an argument
            pid1 = fork();
            if (pid1 == 0) {
                execl(input.at(0).c_str(), input.at(1).c_str());
            }
            // We wait for our other processes to finish
            for (;;) {
                pid_t tmp;
                tmp = wait(NULL);
                if (tmp == pid1) {
                    break;
                }
            }
        } else {
            // We loop through all commands inputted
            for (int c = 0; c < num_cmd; c++){
                // We check if every command is an executable
                if(stat(input.at(c).c_str(), &args) != 0){
                    cout << "One or more of your files cannot be executed or does not exist" << endl;
                    cout << "Please try again";
                    msh_readline();
                    break;
                }
            }
            // We create our file descriptors and our pid array, as well as some other variables
            int fd[num_pipes][2];
            pid_t pid[num_cmd];
            int i, k, lm;
            // Create our pipes
            for (i = 0; i < num_pipes; i++) {
                pipe(fd[i]);
            }
            // Loop through each command inputted
            for (k = 0; k < num_cmd; k++) {
                pid[k] = fork();
                if (pid[k] == 0) {
                    // This runs the first command inputted
                    if (k == 0) {
                        // Closes process if termination request sent
                        signal(SIGTERM, myhandler);
                        // opens out pipe
                        dup2(fd[k][1],STDOUT_FILENO);
                        // Closes all file descriptors
                        for (lm = 0; lm < num_pipes; lm++) {
                            close(fd[lm][0]);
                            close(fd[lm][1]);
                        }
                        // executes command
                        execvp(input[k].c_str(), NULL);
                    // This runs the last command inputted
                    } else if (k == num_cmd - 1) {
                        // Closes process if termination request sent
                        signal(SIGTERM, myhandler);
                        // opens in pipe
                        dup2(fd[k - 1][0],STDIN_FILENO);
                        // Closes all file descriptors
                        for (lm = 0; lm < num_pipes; lm++) {
                            close(fd[lm][0]);
                            close(fd[lm][1]);
                        }
                        // executes command
                        execvp(input[k].c_str(), NULL);
                    // This runs commands in the middle
                    } else {
                        // Closes process if termination request sent
                        signal(SIGTERM, myhandler);
                        // Opens both in and out pipes
                        dup2(fd[k - 1][0], STDIN_FILENO);
                        dup2(fd[k][1], STDOUT_FILENO);
                        // Closes all file descriptors
                        for (lm = 0; lm < num_pipes; lm++) {
                            close(fd[lm][0]);
                            close(fd[lm][1]);
                        }
                        // executes command
                        execvp(input[k].c_str(), NULL);
                    }
                }
            }
            // Closes all file descriptors
            for (k = 0; k < num_pipes; k++) {
                close(fd[k][0]);
                close(fd[k][1]);
            }
            // Waits for processes
            for (;;) {
                pid_t tmp;
                tmp = wait(NULL);
                if (tmp == pid[num_cmd - 1]) {
                    break;
                }
            }
        }
        // Asks for new input from user
        msh_readline();
        break;
    }
}

// Used for Signal() to exit process
void myhandler(int signum) {
    exit(signum);
}

// Asks user for input
void msh_readline() {
    cout << "\ncssc0805% " << flush;
    char commands[1000];
    cin.getline(commands, 1000, '\n');
    msh_parse_line(commands);
}

// Driver
int main() {
    msh_readline();
}