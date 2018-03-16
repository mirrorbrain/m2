#!/usr/bin/env python
import random
import os
import array
from math import sqrt, ceil
from itertools import count, islice
import sys
from random import randrange

def pgcd(a,b):
    a,b=(b,a) if a<b else (a,b)
    while b:
        a,b=b,a%b
    return a

#renvoi (x^y)%n en utilisant l'exponentiation modulaire O(log(n))
def exp_mod(a, n, m):
    b = 1
    while (n != 0):
        if ((n & 1) != 0):
            b = (a*b)%m
        a = (a*a)%m
        n = n//2
    return b

#~ print fermat(7,19)
# renvoi true si 2^p-1 = 3^p-1 = 5^p-1 = 7^p-1 = 1 mod p
def prim(p) :
    return (exp_mod(2,p-1,p) == 1 and exp_mod(3,p-1,p) == 1 and exp_mod(5,p-1,p) == 1 and exp_mod(7,p-1,p) == 1)

#~ print prim(7)
#revoi un nombre premier de n bit en utilisant la primalite de PGP
def random_pri(n) :
    p = 2*(random.getrandbits(n-2) + pow(2,n-1)) + 1
    p_mod3 = p%3
    p_mod5 = p%5
    p_mod7 = p%7
    while (p_mod3 == 0 or p_mod5 == 0 or p_mod7 == 0 or not prim(p)):
        p += 2
        p_mod3 = (p_mod3 + 2)%3
        p_mod5 = (p_mod5 + 2)%5
        p_mod7 = (p_mod7 + 2)%7
    return p

def inverse_modulaire (a, N) :
    b = a
    a = N
    q = a // b
    r = a - q * b
    v_0 = 1
    v_1 = -q % N
    while r != 0 :
        a = b
        b = r
        q = a // b
        r = a - q * b
        v = (v_0 - q * v_1) % N
        v_0 = v_1
        v_1 = v
    return v_0

# genere un le tuple (e, n,p,q,d) pour le couple de cles RSA
def keygenerator(taille) :
    a = random.getrandbits(3*taille/4)
    p = q = 0
#on prend un premier aleatoire different pour p et q
    while p == q :
        pt = random_pri(taille)
        k=1
        while True :
            p = a*k +pt
            if prim(p):
                break
            k +=1
        qt = random_pri(taille)
        k = 1
        while True:
            q = a*k + qt
            if prim(q) :
                break;
            k += 1
    n = p *q
    phiden =  (p - 1) * (q - 1)
    e = (2**16)+1
    d = inverse_modulaire(e,phiden) # on calcul d td d inverse de e mod phi(n)
    while d < 0 : #si d < 0 on ajoute phi(n)
        d = d +phiden
    if ((d >= phiden) or (d==e) ) :# di d est sup a phi(n) ou que e a pour inverse lui meme on creer une nouvelle cle
        keygenerator(taille)
    return (e, n,p,q,d,a)
####################################################################################
def exploit_Trap1 (n, a):
    # Factorisation de N prime
    np = n % a
    p, q = fermat(np)
    return p, q

def exploit_Trap2 (pt, qt, a):
    # Recherche de p et q
    # -> Factorisation de N
    # Recherche de p
    k = 1
    p = a*k + pt
    while not prim(p):
        k += 1
        p = a*k + pt

    # Recherche de q
    k = 1
    q = a*k + qt
    while not prim(q):
        k += 1
        q = a*k + qt
    n = p * q
    return n, p ,q

def exploit_Trap3 (n, p, q, d):
    # Recherche des clefs
    phiN = (p-1)*(q-1)
    # On connait e, appartient a la clef publique

    # e l'inverse modulaire de d mod phiN
    e = inverse_modulaire(d, phiN)
    return e



################################################################""
def fermat(n):
    """
    Factorization of the integer `n`.
    :param n: An integer to be factored.
    :rtype: The factorization of `n`.
    """
    if n & 1 == 0:
        return [n >> 1, 2]
    x = int(sqrt(n))
    if x*x == n:
        return [x, x]
    x += 1
    while True:
        y2 = x*x-n
        y = int(sqrt(y2))
        if y*y == y2:
            break
        else:
            x += 1
    return x-y, x+y
################################################################################
#genere les tuples cle publique , cle prive
def key(taille):
    e,n,p,q,d = keygenerator(taille)
    return (e,n), (d,n)

def encrypt(pk, plaintext):
    #la cle publique
    e, n = pk
    #pour chaque caratere on aplique c^e mod n
    cipher = [exp_mod(ord(char),e,n) for char in plaintext] # on conveti le char en ascii
    return cipher

def decrypt(pk, ciphertext):
    #la cle privee
    d, n = pk
    plain = [(exp_mod(char,d,n)) for char in ciphertext]
    return plain

if __name__ == '__main__':
    message = raw_input("entrer un message a chifrer : ")
    taille = int(raw_input("entrer une taille pour p et q :"))
    e,n,p,q,d,a = keygenerator(taille)
    public = e,n
    private= d,n
    print ("public : ", public ,"\nprivate : ", private)
    print("-------------------------")
    pt, qt = exploit_Trap1(n, a)
    n, p, q = exploit_Trap2(pt, qt, a)
    e = exploit_Trap3(n, p, q, d)
    print("\nResultats de l\'exploitation de la trappe")
    print("\nFactorisation de N\'")
    print("p\' : ", pt)
    print("q\' : ", qt)

    print("\nFactorisation de N")
    print("p : ", p)
    print("q : ", q)
    print("-------------------------")
    print("\nProduction des clefs RSA")
    public = [d , n]
    print("Clef publique : ")
    print("n : ", public[1])
    print("d : ", public[0])
    private = [e, n]
    print("\nClef privee : ")
    print("n : ", private[1])
    print("e : ", private[0])

    print("-------------------------")

    messagetab = [ord(char) for char in message] # on converti le message initiale en ascci
    print ("message ikeynitial")
    print (messagetab)

    encrypted_msg = encrypt(public, message)#chiffrement du message
    print ("message chiffre")
    print (encrypted_msg)

    print ("message dechifre")
    decrypt_msg= decrypt(private, encrypted_msg)# dechifrement du message
    print (decrypt_msg)

    if messagetab==decrypt_msg : #si le tableau ascii du dechifrement est identique au tableau du message initial le chiffrement/dechifrement et fonctionnel
        print ("chifrement/dechifrement fonctionnel")
    else :
        print "erreur lors du dechiffrement"
