# BECAM Tanguy
# CALLEMARD Alexis

import random
import os
import array
from math import sqrt, ceil
from itertools import count, islice
import sys


def anModm(a, n, m):
    # calcul a^n mod m
    result = 1
    while n>0:
        if n&1>0:
            result = (result*a)%m
        n >>= 1
        a = (a*a)%m
    return result


def fermat_test(n):
    # Test si n est possiblement premier
     def try_it(a):
         return anModm(a, n, n) == a
     return try_it(1 + random.randrange(n-1))


def is_fast_prime(n, times):
     # Ap_traplique le test de primalite de fermat 'times' fois
     for i in range(times):
         if not fermat_test(n): return False
     return True


def gcd(x, y):
    # Calcul le pgcd de x et y
    while y != 0:
        (x, y) = (y, x % y)
    return x


def egcd(a, b):
    #Euclide etendu recursif, ne fonctionne pas pour un module RSA trop grand
    if a == 0:
        return (b, 0, 1)
    else:
        g, x, y = egcd(b % a, a)
        return (g, y - (b // a) * x, x)


def xgcd(b, n):
    #Euclide etendu iteratif
    x0, x1, y0, y1 = 1, 0, 0, 1
    while n != 0:
        q, b, n = b // n, n, b % n
        x0, x1 = x1, x0 - q * x1
        y0, y1 = y1, y0 - q * y1
    return  b, x0, y0


def modinv(b, n):
    #Inverse modulaire de b mod n
    g, x, _ = xgcd(b, n)
    if g == 1:
        return x % n
    return -1

# Version corrigee generateur PREMIER
#############################################################################################################
def expMod(a,n,m): #a^n mod m
    b = 1
    while (n != 0):
        if ((n & 1) != 0):
            b = (a*b)%m
        a = (a*a)%m
        n = n//2
    return b

def estPseudoPremier(p):
    return (expMod(2,p-1,p) == 1 and expMod(3,p-1,p) == 1 and expMod(5,p-1,p) == 1 and expMod(7,p-1,p) == 1)

def genPseudoPremier(nb_bits):
    p = 2*(random.getrandbits(nb_bits-2) + pow(2,nb_bits-1)) + 1
    p_mod3 = p%3
    p_mod5 = p%5
    p_mod7 = p%7
    while (p_mod3 == 0 or p_mod5 == 0 or p_mod7 == 0 or not estPseudoPremier(p)):
        p += 2
        p_mod3 = (p_mod3 + 2)%3
        p_mod5 = (p_mod5 + 2)%5
        p_mod7 = (p_mod7 + 2)%7
    return p
# SANS TRAp_trapE
#############################################################################################################

def moduleRSA ():
    # Construction d'un module RSA
    t = 1024 # taille du module RSA
    # x1=round(2.0**(t/2), 0);
    # x2=round(2.0**((t+1)/2), 0);

    # On cherche p et q premiers tels que p!=q
    while True:
        # p = random.randint(x1, x2)
        # p = random.getrandbits(t)
        p = genPseudoPremier(t)
        if is_fast_prime(p, 100) :
            break;

    while True:
        # q = random.randint(x1, x2)
        # q = random.getrandbits(t)
        q = genPseudoPremier(t)
        if is_fast_prime(q, 100) and q!=p:
            break;

    N = p*q
    ret = [N, p, q]
    return ret

# AVEC TRAp_trapE
#############################################################################################################
def moduleRSA_trap(t):
    # Construction d'un module RSA avec trap_trape
    a = random.getrandbits(3*t/4)

    p_trap = genPseudoPremier(t/4)
    k = 1
    while True:
        p = a*k + p_trap
        if estPseudoPremier(p) :
            break;
        k += 1

    q_trap = genPseudoPremier(t/4)
    k = 1
    while True:
        q = a*k + q_trap
        if estPseudoPremier(q) :
            break;
        k += 1
    N = p*q

    ret = [N, p, q, a, t]
    return ret
#############################################################################################################

def factorise(n):
    x = ceil(sqrt(n)) # sqrt(n) arondi a l'entier superieur
    y = x**2 - n
    while not sqrt(y).is_integer():
        x += 1
        y = x**2 - n
    return x + sqrt(y), x - sqrt(y)

def fermat(n):
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

###############################################################################################################

def generetorRSAKey(N, p, q):
    print("Module RSA :")
    print("p : ", p)
    print("q : ", q)

    # Generation d'une paire de clef RSA
    phiN = (p-1)*(q-1)

    # On cherche un e dans l'intervalle [1, phiN] tq pgcd(e, phiN) == 1
    while True :
        e = random.randint(1, phiN)
        if gcd(e, phiN) == 1 :
            break;

    # Soit d l'inverse modulaire de e mod phiN
    d = modinv(e, phiN)

    PublicKey = [d , N]
    print("\nPublicKey : ")
    print("N : ", PublicKey[1])
    print("d : ", PublicKey[0])

    PrivateKey = [e, N]
    print("\nPrivateKey : ")
    print("N : ", PrivateKey[1])
    print("e : ", PrivateKey[0])
    ret = [PrivateKey, PublicKey, d]

    return ret

def exploit_RSA (N, a, c, d):
    # Exploitation de la trap_trape
    p_trap, q_trap = exploit_Trap1(N, a)
    N, p, q = exploit_Trap2(p_trap, q_trap, a)
    e = exploit_Trap3(N, p, q, d)

    print("\nResultats de l\'exploitation de la trap_trape")
    print("\nFactorisation de N\'")
    print("p\' : ", p_trap)
    print("q\' : ", q_trap)

    print("\nFactorisation de N")
    print("p : ", p)
    print("q : ", q)

    print("\nProduction des clefs RSA")
    PublicKey = [d , N]
    print("Clef publique : ")
    print("N : ", PublicKey[1])
    print("d : ", PublicKey[0])

    PrivateKey = [e, N]
    print("\nClef privee : ")
    print("N : ", PrivateKey[1])
    print("e : ", PrivateKey[0])

    m = dechiffreRSA ( c, PrivateKey[0], PrivateKey[1] )
    print("\n Utilisation des clefs")
    print("chiffre : ", c)
    print("message : ", m)


def exploit_Trap1 (N, a):
    # Factorisation de N prime
    Np = N % a
    p, q = fermat(Np)
    return p, q

def exploit_Trap2 (p_trap, q_trap, a):
    k = 1
    p = a*k + p_trap
    while not estPseudoPremier(p):
        k += 1
        p = a*k + p_trap

    # Recherche de q
    k = 1
    q = a*k + q_trap
    while not estPseudoPremier(q):
        k += 1
        q = a*k + q_trap

    N = p * q
    return N, p ,q

def exploit_Trap3 (N, p, q, d):
    # Recherche des clefs
    phiN = (p-1)*(q-1)

    # On connait e, ap_trapartient a la clef publique

    # e l'inverse modulaire de d mod phiN
    e = modinv(d, phiN)

    return e

def dechiffreRSA ( chiffre, e, N ):
        # Dechiffre un message RSA a partir du chiffre et de la clef privee
        dechiffre = anModm (chiffre, e, N)
        return dechiffre

def chiffreRSA ( message, d, N ):
        # Chiffre un message RSA a partir du message en clair et de la clef publique
        chiffre = anModm (message, d, N)
        return chiffre

def echangeRSA ( message, N, p, q ):
        #Chiffre puis dechiffre d'un message avec RSA

        x = generetorRSAKey(N, p, q)
        PrivateKey = x[0]
        PublicKey = x[1]
        d = x[2]

        c = chiffreRSA ( message, PublicKey[0], PublicKey[1] )
        m = dechiffreRSA ( c, PrivateKey[0], PrivateKey[1] )
        print("chiffre", c)
        print("dechiffre : ", m)

        return m, c, d

def main ():
    # x = moduleRSA()
    t = 128 # taille de p et q en bit
    x = moduleRSA_trap(t)
    N = x[0]
    p = x[1]
    q = x[2]
    a = x[3]

    print("--- RSA ---")
    message = 1234 # doit etre un entier !!!
    print("message :", message)
    m, c, d = echangeRSA(message, N, p, q)

    print("--- Trap_trape RSA ---")
    exploit_RSA (N, a, c, d)

main()
