# Authors : Mircho Rodozov, mrodozov@cern.ch 
# created on : 05.07.13

CC=g++
CFLAGS=-g -O0

KERNEL = $(shell uname)
ROOTINC = $(shell root-config --cflags) 
ROOTLIBS = $(shell root-config --glibs)

ifeq (Darwin,$(KERNEL))
# How to install GCC 4.7 on OSX apart from the LLVM-g++ 4.2 version
# http://stackoverflow.com/questions/14153725/installing-gcc-4-7-1-on-os-x
# How to install jsoncpp
	CC=g++-mp-4.7
	ORACLE_LIBS = -locci -lclntsh 
	ORACLE_INCDIR = -I /Users/mrodozov/Desktop/OracleSDK/instantclient_11_2/sdk/include/
else
	ORACLE_LIBS = -locci -lclntsh -lociei
	ORACLE_INCDIR = -I /usr/include/oracle/12.1/client/
#	on lxplus libs and headers are under /afs/cern.ch/sw/lcg/external/oracle/
endif

COMPILE_AS_CPP11 = "-std=c++11"
# -std=c++0x when the compiler is 4.3.x to 4.6.x and -std=c++11 when its 4.7 and above

LBobjects = build/DataObject.o build/Strip.o build/Roll.o build/ExtendedStrip.o build/ExtendedRoll.o build/Chip.o build/RPCLinkBoardChannel.o build/RPCLinkBoard.o build/RPCLinkBoardChip.o build/RPCChambersCluster.o
ConfigObjects = build/RPCDBReader.o build/RPCConfigObject.o build/RPCChamberConditions.o 
RAWobjects = build/RPCRawConverter.o
LBobjects_names = DataObject.o Strip.o Roll.o ExtendedStrip.o ExtendedRoll.o Chip.o RPCLinkBoardChannel.o RPCLinkBoard.o RPCLinkBoardChip.o RPCChambersCluster.o
RAWobjects_names = RPCRawConverter.o
ConfigObjects_names = RPCDBReader.o RPCConfigObject.o RPCChamberConditions.o 

boost_libs = -lboost_thread-mt

all: mainApp

mainApp: main.cpp RPCRawConverter.o UserApplications.o $(LBobjects_names) $(ConfigObjects_names)
	$(CC) main.cpp $(CFLAGS) -o mainApp build/RPCRawConverter.o build/UserApplications.o $(ROOTINC) $(ORACLE_INCDIR) $(ROOTLIBS) $(ORACLE_LIBS) $(ConfigObjects) $(LBobjects) $(COMPILE_AS_CPP11)

UserApplications.o : UserApplications.cpp $(LBobjects_names) $(ConfigObjects_names) $(RAWobjects_names)
	$(CC) $(CFLAGS) -c -Wall UserApplications.cpp -o build/UserApplications.o $(ROOTINC) 

RPCRawConverter.o : src/RPCRawConverter.cpp
	$(CC) $(CFLAGS) -c -Wall src/RPCRawConverter.cpp -o build/RPCRawConverter.o $(ROOTINC)

RPCDBReader.o : src/RPCDBReader.cpp
	$(CC) $(CFLAGS) -c -Wall src/RPCDBReader.cpp -o build/RPCDBReader.o

Chip.o : src/RollCore/Chip.cpp ExtendedRoll.o
	$(CC) $(CFLAGS) -c -Wall src/RollCore/Chip.cpp -o build/Chip.o $(ROOTINC)

DataObject.o: src/RollCore/DataObject.cpp
	$(CC) $(CFLAGS) -c src/RollCore/DataObject.cpp -o build/DataObject.o

Strip.o: src/RollCore/Strip.cpp
	$(CC) $(CFLAGS) -c src/RollCore/Strip.cpp -o build/Strip.o

Roll.o : src/RollCore/Roll.cpp Strip.o DataObject.o
	$(CC) $(CFLAGS) -c src/RollCore/Roll.cpp -o build/Roll.o $(ROOTINC)

ExtendedStrip.o: src/RollCore/ExtendedStrip.cpp Strip.o
	$(CC) $(CFLAGS) -c src/RollCore/ExtendedStrip.cpp -o build/ExtendedStrip.o

ExtendedRoll.o: src/RollCore/ExtendedRoll.cpp Roll.o ExtendedStrip.o DataObject.o
	$(CC) $(CFLAGS) -c src/RollCore/ExtendedRoll.cpp -o build/ExtendedRoll.o $(ROOTINC)

RPCLinkBoardChannel.o : Strip.o ExtendedStrip.o src/RPCLinkBoardChannel.cpp
	$(CC) $(CFLAGS) -c src/RPCLinkBoardChannel.cpp -o build/RPCLinkBoardChannel.o

RPCLinkBoard.o : ExtendedRoll.o
	$(CC) $(CFLAGS) -c src/RPCLinkBoard.cpp -o build/RPCLinkBoard.o $(ROOTINC)

RPCLinkBoardChip.o : Chip.o
	$(CC) $(CFLAGS) -c src/RPCLinkBoardChip.cpp -o build/RPCLinkBoardChip.o $(ROOTINC)

RPCChambersCluster.o : RPCLinkBoard.o
	$(CC) $(CFLAGS) -c src/RPCChambersCluster.cpp -o build/RPCChambersCluster.o $(ROOTINC)

RPCConfigObject.o : src/RPCConfigObject.cpp RPCChamberConditions.o 
	$(CC) $(CFLAGS) -c src/RPCConfigObject.cpp -o build/RPCConfigObject.o

RPCChamberConditions.o : src/RPCChamberConditions.cpp
	$(CC) $(CFLAGS) -c src/RPCChamberConditions.cpp -o build/RPCChamberConditions.o 

clean:
	rm mainApp build/*.o *~
