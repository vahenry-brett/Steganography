# Brett Henry 01017222
CFLAGS = -Wall -lm
CC = gcc
TARGET = image
EX= StegoExtract
EM= Stego
ex  : $(EX)
$(EX): $(EX).c
	$(CC) $(EX).c $(TARGET).c -o StegoEx
em  : $(EM)
$(EM): $(EM).c
	$(CC) $(EM).c $(TARGET).c -g -o StegoEm 
clean:
	rm StegoEx
	rm StegoEm
                 
