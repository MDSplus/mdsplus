#ifndef TESTUTILS_MDSIPMAIN_H
#define TESTUTILS_MDSIPMAIN_H
#define USE_FORK // could also be set by CXXFLAGS=-DUSE_FORK

#ifdef _WIN32
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT _WIN32_WINNT_WIN8 // Windows 8.0
#include <winsock2.h>
#include <ws2tcpip.h>
#define REUSEADDR_TYPE BOOL
#else
#include <string.h>
#ifndef USE_FORK
#include <spawn.h>
#endif
#define SOCKET int
#define INVALID_SOCKET -1
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define REUSEADDR_TYPE int
#include <signal.h>
#include <errno.h>
#endif
#include <unistd.h>
#include <fstream>
#include <sys/types.h>
#include <testutils/Singleton.h>

#include <mdsobjects.h>

extern char **environ;

namespace mds = MDSplus;
namespace testing
{
  class MdsIpInstancer
  {
    struct HostFile
    {
      HostFile()
      {
        std::string hosts_default = "* | MAP_TO_LOCAL \n"
                                    "* | nobody \n";

        std::ofstream hosts_file;
        hosts_file.open("testing.hosts");
        hosts_file << hosts_default << "\n";
        hosts_file.close();
      }
      ~HostFile() { remove("testing.hosts"); }
      const char *name() const { return "testing.hosts"; }
    };

    Singleton<HostFile> m_host_file;
    unsigned short m_port;
    std::string m_protocol;
#ifdef _WIN32
    HANDLE m_process;
#else
    pid_t m_process;
#endif
  public:
    MdsIpInstancer(const char *protocol, unsigned short port, const char *mode)
        : m_port(port), m_protocol(protocol)
    {
      m_process = 0;
      if (port == 0)
        return;
      m_host_file.get_instance();
      // get first available port  //
      /*
    int offset = 0;
    while(!available(m_port,m_protocol) && offset<100 )
      m_port += offset++;
    if(offset==100)
      throw std::out_of_range("any port found within 100 tries");
    */
#ifdef _WIN32
      char *hostfile = (char *)m_host_file->name();
      size_t len = strlen(protocol) + strlen(hostfile) + 128;
      char *cmd = (char *)malloc(len);
      _snprintf_s(cmd, len, len - 1, "mdsip.exe %s -P %s -p %u -h %s", mode,
                  protocol, port, hostfile);
      fprintf(stdout, "spawning \"%s\"\n", cmd);
      PROCESS_INFORMATION piProcInfo;
      STARTUPINFO siStartInfo;
      memset(&siStartInfo, 0, sizeof(siStartInfo));
      siStartInfo.cb = sizeof(siStartInfo);
      siStartInfo.hStdError = GetStdHandle(STD_ERROR_HANDLE);
      siStartInfo.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
      siStartInfo.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
      siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
      BOOL bSuccess = CreateProcess(NULL, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW,
                                    NULL, NULL, &siStartInfo, &piProcInfo);
      free(cmd);
      if (bSuccess)
      {
        m_process = piProcInfo.hProcess;
        CloseHandle(piProcInfo.hThread);
        DWORD pid = piProcInfo.dwProcessId;
#else
      pid_t pid;
      char port_str[20];
      sprintf(port_str, "%i", m_port);
      char *argv[] = {(char *)"mdsip",
                      (char *)"-P",
                      (char *)m_protocol.c_str(),
                      (char *)"-h",
                      (char *)m_host_file->name(),
                      (char *)"-p",
                      port_str,
                      (char *)mode,
                      NULL};
#ifdef USE_FORK
      std::cout << "forking process!\n"
                << std::flush;
      pid = fork();
      if (pid == 0) // child process
                    //   exit(execvpe(argv[0], argv, environ));
        exit(execvp(argv[0], argv) ? errno : 0);
      else if (pid > 0)
      { // spawned ok
#else
      if (!posix_spawnp(&pid, argv[0], NULL, NULL, argv, environ))
      {
#endif
        m_process = pid;
#endif
        std::cout << "started mdsip server for " << m_protocol
                  << " on port: " << m_port << " pid: " << pid << "\n"
                  << std::flush;
      }
      else
        throw std::runtime_error("Could not start mdsip server");
    }

    ~MdsIpInstancer()
    {
      if (m_process)
      {
        std::cout << "removing mdsip for " << m_protocol << ".." << std::flush;
#ifdef _WIN32
        BOOL ok = TerminateProcess(m_process, 0);
        CloseHandle(m_process);
        if (ok)
#else
        if (!kill(m_process, SIGKILL))
#endif
          std::cout << "success\n"
                    << std::flush;
        else
          std::cout << "FAILED\n"
                    << std::flush;
      }
    }

    int getPort() const { return m_port; }

    std::string getAddress() const
    {
      std::stringstream ss;
      if (m_port == 0)
        ss << m_protocol << "://tunnel";
      else if (m_protocol == "tcp")
        ss << "tcp://127.0.0.1:" << m_port;
      else if (m_protocol == "udt")
        ss << "udt://127.0.0.1:" << m_port;
      else if (m_protocol == "tcpv6")
        ss << "tcpv6://::1#" << m_port;
      else if (m_protocol == "udtv6")
        ss << "udtv6://::1#" << m_port;
      else if (m_protocol == "gsi")
        ss << "gsi://localhost:" << m_port;
      else
        ss << m_protocol << "://localhost:" << m_port;
      return ss.str();
    }

    bool waitForServer(int retries = 5, int usec = 500000) const
    {
      if (m_process)
      { // only parent can wait //
        for (int retry = 0; retry < retries; ++retry)
        {
          try
          {
            mds::Connection cnx((char *)this->getAddress().c_str());
            (void)cnx;
            return true;
          }
          catch (mds::MdsException &e)
          {
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
    int allocate(const std::string &protocol)
    {
      SOCKET sock = INVALID_SOCKET;
      int addr, data, proto;
      if (protocol == "tcpv6" || protocol == "udtv6")
        addr = AF_INET6;
      else
        addr = AF_INET;
      if (protocol == "udt" || protocol == "udt6")
      {
        data = SOCK_DGRAM;
        proto = IPPROTO_UDP;
      }
      else
      {
        data = SOCK_DGRAM;
        proto = IPPROTO_UDP;
      }
      sock = socket(addr, data, proto);
      if (sock == INVALID_SOCKET)
      {
        if (errno == EMFILE)
        {
          /* too many open files */
          return 0;
        }
        fprintf(stderr, "error allocating socket '%s': ", protocol.c_str());
        perror("");
        exit(1);
      }
      REUSEADDR_TYPE optval = 1;
      setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
      return sock;
    }

    // Check whether the provided TCP port is available
    // at the moment and return 1 if it's avaiable, zero otherwise
    int available(int port, const std::string protocol)
    {
      int sock = allocate(protocol);
      int error;
      if (protocol == "tcpv6" || protocol == "udtv6")
      {
        const struct in6_addr in6addr_any = IN6ADDR_ANY_INIT;
        struct sockaddr_in6 addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin6_family = AF_INET6;
        addr.sin6_port = htons(port);
        addr.sin6_addr = in6addr_any;
        error = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
      }
      else
      {
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        error = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
      }
      if (!error)
      {
        shutdown(sock, 2);
        close(sock);
      }
      return error == 0;
    }
  };
} // namespace testing

#endif // MDSIPMAIN_H
