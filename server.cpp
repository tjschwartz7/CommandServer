#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <chrono>

#include <string>
#include <stdlib.h>
#include "access_schema.h"

//We're running on a windows machine but the DOCKER environment
//Uses a flavor of LINUX... so we'll use LINUX specific libraries
#define PORT 54000

/*
INSTRUCTIONS:
Make sure this is hosted with Host Port 54000. 

NOTICE:
Client will be searching for a server hosted on localhost:54000
Environment is a linux docker container. Thats how we get away with
using all of these linux includes.
*/

std::string CheckAccess(std::string message, SecureAccess& schema);
std::string EditAccess(std::string message, SecureAccess& schema, bool addAccess);

int main() 
{
    SecureAccess schema(10000);
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 54000
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 54000
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) 
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }


    if (listen(server_fd, 3) < 0) 
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Listening for connection on port " << PORT << std::endl;
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) 
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    //We have a session
    bool done = false;
    while(!done)
    {
        valread = read( new_socket , buffer, 1024);

        std::string client_message = buffer;
        std::string send_buf = "Invalid argument. Please try again.";
        int index = client_message.find_first_of(" ");
        std::string word = client_message.substr(0, index);
        client_message = client_message.substr(index+1);

        //Not end of string, and the space isnt the final character in our string
        //All commands in this block have arguments
        bool commandHasArgs = index != std::string::npos && index != client_message.length() - 1;
        if(commandHasArgs)
        {
            bool addAccess = false;
            
            //If we're adding access, we want to pass 'true' along in our EditAccess argument
            if(word == "AddAccess") addAccess = true;
            
            //Edit Access case
            if(word == "AddAccess" ||
               word == "RemoveAccess")
            {
                send_buf = EditAccess(client_message, schema, addAccess);
                
            }
            else if(word == "CheckAccess")
            {
                send_buf = CheckAccess(client_message, schema);
            }
        }
        else if(word == "Quit")
        {
            send_buf = "Terminating program.\n";
            done = true;
        }
        
        std::cout << send_buf << std::endl;

        send(new_socket , send_buf.c_str() , send_buf.length() , 0 );
        std::cout << "Message sent to client." << std::endl;
    }
    return 0;
}


std::string CheckAccess(std::string message, SecureAccess& schema)
{
    int user;
    int group;
    std::string word;
    std::string ret_string = "";
    //Remove the first word from client_message
    

    //Find index of next space
    int index = message.find_first_of(" ");
    if(index != std::string::npos && index != message.length() - 1)
    {
        //Next word should be an integer
        word = message.substr(0, index);
        int conversion = std::stoi(word);

        //Checking to make sure stoi didn't return zero due to invalid input
        //We're expecting the user number
        if(std::to_string(conversion) == word)
        {
            //User number is this conversion
            user = conversion;
            //Remove last word from client_message
            message = message.substr(index+1);
            index = message.find_first_of(" ");

            //Next word should be another integer - the group number
            word = message.substr(0, index);
            conversion = std::stoi(word);

            //We're expecting the group number
            //If we convert the number back to a string and it equals word...
            //It was a valid input
            if(std::to_string(conversion) == word)
            {
                group = conversion;
                bool hasAccess = schema.UserHasAccess(user, group);
                if(hasAccess)
                    ret_string = "User " + std::to_string(user) + " has access to group " + std::to_string(group) + "\n";
                else 
                    ret_string = "User " + std::to_string(user) + " does not have access to group " + std::to_string(group) + "\n";
            }   
            
        }
    }           
    return ret_string;
}

std::string EditAccess(std::string message, SecureAccess& schema, bool addAccess)
{
    int user;
    int group;
    std::string word;
    std::string ret_string = "";
    //Remove the first word from client_message

    //Find index of next space
    int index = message.find_first_of(" ");
    if(index != std::string::npos && index != message.length() - 1)
    {
        //Next word should be an integer
        word = message.substr(0, index);
        int conversion = std::stoi(word);

        //Checking to make sure stoi didn't return zero due to invalid input
        //We're expecting the user number
        if(std::to_string(conversion) == word)
        {
            //User number is this conversion
            user = conversion;
            //Remove last word from client_message
            message = message.substr(index+1);
            index = message.find_first_of(" ");

            //Next word should be another integer - the group number
            word = message.substr(0, index);
            conversion = std::stoi(word);

            //We're expecting the group number
            //If we convert the number back to a string and it equals word...
            //It was a valid input
            if(std::to_string(conversion) == word)
            {
                group = conversion;
                schema.EditAccess(user, group, addAccess);
                if(addAccess)
                    ret_string = "User " + std::to_string(user) + " has been granted access to group " + std::to_string(group) + "\n";
                else 
                    ret_string = "User " + std::to_string(user) + " has lost access to group " + std::to_string(group) + "\n";
            }   
        }
    }
    return ret_string;
}