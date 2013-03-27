
all: Buffer DTMsg

Buffer: Buffer.cpp ../Buffer.cpp
	g++ Buffer.cpp -o Buffer

DTMsg: DTMsg.cpp ../DTMsg.cpp
	g++  DTMsg.cpp -o DTMsg

test: Buffer DTComm_no_buff
	./Buffer;
	./DTComm_no_buff
