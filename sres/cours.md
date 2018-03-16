#S R E S

##Les attaques orientées réseau
Il existe plusieurs types d'attaques réseau, ayant des buts différents:
- Usurper une identité
- Porter atteinte à la disponibilité d'un service
- Récuperer, voire altérer, des informations

###Usurpation d'identité
**IP Spoofing:** usurper une adresse IP lors de communications sur le réseau. Efficaces à une époque ou des services comme rsh étaient accessibles et non sécurisés.

####Rappels sur TCP
TCP au dessus d'IP dans le modèle TCP/IP (couche transport OSI).

![Établissement d'une session](http://www.laissus.fr/cours/img57.png)

####Interception de communications

**MAC flooding:** Saturer le hub en envoyant pleins de trames Ethernet en prenant des MACs aléatoires pour remplir la table de correspondance ports/MAC du switch ayant une taille limitée.

####Le protocole ARP

**ARP:** Broadcast pour récupérer l'adresse MAC de la machine destinataire sur le réseau local.

####ARP cache poisonning

Déposer des associations IP/MAC fallacieuses.

**Attaque:**
- Envoi 2 réponses ARP non-sollicitées (à la machine cible et à la passerelle de leur sous réseau)
  - MAC de l'attaquant avec IP passerelle à la cible
  - MAC attaquant avec IP cible à la passerelle
- Récupération de tout le trafic de la cible vers la passerelle (tout le trafic de la cible vers internet) et possibilité de modifier les réponses
- Type MITM (Man In The Middle)

####Outillage
- Wireshark
- hping3
- scapy
- Ettercap
