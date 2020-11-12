# Initiation à MPI

Ce TP permet une initiation aux commandes de base de MPI.

Rappels essentiels :
-------------------
   1. Pour lancer une exécution parallèle avec MPI :

        mpiexec -np xx ./nom_executable

      où xx est le nombre de processus qui lanceront simultanément l'exécutable.

   2. Utiliser le Makefile fourni avec le TP, cela vous évitera (principalement pour ceux utilisant MSYS 2) une fastidieuse ligne de commande.
 
   3. Pour ceux ayant installé MSYS 2, bien penser à commenter dans le Makefile la ligne
           
        include Make.inc

      et décommenter la ligne

        include Make_msys2.inc

       Pour ceux ayant un Mac, penser également à commenter la ligne

        include Make.inc

       et décommenter la ligne

        include Make_osx.inc

   4. Utiliser les fichiers SkeletonMPIProgram.cpp ou SkeletonMPIProgramWithFilesOutput.cpp comme canevas pour commencer votre programme MPI (conserver ces squelettes pour les prochaines sessions, ils seront utiles).

   5. Lorsque vous mettez en oeuvre un algorithme en parallèle, ayez une vision locale du problème :

        - Si je suis le processus de rang x, que dois-je faire ?
        - J'envoie des messages à qui ?
        - Je reçois des messages de qui ?

      Par contre, lors de la mise au point de l'algorithme parallèle, il est nécessaire d'avoir une vision globale
      de ce que font tous les processus !
