
all: Buffer DtReceive

Buffer: Buffer.cpp ../Buffer.cpp
	g++ Buffer.cpp -o Buffer

DtReceive: DtReceive.cpp ../DtReceive.cpp
	g++  DtReceive.cpp -o DtReceive

test: Buffer DTComm_no_buff
	./Buffer;
	./DTComm_no_buff
