# Rapport de projet

Le projet d'IN203 consistait en la parallélisation d'un programme qui simule la colonisation d'une galaxie par des civilisations technologiques.

Cette parallélisation s'est effectuée avec différentes méthodes : OpenMP, Threads, et MPI.

Ces différentes méthodes ont permis de paralléliser le code sans changer son résultat, mais avec des performances légèrement différentes.

## Compilation

Mon projet a la particularité d'avoir été effectué sur Windows. En effet, j'ai installé la librairie graphique SDL2 sur Windows, et ai modifié le fichier Makefile utilissé pour compiler le projet afin de le faire fonctionner avec mingw32-make, qui remplit la même fonction.

Les étapes que j'ai suivies sont documentées sur ce git : https://github.com/arthurliu2022/in203-win

Pour résumer, il s'agissait d'installer la librairie SDL2 dans le dossier de MinGW, puis d'ajouter des bibliothèques .dll nécéssaires pour exécuter un programme avec SDL2. Ensuite, l'ajout des chemins dans la variable d'environnement PATH et l'ajout d'un alias pour appeller mingw32-make.

J'ai également installé MS-MPI, l'implémentation Windows de MPI afin de pouvoir tester la parallélisation avec MPI.

## Les programmes

J'ai modifié et écrit plusieurs sources pour pouvoir tester et comparer les différents façon de paralléliser la simulation.

- `paramètres.cpp` a été parallélisé avec OpenMP.
- `colonisation.cpp` a été parallélisé avec Threads pour afficher et calculer simultanément.
- `colonisation_mpi.cpp` a été parallélisé avec MPI.

Des sources "benchmark" ont été créées, dans le but de comparer les performances selon différentes méthodes : 
- `colonisation_benchmark.cpp` pour la méthode OpenMP + Threads
- `colonisation_benchmark2.cpp` pour la méthode OpenMP seulement
- `colonisation_benchmark3.cpp` pour la méthode MPI seulement

Un fichier `benchmark.bat` a été écrit pour pouvoir lancer tous les benchmarks à la suite puis écrire tous les résultats dans un fichier `benchmark.csv`. Ce format a été choisi explicitement pour pouvoir visualiser et traiter ces résultats avec un logiciel de type Excel.

## Implémentation

Dans le fichier `paramètres.cpp`, la parallélisation s'est effectuée en utilisant OMP, en parallélisant la boucle dans la fonction `mise_a_jour()`. Il s'agissait d'ajouter une ligne `#pragma omp parallel for simd` devant la boucle for.

Le mot-clé `simd` permet de demander au compilateur d'utiliser les optimisation SIMD de vectorisation à l'échelle du cœur de processeur.

Une déclaration des indices de la boucle for qui était faite au départ avant le for a été replacée à l'intérieur du for. Cela a permis de rendre les indices i,j privés et permettre une parallélisation correcte du code.

Le générateur de nombre aléatoires a été modifié, en utilisant cette implémentation du générateur de nombres pseudo-aléatoires **Mersenne Twister** qui gère la parallélisation, contrairement à `rand()`.

```C++
double random_threaded_double() {
    thread_local static std::mt19937 generator(std::random_device{}());
    std::uniform_real_distribution<double> distribution(0., 1.);
    return distribution(generator);
}

int random_direction() {
    thread_local static std::mt19937_64 generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(0, 3);
    return distribution(generator);
}
```


Dans le fichier `colonisation.cpp`, l'affichage et le calcul ont été parallélisés. Pour cela, un thread a été créé, dans lequel l'affichage est effectué. Le calcul se fait dans le thread principal, en appellant la version parallélisée avec OMP de `mise_a_jour()`.


Dans le fichier `colonisation_MPI.cpp`, l'affichage est réalisé par le thread principal, le thread 0, les autres threads effectuant les calculs. Le thread 0 reçoit simplement les données calculées par les autres threads, et envoie en retour les données correspondants aux cellules fantômes pour permettre aux autres threads de calculer correctement l'expansion des civilisations.

## Conflits mémoires éventuels

Il peut y avoir une écriture simultanée :
- Si deux civilisations s'étendent sur la même case (pas de problème dans ce cas, la case finira quand-même habitée)
- Si une civilisation s'étend sur une case qui devient inhabitable.
- Si une civilisation s'étend sur une case qui devient inhabitée.

En théorie, les deux derniers problèmes ne devraient pas survenir, car le programme vérifie si une case est bien inhabitée avant d'essayer d'écrire dessus.

Il serait possible de régler définitivement ces problèmes en rendant l'écriture critique avec `#pragma omp critical` mais le prix en performance serait extrèmement élevé.

## Performance

Les calculs de performance ont été réalisés avec les fichiers `colonisation_benchmark.exe`, `colonisation_benchmark2.exe`, `colonisation_benchmark3.exe`, compilés à partir des sources respectives `colonisation_benchmark.cpp`, `colonisation_benchmark2.cpp`, `colonisation_benchmark3.cpp`, ainsi que des autres sources : `galaxie.cpp`, `parametres.cpp`.

Le script `benchmark.bat` lance les programmes pour un nombre de cœurs allant de 1 à 16 (maximum sur ma machine 8 cœurs 16 threads), et donne les résultats suivants :

|         | OMP     |         |             | OMP+Threads |         |             | MPI     |         |             |
|---------|---------|---------|-------------|-------------|---------|-------------|---------|---------|-------------|
| Threads | Temps (s)  | FPS     | Speedup     | Temps (s)      | FPS     | Speedup     | Temps (s)  | FPS     | Speedup     |
| 1       | 27.0686 | 18.4716 |           1 | 26.7193     | 18.7131 | 1.013074125 |         |         |             |
| 2       | 13.9216 | 35.9153 | 1.944352411 | 13.9649     | 35.8041 | 1.938332359 | 26.4343 | 18.9148 |  1.02399359 |
| 3       | 9.63876 | 51.8739 | 2.808305723 | 9.53098     | 52.4605 | 2.840062583 | 13.2307 | 37.7909 | 2.045892072 |
| 4       | 7.48338 | 66.8147 | 3.617158232 | 7.4489      | 67.124  | 3.633902856 | 8.84325 | 56.5403 | 3.060931376 |
| 5       | 6.41164 | 77.9831 | 4.221783711 | 6.25691     | 79.9117 | 4.326192642 | 6.71957 | 74.4096 | 4.028324563 |
| 6       | 5.51593 | 90.6466 | 4.907349661 | 5.29697     | 94.3935 | 5.110196193 | 5.51288 | 90.6967 | 4.910061933 |
| 7       | 5.06133 | 98.7882 | 5.348112779 | 5.07927     | 98.4394 | 5.329229736 | 4.77073 | 104.806 | 5.673899391 |
| 8       | 5.03786 | 99.2486 | 5.373037528 | 5.42627     | 92.1443 | 4.988430888 | 4.55882 | 109.677 | 5.937601507 |
| 9       | 6.03612 | 82.8347 | 4.484435566 | 5.46275     | 91.529  | 4.955120293 | 5.08435 | 98.341  |  5.32390264 |
| 10      | 5.67919 | 88.0408 | 4.766279045 | 5.02307     | 99.5407 | 5.388850993 | 4.89735 | 102.096 | 5.527187683 |
| 11      | 5.28972 | 94.523  | 5.117206956 | 4.69986     | 106.386 | 5.759436107 | 4.54062 | 110.117 | 5.961421858 |
| 12      | 4.91913 | 101.644 | 5.502717686 | 4.32977     | 115.479 | 6.251705321 | 4.16905 | 119.932 | 6.492778103 |
| 13      | 4.67929 | 106.854 | 5.784772299 | 4.12791     | 121.127 | 6.557472011 | 3.91064 | 127.856 | 6.921760974 |
| 14      | 4.42426 | 113.013 | 6.118203079 | 3.85676     | 129.642 | 7.018449945 | 3.64531 | 137.163 |  7.42561554 |
| 15      | 4.19794 | 119.106 | 6.448060807 | 3.55873     | 140.499 | 7.606217112 | 3.58546 | 139.452 | 7.549535503 |
| 16      | 4.14617 | 120.593 | 6.528562767 | 3.93503     | 127.064 | 6.878884341 | 3.74291 | 133.586 | 7.231966911 |

![alt text](https://github.com/arthurliu2022/IN203/blob/main/Projet/results/results.png?raw=true)

On constate des résultats relativement proches. La performance la plus élevée a été atteinte sur l'implémentation avec OMP+Threads, avec 15 threads. Le speedup obtenu est de 7.606217112, ce qui permet d'afficher la simulation à 139.5 FPS (images par secondes).

En moyenne, l'implémentation MPI a des meilleures performances en nombre de threads élevé.

L'implémentation la plus lente est celle avec OMP seul.

On constate une baisse des performances après 8 cœurs puis une remontée. Ceci est dû au passage en hyperthreading (mon processeur ayant 8 cœurs 16 threads).

La différence entre l'implémentation OMP+Threads et OMP seul est faible à nombre de cœurs faibles, en effet, avec mon ordinateur, l'affichage prend seulement 1ms, par rapport à un calcul qui prend plus de 50ms à un cœur. Donc paralléliser l'affichage et le calcul ne change presque rien au départ. La différence devient plus marquante à nombre de cœurs élevés.
