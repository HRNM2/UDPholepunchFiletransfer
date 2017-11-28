all: Rendezvous.c ClienteA.c ClientB.c

rendezvouz: Rendezvous.c
        gcc -o Rendezvous Rendezvous.c 

clienta: clienta.c
        gcc -o clientea ClientA.

clientb: clientb.c
        gcc  -o clientb ClientB.c

clean:
        rm -f Rendezvous.c ClienteA.c ClientB.c
