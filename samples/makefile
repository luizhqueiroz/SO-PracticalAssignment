parametrosCompilacao=-Wall -lm
nomePrograma=samples

all: $(nomePrograma)

$(nomePrograma): samples.o
	gcc -o $(nomePrograma) samples.o $(parametrosCompilacao)

samples.o: samples.c
	gcc -c samples.c $(parametrosCompilacao)

clean:
	rm -f *.o *.gch $(nomePrograma)
