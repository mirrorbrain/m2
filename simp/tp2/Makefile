CC	:= g++
CFLAGS	:= -O3 -std=c++11
IFLAGS	:= -I./includes

main: src/main.cpp  obj/api_int.o obj/api_ext.o obj/user.o obj/key.o obj/base64.o obj/aes.o obj/hmac.o obj/sha256.o obj/json.o
	$(CC) $(IFLAGS) $(CFLAGS) -o $@ $^

correction: src/correction.cpp  obj/api_int.o obj/api_ext.o obj/user.o obj/key.o obj/base64.o obj/aes.o obj/hmac.o obj/sha256.o obj/json.o
	$(CC) $(IFLAGS) $(CFLAGS) -o $@ $^

doc: includes/*
	doxygen api.dox

obj/%.o: src/%.cpp includes/*.hpp
	mkdir -p obj
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

obj/aes.o: src/crypto/aes.cpp includes/*.hpp
	mkdir -p obj
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

obj/hmac.o: src/crypto/hmac.cpp includes/*.hpp
	mkdir -p obj
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

obj/sha256.o: src/crypto/sha256.cpp includes/*.hpp
	mkdir -p obj
	$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f obj/* main *~ */*~ */*/*~
