parametrosCompilacao=-Wall 
nomePrograma=txt2epub

all: $(nomePrograma)

$(nomePrograma): txt2epub.o
	gcc -o $(nomePrograma) txt2epub.o $(parametrosCompilacao)

txt2epub.o: txt2epub.c
	gcc -c txt2epub.c $(parametrosCompilacao)

clean:
	rm -f *.o *.gch *.epub *.zip $(nomePrograma)
