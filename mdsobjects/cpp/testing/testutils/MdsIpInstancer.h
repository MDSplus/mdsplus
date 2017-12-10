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

#include <mdsobjects.h>

extern "C" int mdsip_main(int argc, char **argv);

namespace mds = MDSplus;

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
    unsigned short m_port;
    std::string m_protocol;

public:

    MdsIpInstancer(const char *protocol,unsigned short port) :
        m_port(port),
        m_protocol(protocol)
    {
        // build lazy singleton instance //
        m_host_file.get_instance();
        if (m_port>0) {
          // get first available port //
          int offset = 0;
          while(!available(m_port,m_protocol) && offset<100 )
            m_port += offset++;
          if(offset==100)
            throw std::out_of_range("any port found within 100 tries");
        }
        m_pid = fork();
        if(m_pid<0) {
            perror("unable to fork process\n");
            exit(1);
        }

        // child //
        if(m_pid == 0) {
            char port_str[20];
            char *_argv[] = {(char *)"mdsip",
                             (char *)"-P",(char *)m_protocol.c_str(),
                             (char *)"-h",(char *)m_host_file->name(),
                             (char *)"-p",port_str,(char *)"-m"};
            int _argc = 5;
            if (m_port>0) {
              sprintf(port_str,"%i",m_port);
              _argc += 3;
            }
            int status __attribute__ ((unused)) = mdsip_main(_argc,_argv);
            exit(1);
        }  else {
            std::cout << "started mdsip server for " << m_protocol << " on port: " << m_port << " pid: " << m_pid << "\n" << std::flush;
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
        //ss << "::1#" << m_port; TODO: test ip6 addresses
        if (m_protocol=="tcp" || m_protocol=="tcpv6")
          ss << "tcp://localhost:" << m_port;
        else if (m_protocol=="udt" || m_protocol=="udtv6")
          ss << "udt://localhost:" << m_port;
        else if (m_protocol=="gsi")
          ss << "gsi://localhost:" << m_port;
        else
          ss << m_protocol << "://localhost";
        return ss.str();
    }

    bool waitForServer(int retries = 5, int usec = 500000) const {
        if(m_pid > 0) { // only parent can wait //
            for(int retry = 0; retry<retries; ++retry) {
                try {
                    mds::Connection cnx((char *)this->getAddress().c_str());
                    (void)cnx;
                    return true; }
                catch (mds::MdsException &e) {
                    (void)e;
                    usleep(usec);
                }
            }
        }
        return false;
    }

private:


    // Allocate a new TCP server socket, and return
    // its handler
    int allocate(const std::string &protocol) {
        int sock = -1;

        if(protocol == "tcp" || protocol == "tcpv6" || protocol == "gsi")
            sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        else if(protocol == "udt" || protocol == "udtv6")
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

