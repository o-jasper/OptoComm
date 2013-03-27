
all: Buffer DTComm_no_buff DTMsg

Buffer: Buffer.cpp ../Buffer.cpp
	g++ Buffer.cpp -o Buffer

#Note: this one doesnt seem like it will have a use.
DTComm_no_buff: DTComm_no_buff.cpp ../DTComm_no_buff.cpp
	g++  DTComm_no_buff.cpp -o DTComm_no_buff

DTMsg: DTMsg.cpp ../DTMsg.cpp
	g++  DTMsg.cpp -o DTMsg

test: Buffer DTComm_no_buff
	./Buffer;
	./DTComm_no_buff
