#############
# generic application Makefile
# you can probably use this makefile for your app by changing the APPNAME variable
#############
APPNAME=openredalert
TARGET=../obj/$(APPNAME)
VERSION=462
RELEASE=1

#############
# compiler and linker
#############
CPLUSPLUS=g++
LINK=$(CPLUSPLUS)

#############
# SDL
#############
SDL_INCLUDE=`sdl-config --cflags`
SDL_LIBS=`sdl-config --libs`

#############
# openredalert source and include directories
#############
INC=./src
OPENREDALERT_INCLUDE=-I$(INC)
SRC=./src ./src/audio ./src/game ./src/misc ./src/ui ./src/vfs ./src/vfs/mix ./src/vfs/filesystem ./src/video
DEST=./obj

#############
# compile parameters
#############
DEFAULT_CPPFLAGS=-O2 -g -fmessage-length=0 -D_FORTIFY_SOURCE=2 -fPIC
#DEFAULT_CPPFLAGS=$(CPPFLAGS)
CPPFLAGS_RELEASE=
CPPFLAGS_DEBUG=-g
ALL_CPPFLAGS=$(DEFAULT_CPPFLAGS) $(SDL_INCLUDE) $(OPENREDALERT_INCLUDE) -o

# Turns on debugging: to enable run "make -DDEBUG=1"
ifeq ($(DEBUG),1)
	ALL_CPPFLAGS += -DDEBUG=1
endif

#############
# linker parameters
#############
#DEFAULT_LD_FLAGS=-fPIC -shared -Wl,-soname,$(TARGET).$(VERSION) -o
DEFAULT_LD_FLAGS=-o
LDFLAGS=$(SDL_LIBS) -lSDL_mixer $(DEFAULT_LD_FLAGS)

DEP_FILE=$(APPNAME).dep

#############
# tell make where to find the files
#############
vpath %.h $(INC)
vpath %.cpp $(SRC)
vpath %.o $(DEST)

#############
# create object file names from the source file names
#############
src_dirs=$(subst :, ,$(SRC))
source_files=$(foreach dir,$(src_dirs),$(wildcard $(dir)/*.cpp))
header_files=$(foreach dir,src,$(wildcard $(dir)/*.h))
OBJECTS=$(notdir $(patsubst %.cpp,%.o,$(source_files)))

#############
#############
release:
	$(MAKE) $(TARGET) CPPFLAGS="$(CPPFLAGS_RELEASE) $(ALL_CPPFLAGS)"

#############
#############
debug:
	$(MAKE) $(TARGET) CPPFLAGS="$(CPPFLAGS_DEBUG) $(ALL_CPPFLAGS)"

#############
#############
install:
	mkdir -p $(DESTDIR)$(BINDIR)
	install -m 755 ${DEST}/$(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)
	# create links ...

#############
#############
clean:
	-rm -rf $(DEST) $(TARGET)

#############
#############
depend:
	-makedepend -f- -Y -I$(INC) $(source_files) > $(DEP_FILE)

#############
# internal targets
#############
$(TARGET): $(DEST) $(OBJECTS)
	cd $(DEST); $(LINK) $(OBJECTS) $(LDFLAGS) $@

#############
#############
$(DEST):
	-mkdir -p $(DEST)

#############
#############
$(OBJECTS): %.o: %.cpp
	$(CPLUSPLUS) $(ALL_CPPFLAGS) $(DEST)/$@ -c $<

#############
#############
#include $(DEP_FILE)
