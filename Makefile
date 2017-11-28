all: Rendezvous ClientA ClientB

Rendezvouz: Rendezvous.c
	gcc -o Rendezvous Rendezvous.c 

ClientA: ClientA.c
	gcc -o ClientA ClientA.c -lcrypto

ClientB: ClientB.c
	gcc  -o ClientB ClientB.c -lcrypto

clean:
	rm -f Rendezvous ClientA ClientB
