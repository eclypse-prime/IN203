# Question du cours n°1



# Question du cours n°2

La loi d'Amdahl donne S(n) = n/(1+(n-1)f), avec f = 0.1, donc pour n >> 1, S = 1/f = 10.

Le rapport de temps maximal gagné est donc de 10.

Pour ce jeu de données spécifiques, si on prend n = 16, on obtien un rapport de temps gagné de 6,4.

Pour n plus grand, le gain en rapport de temps gagné devient de plus en plus faible, il est raisonnable d'utiliser 16 ou moins nœuds de calcul.

En doublant le nombre de données à traiter, la proportion p passe à 0,95 et f à 0,05.

La loi de Gustafson donne S_s(n) = n + (1-n)s
