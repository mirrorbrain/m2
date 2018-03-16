# Base de la Cryptographie pour la Sécurité

Clé symétrique : 128 bits de sécurité

Clé asymétrique : 1536 bits de sécurité

## Symmetric Crypto Block Cipher
Canal sécurisé : confidentialité, intégrité et d'authentification emetteur

Confidentialité chiffrement :
- par flot :
  - Stream Cipher
  - A S/1 (GLM)
  - RC4 (TLS)
  - **AES-CTR**
- par bloc :
  - Block Cipher
  - AES, DES, 3DES, RCS, MARS, Blowfish, Camellia --> 128 bits
  - mode opératoire : chiffré de longs messages.
  - ECB --> AES-ECB
  - CBC --> AES-CBC
  - CFB, CTR, OFB

![f function](https://upload.wikimedia.org/wikipedia/commons/thumb/2/25/Data_Encription_Standard_Flow_Diagram.svg/1200px-Data_Encription_Standard_Flow_Diagram.svg.png)

### Attack against DES
Before 1990: attacks against round reduced version (less than 16 rounds)

1990-92: **Differential** cryptanalysis

1993-94: **Linear** cryptanalysis

other attacks: Davies-Murphy

2DES --> 3DES

### Advanced Encryption Standard
Substitution / Permutation Network

Key Length: 128 / 192 / 256 bits

#Fonctions de hachage
F:{0,1}* --> {0,1}n

{0,1}* : l'ensemble de toutes les chaînes de bits de longueur arbitraire (mot vide, 0, 1, 00, ...).

~~MD2, MD4, MD5~~, MD6, ...

~~SHA-1~~, SHA-2, SHA-3

Blake2, Blake2b

##Propriété de sécurité
1. Résistante aux préimages : --> **pwd**

Étant donné H,k trouver m tq H(m)=k
2. Résistante aux 2nd préimages

Étant donné H,k,m tq k=H(m)
trouver m'!=m tq H(m')=k
3. Résistante aux collisions:

Étant donné H, trouver m!=m' tq H(m)=H(m')
4. Fonction hachage ressemble à un Oracle

H(x)~X~>H(x xor 1)

#Fonction de compression
Fonction de hachage avec entrée de taille fixe.

Fc={0,1}n+b -->{0,1}n

512+160 --> 160 bits
![fonction de compression vers un hash](https://upload.wikimedia.org/wikipedia/commons/thumb/5/57/MerkleDamgardFr.svg/530px-MerkleDamgardFr.svg.png)

**Théorème:** Si la fonction de compression est résistante aux collisions, alors la fonction de hachage aussi.
