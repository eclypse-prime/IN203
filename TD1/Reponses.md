**Envoi bloquant et non bloquant**

Le deuxième code est dangereux car le buffer myvar n'est pas bloqué, donc on ne peut pas assurer que les opérations se réalisent toujours dans le même ordre.
Pour sécuriser le code, il aurait fallu placer MPI_WAIT avant myvar = myvar + 2
