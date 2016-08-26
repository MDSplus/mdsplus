#ifndef TESTUTILS_MDSIPMAIN_H
#define TESTUTILS_MDSIPMAIN_H

#include <string>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#include <testutils/Singleton.h>

extern "C" int mdsip_main(int argc, char **argv);



namespace testing {
class MdsIpInstancer {
        
    struct HostFile {        
        HostFile() {
            std::string hosts_default = 
                    "/O=Grid/O=National Fusion Collaboratory/OU=MIT/CN=Thomas W. Fredian/Email=twf@psfc.mit.edu | twf \n"
                    "* | MAP_TO_LOCAL \n"
                    "* | nobody \n";
            
            std::ofstream hosts_file;
            hosts_file.open("testing.hosts");
            hosts_file << hosts_default << "\n";
            hosts_file.close();
        }
        ~HostFile() {
            remove("testing.hosts");
        }
        const char *name() const { return "testing.hosts"; }
    };    
    
    Singleton<HostFile> m_host_file;
    pid_t m_pid;
    int m_port;
    std::string m_protocol;
    
public:
    
    MdsIpInstancer(const char *protocol) :
        m_port(8000),
        m_protocol(protocol)
    {
        // build lazy singleton instance //
        m_host_file.get_instance();

        // get first available port //        
        m_port += getpid()%1000;
        int offset = 0;
        while(!available(m_port,m_protocol) && offset<10 ) {
            m_port += offset++ * 1000;
        }
        if(offset==10) 
            throw std::out_of_range("any port found within 10 tries");
        
        
        m_pid = fork();
        if(m_pid<0) {
            perror("unable to fork process\n");
            exit(1);
        }
        
        // child //
        if(m_pid == 0) {                          
            char port_str[20];
            sprintf(port_str,"%i",m_port);            
            int _argc = 8;
            char *_argv[] = {(char *)"mdsip",
                             (char *)"-m",
                             (char *)"-P",(char *)m_protocol.c_str(),
                             (char *)"-p",(char *)port_str,
                             (char *)"-h",(char *)m_host_file->name() };
            int status __attribute__ ((unused)) = mdsip_main(_argc,_argv);
            exit(1);
        }
        else {
            std::cout << "started mdsip server on port: " << m_port << " pid: " << m_pid << "\n" << std::flush;
        }
            
    }
    
    ~MdsIpInstancer() {
        if(m_pid>0) {
            std::cout << "removing mdsip for " << m_protocol << "\n" << std::flush;
            kill(m_pid,SIGKILL);
        }
    }
    
    int getPort() const { return m_port; }
    
    std::string getAddress() const {
        std::stringstream ss;
        ss << m_protocol << "://" << "localhost" << ":" << m_port;
        return ss.str();
    }
    
private:
    
    
    // Allocate a new TCP server socket, and return
    // its handler
    int allocate(const std::string &protocol) {
        int sock = -1;
        
        if(protocol == "tcp")
            sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        else if(protocol == "udt")
            sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (sock < 0) {
            if (errno == EMFILE) {
                /* too many open files */
                return 0;
            }
            perror("error allocating socket\n");
            exit(1);
        }
        int optval = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
        return sock;
    }
    
    // Check whether the provided TCP port is available
    // at the moment and return 1 if it's avaiable, zero otherwise
    int available(int port, const std::string protocol) {
        int sock = allocate(protocol);
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY); //inet_addr("0.0.0.0")
        int error = bind(sock, (struct sockaddr*) &addr, sizeof(addr));
        if(!error) {
            shutdown(sock,2);
            close(sock);
        }
        return error == 0;
    }
    
 
};
} // testing

#endif // MDSIPMAIN_H

