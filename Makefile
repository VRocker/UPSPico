OBJS=gpiohandler.o main.o

CXXFLAGS+=-std=c++11 -O3
BIN=upspico.bin

BUILDCC ?= $(CC)
BUILDCXX ?= $(CXX)
BUILDAR ?= $(AR)

ifdef OUTBINDIR
	$CPTOBIN=cp $@ $(OUTBINDIR)
endif

all: $(BIN) $(LIB)

%.o: %.c
	@rm -f $@
	$(BUILDCC) $(CFLAGS) $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.o: %.cpp
	@rm -f $@
	$(BUILDCXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@ -Wno-deprecated-declarations

%.bin: $(OBJS)
	$(BUILDCXX) -o $@ -Wl,--whole-archive $(OBJS) $(LDFLAGS) -Wl,--no-whole-archive -rdynamic
	$($CPTOBIN)	

%.a: $(OBJS)
	$(BUILDAR) r $@ $^

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)

