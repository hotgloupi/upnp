#include <natpmp.h>

#include <stdexcept>

struct NATPMP
{
private:
  natpmp_t  _natpmp;

public:
  NATPMP()
  {
    if (::initnatpmp(&_natpmp, 0, 0))
      throw std::runtime_error("couldn't initialize natpmp library");

    int res = sendpublicaddressrequest(&_natpmp);
    if (res != 2)
      throw std::runtime_error("couldn't request public address");
    printf("Retreive external ip address\n");
    natpmpresp_t response;
    res = _wait_response(&response);
    if (res != 0)
      throw std::runtime_error(
          "couln't retreive the public address: " + _get_error_string(res)
      );
  }

  void
  tcp_port_mapping(uint16_t private_port,
                   uint16_t public_port,
                   uint32_t lifetime = 3600)
  {
    printf("Request a tcp port mapping from %d (public) to %d (private) ports",
           public_port,
           private_port);
    _port_mapping(NATPMP_PROTOCOL_TCP, private_port, public_port, lifetime);
  }

  void
  udp_port_mapping(uint16_t private_port,
                   uint16_t public_port,
                   uint32_t lifetime = 3600)
  {
    printf("Request an udp port mapping from %d (public) to %d (private) ports",
           public_port,
           private_port);
    _port_mapping(NATPMP_PROTOCOL_UDP, private_port, public_port, lifetime);
  }

private:
  int
  _wait_response(natpmpresp_t* response)
  {
    int retry = NATPMP_TRYAGAIN;
    while (retry == NATPMP_TRYAGAIN)
    {

      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(_natpmp.s, &fds);
      int res;

      {
          struct timeval timeout;
          if (::getnatpmprequesttimeout(&_natpmp, &timeout))
              throw std::runtime_error("couldn't request natpmp timeout");
          printf("Checking socket fd\n");
          res = ::select(_natpmp.s + 1, &fds, NULL, NULL, &timeout);
      }

      if (res < 0)
        throw std::runtime_error("couldn't check socket fd");
      retry = ::readnatpmpresponseorretry(&_natpmp, response);
      if (retry == NATPMP_TRYAGAIN)
        printf("Trying again\n");
    }
    return retry;
  }

  void
  _port_mapping(int protocol,
                uint16_t private_port,
                uint16_t public_port,
                uint32_t lifetime)
  {
    auto error = ::sendnewportmappingrequest(
        &_natpmp, protocol, private_port, public_port, lifetime
    );
    if (error != 12)
      throw std::runtime_error("couldn't send new port mapping request");

    natpmpresp_t response;
    int res = _wait_response(&response);
    if (res != 0)
      throw std::runtime_error(
          "couldn't read natpmp response: " + _get_error_string(res)
      );
    printf("mapped public port %hu to localport %hu liftime %u\n",
           response.pnu.newportmapping.mappedpublicport,
           response.pnu.newportmapping.privateport,
           response.pnu.newportmapping.lifetime);
  }

  static
  std::string
  _get_error_string(int retval)
  {
    switch (retval)
    {
    case NATPMP_TRYAGAIN:
      return "Too much tries";

    case NATPMP_ERR_INVALIDARGS:
      return "NATPMP_ERR_INVALIDARGS : invalid arguments passed to the function";
    case NATPMP_ERR_SOCKETERROR:
      return "NATPMP_ERR_SOCKETERROR : socket() failed. check errno for details";
    case NATPMP_ERR_CANNOTGETGATEWAY:
      return "NATPMP_ERR_CANNOTGETGATEWAY : can't get default gateway IP";
    case NATPMP_ERR_CLOSEERR:
      return "NATPMP_ERR_CLOSEERR : close() failed. check errno for details";
    case NATPMP_ERR_RECVFROM:
      return "NATPMP_ERR_RECVFROM : recvfrom() failed. check errno for details";
    case NATPMP_ERR_NOPENDINGREQ:
      return "NATPMP_ERR_NOPENDINGREQ : readnatpmpresponseorretry() called while no NAT-PMP request was pending";
    case NATPMP_ERR_NOGATEWAYSUPPORT:
      return "NATPMP_ERR_NOGATEWAYSUPPORT : the gateway does not support NAT-PMP";
    case NATPMP_ERR_CONNECTERR:
      return "NATPMP_ERR_CONNECTERR : connect() failed. check errno for details";
    case NATPMP_ERR_WRONGPACKETSOURCE:
      return "NATPMP_ERR_WRONGPACKETSOURCE : packet not received from the network gateway";
    case NATPMP_ERR_SENDERR:
      return "NATPMP_ERR_SENDERR : send() failed. check errno for details";
    case NATPMP_ERR_FCNTLERROR:
      return "NATPMP_ERR_FCNTLERROR : fcntl() failed. check errno for details";
    case NATPMP_ERR_GETTIMEOFDAYERR:
      return "NATPMP_ERR_GETTIMEOFDAYERR : gettimeofday() failed. check errno for details";
    case NATPMP_ERR_UNSUPPORTEDVERSION:
      return "NATPMP_ERR_UNSUPPORTEDVERSION";
    case NATPMP_ERR_UNSUPPORTEDOPCODE:
      return "NATPMP_ERR_UNSUPPORTEDOPCODE";
    case NATPMP_ERR_UNDEFINEDERROR:
      return "NATPMP_ERR_UNDEFINEDERROR";
    case NATPMP_ERR_NOTAUTHORIZED:
      return "NATPMP_ERR_NOTAUTHORIZED";
    case NATPMP_ERR_NETWORKFAILURE:
      return "NATPMP_ERR_NETWORKFAILURE";
    case NATPMP_ERR_OUTOFRESOURCES:
      return "NATPMP_ERR_OUTOFRESOURCES";
    }
    return "Unkwown NATPMP error code";
  }

};

int main()
{
  NATPMP natpmp;
  natpmp.tcp_port_mapping(22, 44534);

}
