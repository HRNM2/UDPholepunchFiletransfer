all: Rendezvous ClienteA ClienteB

Rendezvouz: Rendezvous.c
	gcc -o Rendezvous Rendezvous.c 

ClienteA: ClienteA.c
	gcc -o ClienteA ClientA.c -lcrypto

ClienteB: ClienteB.c
	gcc  -o ClienteB ClientB.c -lcrypto

clean:
	rm -f Rendezvous ClienteA ClientB
