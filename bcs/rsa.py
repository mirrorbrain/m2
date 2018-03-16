#!/usr/bin/env python

#Auteurs
#Nogues Mael
#Grandmontagne Mathieu
import random
from random import randrange

#On utilse une fonction de calcul de racine carree pour nombre de longueur indefini car la fonciton fourni par python n'est pas capable de calculer au deca de 256 bits.
def exact_sqrt(x):
    """Calculate the square root of an arbitrarily large integer.

    The result of exact_exact_sqrt(x) is a tuple (a, r) such that a**2 + r = x, where
    a is the largest integer such that a**2 <= x, and r is the "remainder".  If
    x is a perfect square, then r will be zero.

    The algorithm used is the "long-hand square root" algorithm, as described at
    http://mathforum.org/library/drmath/view/52656.html

    Tobin Fricke 2014-04-23
    Max Planck Institute for Gravitational Physics
    Hannover, Germany
    """

    N = 0   # Problem so far
    a = 0   # Solution so far

    # We'll process the number two bits at a time, starting at the MSB
    L = x.bit_length()
    L += (L % 2)          # Round up to the next even number

    for i in xrange(L, -1, -1):

        # Get the next group of two bits
        n = (x >> (2*i)) & 0b11

        # Check whether we can reduce the remainder
        if ((N - a*a) << 2) + n >= (a<<2) + 1:
            b = 1
        else:
            b = 0

        a = (a << 1) | b   # Concatenate the next bit of the solution
        N = (N << 2) | n   # Concatenate the next bit of the problem

    return (a, N-a*a)

#renvoi (x^y)%n en utilisant l'exponentiation modulaire O(log(n))
def modular_exp(a, n, m):
    b = 1
    while (n != 0):
        if ((n & 1) != 0):
            b = (a*b)%m
        a = (a*a)%m
        n = n//2
    return b

#renvoi vrai si 2^p-1 = 3^p-1 = 5^p-1 = 7^p-1 = 1 mod p
def prim(p) :
    return (modular_exp(2,p-1,p) == 1 and modular_exp(3,p-1,p) == 1 and modular_exp(5,p-1,p) == 1 and modular_exp(7,p-1,p) == 1)

#renvoi un nombre premier de n bit en utilisant la primalite de PGP
def prim_rand(n) :
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

#renvoi l inverse de a modulo n
def modular_inv (a, n) :
    b = a
    a = n
    q = a // b
    r = a - q * b
    v_0 = 1
    v_1 = -q % n
    while r != 0 :
        a = b
        b = r
        q = a // b
        r = a - q * b
        v = (v_0 - q * v_1) % n
        v_0 = v_1
        v_1 = v
    return v_0

#genere un le tuple (e, n,p,q,d) pour le couple de cles RSA
def keygenerator(size) :
    p = q = 0

    #on prend un premier aleatoire different pour p et q
    while p == q :
        p = prim_rand(size)
        q = prim_rand(size)
    n = p*q
    phi_n =  (p - 1) * (q - 1)

    e = (2 ** 16) + 1
    d = modular_inv(e, phi_n) #on calcul d td d inverse de e mod phi(n)
    while d < 0 : #si d < 0 on ajoute phi(n)
        d = d + phi_n
    if ((d >= phi_n) or (d == e) ) :#si d est sup a phi(n) ou que e a pour inverse lui meme on creer une nouvelle cle
        e, n, p, q, d = keygenerator(size)
    return e, n, p, q, d

#####################################TRAPPE#####################################

def fermat(n):
    if n & 1 == 0 :
        return [n >> 1, 2]
    temp, _ = exact_sqrt(n)
    x = long(temp)
    if x * x == n :
        return [x, x]
    x += 1
    while True :
        y2 = x * x - n
        temp, _ = exact_sqrt(y2)
        y = long(temp)
        if y * y == y2 :
            break
        else:
            x += 1
    return x - y, x + y

def factor_n (n, a):
    n_trap = n % a
    return fermat(n_trap)

def exploit_trap (p_trap, q_trap, a):
    k = 1
    p = a * k + p_trap
    while not prim(p) :
        k += 1
        p = a * k + p_trap

    k = 1
    q = a * k + q_trap
    while not prim(q) :
        k += 1
        q = a * k + q_trap
    n = p * q
    return n, p ,q

def search_keys (p, q, d):
    phi_n = (p - 1) * (q - 1)
    e = modular_inv(d, phi_n)
    return e

def keygenerator_trap(size) :
    a = random.getrandbits(3*size/4)
    p = q = 0

    #on prend un premier aleatoire different pour p et q
    while p == q :
        p_trap = prim_rand(size/4)
        k = 1
        while True :
            p = a * k + p_trap
            if prim(p) :
                break
            k += 1

        q_trap = prim_rand(size/4)
        k = 1
        while True :
            q = a * k + q_trap
            if prim(q) :
                break
            k += 1
    n = p * q

    phi_n = (p - 1) * (q - 1)

    e = (2 ** 16) + 1
    d = modular_inv(e, phi_n) #on calcul d tel que d est l'inverse de e mod phi(n)
    while d < 0 : #si d < 0 on ajoute phi(n)
        d = d + phi_n
    if (d >= phi_n) or (d == e) :#si d est sup a phi(n) ou que e a pour inverse lui meme on creer une nouvelle cle
        e, n, p, q, d, a = keygenerator_trap(size)
    return e, n, p, q, d, a

################################################################################

#genere les tuples cle publique , cle prive
def key(size):
    e,n,p,q,d = keygenerator(size)
    return (e,n), (d,n)

def encrypt(pk, plaintext):
    #la cle publique
    d, n = pk
    #pour chaque caratere on aplique c^e mod n
    cipher = [modular_exp(ord(char),e,n) for char in plaintext] # on conveti le char en ascii
    return cipher

def decrypt(pk, ciphertext):
    #la cle privee
    e, n = pk
    #pour chaque caractere on aplique c^d mod n
    plain = [(modular_exp(char,d,n)) for char in ciphertext]
    return plain

if __name__ == '__main__':
    message = raw_input("entrer un message a chifrer : ")
    size = int(raw_input("entrer une taille pour p et q : "))
    e, n, p, q, d, a = keygenerator_trap(size)
    print "public : ", d, ", ", n , "\nprivate : ", e, ", ", n
    print "p : ", p, "\nq : ", q

    print "####################################################################"
    p_trap, q_trap = factor_n(n, a)
    n, p, q = exploit_trap(p_trap, q_trap, a)
    e = search_keys(p, q, d)
    print "Exploitation de la trappe"
    print "\nFactorisation de N\'"
    print "p\' : ", p_trap
    print "q\' : ", q_trap

    print "\nFactorisation de N"
    print "p : ", p
    print "q : ", q, "\n"

    print "####################################################################"
    print "Production des clefs RSA"

    public = [d , n]
    print "Clef publique : "
    print "n : ", public[1]
    print "d : ", public[0]

    private = [e, n]
    print "\nClef privee : "
    print "n : ", private[1]
    print "e : ", private[0]

    encrypted_msg = encrypt(public, message)

    decrypted_msg = decrypt(private, encrypted_msg)
    print "\n Utilisation des clefs"
    print "chiffre : ", encrypted_msg
    print "message : ", ''.join(map(chr, decrypted_msg)), "\n"

    print "####################################################################"
    # messagetab = [ord(char) for char in message] # on converti le message initiale en ascci
    # print "message initial"
    # print messagetab
    #
    # encrypted_msg = encrypt(public, message)#chiffrement du message
    # print "message chiffre"
    # print encrypted_msg
    #
    # print "message dechifre"
    # decrypt_msg = decrypt(private, encrypted_msg)# dechifrement du message
    # print decrypt_msg
    #
    # if messagetab == decrypt_msg : #si le tableau ascii du dechifrement est identique au tableau du message initial le chiffrement/dechifrement et fonctionnel
    #     print "chifrement/dechifrement fonctionnel"
    # else :
    #     print "erreur lors du dechiffrement"
