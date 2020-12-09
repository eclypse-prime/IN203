# Question du cours n°1



# Question du cours n°2

La loi d'Amdahl donne S(n) = n/(1+(n-1)f), avec f = 0.1, donc pour n >> 1, S = 1/f = 10.

Le rapport de temps maximal gagné est donc de 10.

Pour ce jeu de données spécifiques, si on prend n = 16, on obtient un rapport de temps gagné de 6,4.

Pour n plus grand, le gain en rapport de temps gagné devient de plus en plus faible, il est raisonnable d'utiliser 16 ou moins nœuds de calcul.

Alice obtient une accélération maximale de 4. alors f = 0,25 en réalité.

La loi de Gustafson donne S(n) = s(1-n)+n. avec s = 0,25.

On sait que S(n) = 4, ce qui donne n égal à 5.

On calcule donc S(2n) = S(10) = 7,75.

Alice peut espérer une accélération de 7,75.

# Question du cours n°3

Pour la première version de Mandelbrot, le temps d'exécution avec un seul cœur est de 11,5 secondes, alors que le temps d'exécution sur 16 cœurs est de 2.12 secondes.
On constate donc un speedup de 5,42.
Le speedup n'est pas celui attendu car les temps d'exécution de chaque processus sont différents : certains ont plus de calculs à faire que les autres.
Il faut alors attendre la fin du calcul le plus long avant de rassembler les morceaux d'image. Ce n'est pas optimal

