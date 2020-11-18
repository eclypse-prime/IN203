**Envoi bloquant et non bloquant**

Le deuxième code est dangereux car le buffer myvar n'est pas bloqué, donc on ne peut pas assurer que les opérations se réalisent toujours dans le même ordre.
Pour sécuriser le code, il aurait fallu placer MPI_WAIT avant myvar = myvar + 2

**Circulation d’un jeton dans un anneau**

Les messages sont affichés dans l'ordre des processus car chaque processus doit attendre la réception du jeton du processus précédent. Chaque processus doit donc attendre l'exécution du processus précédent pour afficher le message. Le processus 0 doit attendre la réception du jeton du dernier processus pour afficher le message. Le message du processus 0 s'affiche donc en dernier.
