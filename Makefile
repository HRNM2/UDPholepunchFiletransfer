all: Rendezvous ClienteA ClientB

rendezvouz: Rendezvous.c
        gcc -o Rendezvous Rendezvous.c 

clienta: ClienteA.c
        gcc -o ClienteA ClientA.c -lcrypto

clientb: ClienteB.c
        gcc  -o ClienteB ClientB.c -lcrypto

clean:
        rm -f Rendezvous ClienteA ClientB
