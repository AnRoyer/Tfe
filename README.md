# Compiler le mesh partitioner

- Créer un répertoire "build" dans le dossier "Tfe".
- Exécuter "cmake .." puis "make" (utiliser "cmake .. -DMPI=ON" pour compiler la version parallèle). /!\ Due to recent modification it is not guarantee that the parallel version work.
- L'exécutable se trouvera dans "Tfe/bin/osx" (ou "Tfe/bin/nic4" sur le cluster).

# Exécuter "circle_pie"

- Les fichiers de base nécéssaires se trouvent dans le dossier "Tfe/ddm/circle_pie".
- Partitionner "Tfe/examples/circle_pie.msh".
- Copier les fichiers de maillage partitionnées dans un dossier "out" qui se doit se trouver à coté des fichiers de base nécessaires.
- Copier le fichier "partition.pro" à coté du fichier "circle_pie.pro".
- Lancer "circle_pie.pro".

(- En remplaçant "Helmholtz.pro" par "Helmholtz_error.pro" on peut calculer l'erreur par rapport à la solution analytique.)

# Exécuter "ground.pro" (salt3D)

- A l'aide du programme écrit dans le fichier "Tfe/salt/main.cpp" on peut sélectionner un plan de coupe dans le set de données.
- Compiler et lancer ce programme avec comme commande "-sx", "-sy" ou "-sz" pour un plan de coupe en x, y, ou z suivi du point ou faire la coupe. Ex: "-sy 30" fera une coupe selon Y au 30ème point de mesure.
- Ce programme sort un fichier "velocity.dat" qui contient le liste des vitesses pour la coupe demandée et un fichier "geometry.geo" qui contient la géométrie de la coupe.
- Mailler le fichier ".geo" et le partitionner (On peut modifier la finesse du maillage avant).
- Copier les maillages et le fichier "partition.pro" de la même manière que pour "circle_pie" dans un dossier contenant les fichiers de base pour l'analyse du sol (qui se trouvent dans "Tfe/ddm/ground").
- Lancer "ground.pro"
