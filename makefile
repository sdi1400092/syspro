
sniffer: sniffer.o queueimplementation.o clean
	gcc -o sniffer sniffer.o queueimplementation.o

sniffer.o: sniffer.c queue.h
	gcc -c sniffer.c

queueimplementation.o: queue.h
	gcc -c queueimplementation.c

clean:
	rm fifo*
