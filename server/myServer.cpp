// Server side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <sys/ioctl.h>
#include <fstream>
#include <signal.h>
#include "../Crawler/Common.h"//shay
#include "../Crawler/Crawler.h"//shay
#include "../Converter/convert.cpp"//damir
#include "../htmlBuilder/htmlbuilder.cpp"//damir


using namespace std;

# define PORT 5060

extern pthread_mutex_t _rawPagesOfsMutex;
pthread_mutex_t _eransMutex=PTHREAD_MUTEX_INITIALIZER;
string getAns(char* buff);
static int connFd;

void* damir(int s);
void *newconect(void *);
bool isclosed(int sock);
void startSpider();

int server_fd, new_socket;
int threadNumForSockets=0;

void sighandle(int sig)
{
    cout << "\nclosing conections..." << "\n";
    close(connFd);
    cout << "bye bye..." << "\n";
    signal(SIGINT,SIG_DFL);
    raise(SIGINT);

}
void* startSpider(void* args)
{
    int threadNum=*(int*)args;
    if(threadNum <= 0) {
                                    cerr << "numThreads <= 0 error" << endl;
                                    exit(1);
    }

    ifstream urlSeedIfs(URL_PATH.c_str());
    if(!urlSeedIfs) {
                                    cerr << "open file " << URL_PATH << " error" << endl;
                                    exit(1);
    }

    ofstream rawPagesOfs(RAW_PAGES_PATH.c_str());
    if(!rawPagesOfs) {
                                    cerr << "open file " << RAW_PAGES_PATH << " error" << endl;
                                    exit(1);
    }

    Crawler crawler(urlSeedIfs, rawPagesOfs, threadNum);
    crawler.multiPthreadCrawl();
    return 0;
}

void* damir(void* args)
{
    //sleep(10);
    Convert con("../Crawler/data/Words.data");

    while(true)
    {

        pthread_mutex_lock(&_rawPagesOfsMutex);
        con.write();
        pthread_mutex_unlock(&_rawPagesOfsMutex);
    }

    return 0;
}
int main(int argc, char const *argv[])
    {


        signal(SIGINT,sighandle);

        int threadNum=atoi(argv[1]);
        pthread_t t;
        pthread_create(&t,NULL,startSpider,&threadNum);


        struct sockaddr_in serverAddress,clientAddress;
        int opt = 1;
        socklen_t len = sizeof(clientAddress);
        pthread_t threadA[4];//we pick

        // Creating socket file descriptor
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                      &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = INADDR_ANY;
        serverAddress.sin_port = htons( PORT );

        // Forcefully attaching socket to the port 5060
        if (bind(server_fd, (struct sockaddr *)&serverAddress,
                                     sizeof(serverAddress))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 5) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        cout << "server runnng and waiting for conections..." << "\n";


        pthread_t t2;
        pthread_create(&t2,NULL,damir,NULL);



        while (1)
        {
            cout << "Listening" << endl;

            //this is where client connects. svr will hang in this mode until client conn
            connFd = accept(server_fd, (struct sockaddr *)&serverAddress, &len);
            if (connFd < 0 )
            {
                cerr << "Cannot accept connection" << endl;
                return 0;
            }
            else
            {
                cout << "Connection successful" << endl;
                pthread_create(&threadA[threadNumForSockets], NULL, newconect, &connFd);
            }
        }

        for(int i = 0; i < 3; i++)
        {
            pthread_join(threadA[i], NULL);
        }
        return 0;
    }
    void* newconect(void * sock)
    {
        int s=(*(int*) sock);
        cout << "new conection sock "<<s << '\n';
        //cout << "Thread No: " << pthread_self() << endl;
        while (!isclosed(s))
         {
            char buffer[1024] = {0};
            read( s , buffer, 1024);
            cout << "server recived:"<< string(buffer) << '\n';
            string ans=getAns(buffer);
                if(sizeof(ans)==0 || isclosed(s))
                {
                    cout << "closing conection on socket: "<<s << '\n';
                    close(s);
                    return 0;
                }
                else
                {
                    int size=ans.length();
                    char buffffer[1024];
                    //string length;
                    sprintf(buffffer,"%d",size);
                    std::cout << buffffer << "\n";
                    send(s , buffffer , 1024 , 0 );
                    read( s , buffer, 1024);
                    send(s , ans.c_str() , size , 0 );
                    cout << "answer sent "<<s << '\n';
               }
           }

           cout << "closing conection on socket: "<<s << '\n';
           close(s);
           return 0;
    }

string getAns(char* buff)
{
    Builder b((string)buff);
    b.build();
    string ans=b.ans.str();
    std::cout << ans << "\n";
    return ans;
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
