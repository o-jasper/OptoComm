
all: Buffer DtReceive dt_connect

Buffer: Buffer.cpp ../Buffer.cpp
	g++ Buffer.cpp -o Buffer

DtReceive: DtReceive.cpp ../DtReceive.cpp
	g++  DtReceive.cpp -o DtReceive

dt_connect: dt_connect.cpp ../DtReceive.cpp ../DtSend.cpp
	g++ dt_connect.cpp -o dt_connect

test: Buffer DTComm_no_buff
	./Buffer;
	./DTComm_no_buff
