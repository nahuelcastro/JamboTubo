.PHONY : all clean clean-res

all:
	rm -f jambotubo
	g++ -o jambotubo jambotubo.cpp -O2 -std=c++14


clean :
	rm -f jambotubo
	find . -type f -name '*.txt' -delete
	find . -type f -name 'indice.csv' -delete

clean-res:
	find . -type f -name 'resultado.csv' -delete