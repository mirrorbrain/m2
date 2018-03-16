CC	:= g++
CFLAGS	:= -O3 -std=c++0x
IFLAGS	:= -I./includes


main: src/main.cpp obj/rc4.o obj/aes.o obj/card.o obj/terminal.o
	$(CC) $(IFLAGS) $(CFLAGS) -o $@ $^

correction: src/correction.cpp obj/rc4.o obj/aes.o obj/card.o obj/terminal.o
	$(CC) $(IFLAGS) $(CFLAGS) -o $@ $^

obj/%.o: src/%.cpp includes/*.hpp
	mkdir -p obj
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f obj/* main *~ */*~
