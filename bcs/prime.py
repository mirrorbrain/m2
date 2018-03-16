#!/usr/bin/env python
from sys import argv
from time import time
from random import randrange
from random import getrandbits

#renvoi (x^y)%n en utilisant l'exponentiation modulaire O(log(n))
def lpowmod(x, y, z):
    a = 1
    while y > 0:
        if y % 2 == 1:
            a = (a * x) % z
        x = (x * x) % z
        y //= 2
    return a

#true si a^p =1 mod p
def fermat(a, p):
    if p == 2:
        return True
    if not p & 1:
        return False
    return lpowmod(a, p-1, p) == 1

# renvoi true si 2^p-1 = 3^p-1 = 5^p-1 = 7^p-1 = 1 mod p
def prim(n) :
    for  i in [2, 3, 5, 7] :# pour chaque valeur on regarde si fermat renvoi true
        if fermat(i, n) != 1 :
            return False
    return True

#renvoi un nombre premier de n bit en utilisant la primalite de PGP
def randompri(n) :
    p = getrandbits(n)*2 + 1
    a = [lpowmod(2, p-1, p), lpowmod(3, p-1, p), lpowmod(5, p-1, p), lpowmod(7, p-1, p)]
    b = [(2*2) % p, (3*3) % p, (5*5) % p, (7*7) % p]
    while a[0] != 1 or a[1] != 1 or a[2] != 1 or a[3] != 1:
        if (prim(p)):
            print("chelou")
        for i in [0, 1, 2, 3]:
            a[i] = (a[i] * b[i]) % p
        p += 2

    return p

if __name__ == "__main__":
	if len(argv) > 1 :
		start = time()
		print (randompri(int(argv[1]) - 1)) #prints the given number of prime numbers
		print ('Time in seconds: ' + str(time() - start)) #prints the execution time in seconds
	else :
		print ('Parameter needed : number of bits for the prime')
