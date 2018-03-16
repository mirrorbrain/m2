# GLA
TP de Génie Logiciel Appliqué

Ce TP consiste en un projet eclipse comportant un forum en java propulsé par tomcat.

Pour utiliser ce forum sur votre machine il vous faudra modifier le chemin se situant dans la classe fr.acceis.forum.db.Connexion.java, la variable path, afin qu'il reflète correctement l'arborescence dans laquelle le projet se situe (racine du dossier forum).

Vous pourrez initialiser la base donnée avec la classe fr.acceis.forum.db.JeuDeTestJdBc.java qui comporte un main et que vous pourrez donc exécuter comme une application java.

Vous pourrez enfin lancez cette application sur un serveur tomcat 8.5 et profitez de ce forum en vous rendant sur votre navigateur favoris à l'adresse http://localhost:8080/forum/home.

Ce forum possède les fonctionnalitée suivante :
- création de nouveaux utilisateur (à login unique)
- création de nouveaux threads (fils de discussion)
- création de nouveaux postes (réponses aux fils de discussion)
- consultation des threads et des posts
- connexion et déconnexion des utilisateurs

La création de nouveaux threads et postes, ainsi que la consultation des postes sur les threads est réservée aux membres du forum connectés.
