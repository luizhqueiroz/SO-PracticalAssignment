parametrosCompilacao=-Wall 
nomePrograma=tokenring

all: $(nomePrograma)

$(nomePrograma): tokenring.o
	gcc -o $(nomePrograma) tokenring.o $(parametrosCompilacao)

tokenring.o: tokenring.c
	gcc -c tokenring.c $(parametrosCompilacao)

clean:
	rm -f *.o *.gch pipe* $(nomePrograma)
