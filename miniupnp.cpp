// g++-4.7 -std=c++11 -Wall -Wextra -I./local/include miniupnpc.c -Llocal/lib -Wl,-Bstatic -lminiupnpc -Wl,-Bdynamic
#include <miniupnpc/miniupnpc.h>

#include <stdexcept>

struct ControlPoint
{
private:
  UPNPUrls _upnp_urls;
  IGDdatas _igd_data;

public:
  // Discover upnp devices on the network.
  void
  discover_services(int search_time = 10)
  {
    int error;
    struct ::UPNPDev* devlist = ::upnpDiscover(
        search_time * 1000,   // delay
        nullptr,              // multicastif
        nullptr,              // minissdpdsock
        0,                    // sameport
        0,                    // ipv6
        &error                // retreive error
    );

    if (error)
      throw std::runtime_error("couldn't discover UPNP devices");

    int res = ::UPNP_GetValidIGD(devlist, &_upnp_urls, &_igd_data, nullptr, 0);
    ::freeUPNPDevlist(devlist);
    if (res == 0)
        throw std::runtime_error("Couldn't find any IGD devices");
  }
};

int main()
{
  ControlPoint ctrl;

  ctrl.discover_services(1);
}
