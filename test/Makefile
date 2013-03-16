
all: Buffer DTComm_no_buff

Buffer: Buffer.cpp ../Buffer.cpp
	g++ Buffer.cpp -o Buffer

DTComm_no_buff: DTComm_no_buff.cpp ../DTComm_no_buff.cpp
	g++  DTComm_no_buff.cpp -o DTComm_no_buff

test: Buffer DTComm_no_buff
	./Buffer;
	./DTComm_no_buff
