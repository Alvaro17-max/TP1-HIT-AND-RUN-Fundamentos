help: 
	@echo "make compilar"
	@echo "make ejecutar"

compilar: 
	@gcc juego.c hitrun.c utiles.o -o juegohiti -std=c99 -Wall -Wconversion -Werror -lm
 
ejecutar: 
	@./juegohiti
