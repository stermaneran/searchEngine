// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <sys/ioctl.h>
#include <fstream>
#include <signal.h>
#include <arpa/inet.h>

using namespace std;

#define PORT 5060

bool isclosed(int sock);
int sock = 0 ;
void sighandle(int sig)
{
    cout << "\nclosing conections..." << "\n";
    close(sock);
    cout << "bye bye..." << "\n";
    signal(SIGINT,SIG_DFL);
    raise(SIGINT);

}

int main(int argc, char const *argv[])
{
    signal(SIGINT,sighandle);

    struct sockaddr_in serv_addr;
    char buffer[65536] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << " \nSocket creation error " << '\n';
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        cout << " \nInvalid address/ Address not supported  " << '\n';
        return -1;
    }
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        cout << " \nConnection Failed" << '\n';
        return -1;
    }
    ofstream file;
    file.open("ans.html",ofstream::out );
    while(1)
    {
        cout << "ask Q now please:" << '\n';
        string question;
        cin >> question;

        if(isclosed(sock))
        {
            raise(SIGINT);
        }
        send(sock , question.c_str() , question.length() , 0 );
        cout << "question asked " << '\n';
        read( sock ,buffer, 4096);
        std::cout << buffer << "\n";
        int size =atoi(buffer);
        std::cout << size << "\n";
        send(sock , question.c_str() , question.length() , 0 );
        read( sock ,buffer, size);
        if(string(buffer).compare("close") != 0 && string(buffer).compare("404") != 0)
        {
            file.close();
            file.open("ans.html",ofstream::out |ios::trunc);
            file<<buffer;
            file.close();
            system("xdg-open ans.html");
        }
            else
            {
                if(string(buffer).compare("close") != 0)
                {
                        cout << "closing Connection" << '\n';
                        close(sock);
                        return 0;
                }

                else
                {
                    cout << "sorry word not found" << '\n';
                }
            }
        }
}

bool isclosed(int sock)
 {
  fd_set rfd;
  FD_ZERO(&rfd);
  FD_SET(sock, &rfd);
  timeval tv = { 0 };
  select(sock+1, &rfd, 0, 0, &tv);
  if (!FD_ISSET(sock, &rfd))
    return false;
  int n = 0;
  ioctl(sock, FIONREAD, &n);
  return n == 0;
}
