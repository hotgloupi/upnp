
CXX		= g++

CFLAGS		= -Wall -Wextra -std=c++0x -Ilocal/include

LDFLAGS		= -Llocal/lib

MINIUPNP        = miniupnp
MINIUPNP_SRCS   = miniupnp.cpp
MINIUPNP_OBJS   = $(MINIUPNP_SRCS:.cpp=.o)

NATPMP          = natpmp
NATPMP_SRCS     = natpmp.cpp
NATPMP_OBJS     = $(NATPMP_SRCS:.cpp=.o)

TARGETS         = $(MINIUPNP)      $(NATPMP)
OBJECTS         = $(MINIUPNP_OBJS) $(NATPMP_OBJS)
SOURCES         = $(MINIUPNP_SRCS) $(NATPMP_SRCS)

LIBMINIUPNPC    = local/lib/libminiupnpc.a
LIBUPNP         = local/lib/libupnp.a
LIBNATPMP       = local/lib/libnatpmp.a

LIBRARIES       = $(LIBMINIUPNPC) $(LIBUPNP) $(LIBNATPMP)

.PHONY: all clean distclean depend
.SUFFIXES:
.SUFFIXES: .cpp .o

all: $(OBJECTS) $(TARGETS)

$(MINIUPNP): $(MINIUPNP_OBJS)
	@echo "Link CXX executable $@"
	@$(CXX) $(MINIUPNP_OBJS) $(LIBMINIUPNPC) $(LDFLAGS) -o $@

$(NATPMP): $(NATPMP_OBJS)
	@echo "Link CXX executable $@"
	@$(CXX) $(NATPMP_OBJS) $(LIBNATPMP) $(LDFLAGS) -o $@


.cpp.o:
	@echo "Build CXX object $@"
	@$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJECTS)

distclean: clean
	rm -rf $(TARGETS)

depend: $(LIBRARIES)

$(LIBMINIUPNPC):
	@mkdir -p ./local
	@echo "Build miniupnpc library"
	@sh -c 'cd libraries/miniupnpc; INSTALLPREFIX=../../local make install'

$(LIBUPNP):
	@echo "Build libupnp library"
	@sh -c 'export PREFIX=`pwd`/local && cd ./libraries/libupnp && ( make distclean || true ) && ./configure --prefix=$${PREFIX} && make install'

$(LIBNATPMP):
	@echo "Build natpmp library"
	@sh -c 'cd libraries/libnatpmp; INSTALLPREFIX=../../local make install'

