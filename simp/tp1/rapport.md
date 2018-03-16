# Compte rendu du TP1
Nogues Maël

## Quelles sont les protections sur les utilisateurs, sur les clefs ?
- Les utilisateurs peuvent se supprimer eux-mêmes.
- Les fichiers json ont un test d'intégrité afin d'éviter la modification en direct.
- Le mot de passe administrateur est écris dans la documentation.
- S'il a été modifié, on peut récupérer le mot de passe par défaut en faisant un reset de l'api.
- Il y a un bufferoverflow sur le fichier users.json (30 caractères dans le login + \01 pour mettre la valeur privilège à 1).

## Essayez de modifier les fichiers et vérifiez que ce n'est pas possible.
