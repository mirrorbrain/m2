#include "api.hpp"

/* TP 2

   Le but de ce TP est, de nouveau, d'être capable de récupérer la valeur des clefs de l'utilisateur benoit.
   Pour cela, il est interdit d'utiliser les failles vues lors du premier TP.

   Le code fourni avec ce TP est similaire au précédent mais 3 erreurs d'implémentation se sont glissées. Celles-ci permettent cet exploit sans utiliser les failles précédentes. Plus précisément, l'enchainement de deux failles permet l'exploit. On a alors deux façons différentes de s'y prendre.
   Ces erreurs peuvent être considérées comme des fautes de frappes ou d'inattention dont les effets sont graves. Cependant, ces erreurs seraient détectées par des tests unitaires.

   Il est fortement déconseillé de faire un diff avec le code du TP précédent : le but ici est de vous entraîner à détecter ces erreurs en relisant du code car un exercice de ce type sera présent à l'examen écrit. Il est donc dans votre intérêt de jouer le jeu et de ne pas regarder quelles lignes ont été modifiées dans le source.

   Après 1 heure de TP environ je donnerai la liste des 3 "effets graves" que l'on peut obtenir afin d'aiguiller les personnes n'ayant pas suffisament avancé. Puis je donnerai au cas par cas des indices voire la réponse afin que chacun puisse avoir un bout de code qui permette de mener l'exploit jusqu'au bout d'ici la fin du TP.

   Si des personnes terminent avant la fin, l'exercice supplémentaire sera d'écrire le code d'exploit de façon "propre" (si ce n'est déjà le cas). C'est-à-dire, un code d'exploit qui suit les différentes bonnes pratiques vues en cours (commentaires, tests de codes retour, découpage en fonctions simples ...). Cela sera potentiellement l'occasion de vous exercer.

 */

int main()
{

        // Pour ce TP vous jouez le rôle de l'administrateur même si au final l'attaque peut être exécutée par n'importe quel utilisateur.

        API::initFile("users.json","keys.json",1u);

        API *api = new API("users.json","keys.json");

        api->connectUser("admin","s3cr3t_p@ss");

        api->disconnect();

        delete api;

        return 0;
}
