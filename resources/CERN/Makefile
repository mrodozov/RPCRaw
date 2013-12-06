#include Makefile.arch
#----------------------------------------------------------------------------------
TESTS           := rpc_offline.C IniFile.cpp
TESTO           := $(TESTS:.C=.o)
TEST            := rpc_offline

TESTS2           := data.C
TESTO2           := $(TESTS2:.C=.o)
TEST2            := data

TESTS3           := rpc_offline_noise.C IniFile.cpp
TESTO3           := $(TESTS3:.C=.o)
TEST3           := rpc_offline_noise

OBJ             = $(TESTO)
OBJ2            = $(TESTO2)
OBJ3            = $(TESTO3)
PROGRAMS        = $(TEST)
PROGRAMS2       = $(TEST2)
PROGRAMS3       = $(TEST3)

CXXFLAGS      += -std=c++0x 
CXXFLAGS      += 
GLIBS         += 
#----------------------------------------------------------------------------------
.SUFFIXES: .$(SrcSuf) .$(ObjSuf) .$(DllSuf)

all:            $(PROGRAMS) $(PROGRAMS2) $(PROGRAMS3)

$(TEST):        $(TESTO)
		$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(TEST2):       $(TESTO2)   
		$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
		@echo "$@ done"

$(TEST3):       $(TESTO3)   
		$(LD) $(LDFLAGS) $^ $(GLIBS) $(OutPutOpt)$@
		@echo "$@ done"

clean:
		@rm -f $(OBJ) $(TESTO) core *~ src/*~ include/*~

.$(SrcSuf).$(ObjSuf):
		$(CXX) $(CXXFLAGS) -c $< -o $@
