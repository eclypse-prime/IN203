

# TP3 de Arthur LIU

`pandoc -s --toc tp2.md --css=./github-pandoc.css -o tp2.html`





## lscpu

Architecture:        x86_64\
CPU op-mode(s):      32-bit, 64-bit\
Byte Order:          Little Endian\
Address sizes:       36 bits physical, 48 bits virtual\
CPU(s):              16\
On-line CPU(s) list: 0-15\
Thread(s) per core:  2\
Core(s) per socket:  8\
Socket(s):           1\
Vendor ID:           AuthenticAMD\
CPU family:          23\
Model:               96\
Model name:          AMD Ryzen 7 4800H with Radeon Graphics\
Stepping:            1\
CPU MHz:             2900.000\
CPU max MHz:         2900.0000\
BogoMIPS:            5800.00\
Virtualization:      AMD-V\
Hypervisor vendor:   Windows Subsystem for Linux\
Virtualization type: container\
Flags:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 h
                     t syscall nx mmxext fxsr_opt pdpe1gb rdtscp lm pni pclmulqdq monitor ssse3 fma cx16 sse4_1 sse4_2 m
                     ovbe popcnt aes xsave osxsave avx f16c rdrand lahf_lm cmp_legacy svm extapic cr8_legacy abm sse4a m
                     isalignsse 3dnowprefetch osvw ibs skinit wdt tce topoext perfctr_core perfctr_nb bpext perfctr_llc
                     mwaitx fsgsbase bmi1 avx2 smep bmi2 cqm rdt_a rdseed adx smap clflushopt clwb sha_ni umip rdpid\



## Produit scalaire 

`make dotproduct.exe; export OMP_NUM_THREADS=8; ./dotproduct.exe`

On ajoute avant la boucle for la ligne suivante pour paralléliser le produit scalaire
```C++
#pragma omp parallel for reduction (+:scal)
for ( size_t i = 0; i < u.size(); ++i ) {
    scal += u[i]*v[i];
}
```

OMP_NUM    | samples=1024 | speedup
-----------|--------------|-----
séquentiel | 0.0818299  | 1
1          | 0.0892672  | 0.92
2          | 0.0535898  | 1.53
3          | 0.0448287  | 1.82
4          | 0.0439164  | 1.86
8          | 0.0436214  | 1.88


À nombre de threads égaux, le code avec OpenMP est plus rapide que celui avec threads. 

Le speedup est très faible comparé au nombre de threads, car le produit scalaire est memory-bound, augmenter le nombre de threads améliore donc très peu les performances.

En effet, l'opération effectuée est
``` C++
for ( size_t i = 0; i < u.size(); ++i ) {
    scal += u[i]*v[i];
}
```
Il y a deux accès mémoire et deux opérations par boucle, donc le produit scalaire est memory-bound.





## Produit matrice-matrice



### Permutation des boucles

`make TestProduct.exe && ./TestProduct.exe 1024`

On lance TestProduct.exe avec l'ordre de base des boucles, pour les dimensions 1023, 1024, et 1025.

  dimension          | time    | MFlops  
------------------|---------|---------
1023   | 0.909666 | 2353.83           
1024   | 9.17872 | 233.963     
1025   | 0.980797 | 2195.95     

On constate que l'exécution avec une dimension de 1024 est beaucoup plus lente. Cela s'explique par le fait que à chaque saut de colonne, le cache doit se vider puis se réécrire au même endroit, car 1024 est un multiple du nombre de blocs mémoire dans le cache. Avec une dimension de 1023 ou 1025, ce problème n'existe pas. 

  ordre           | time    | MFlops  
------------------|---------|---------
i,j,k (origine)   | 9.53381 | 225.249              
j,i,k             | 9.19872 | 233.455   
i,k,j             | 30.4662 | 70.4875 
k,i,j             | 34.473  | 62.2947   
j,k,i             | 0.62746 | 3422.45 
k,j,i             | 0.947062| 2267.52 

Sachant que les valeurs des matrices sont stockées sous forme Column Major sur un vecteur, il faut accéder aux valeurs des matrices le plus séquenciellement possible. Il faut donc parcourir toutes les matrices colonne par colonne. Les ordres j,k,i et k,j,i sont optimaux.\
Par la suite, on constate en parallélisant qu'avec l'ordre k,j,i il y a de la concurrence d'accès, que l'on peut régler en plaçant un `#pragma omp atomic` mais cela diminue considérablement les performances. On utilise donc finalement l'ordre j,k,i.

### OMP sur la meilleure boucle 

`make TestProductMatrix.exe`\
`for ((i=1;i<=8;i++)); do OMP_NUM_THREADS=$i; ./TestProductMatrix.exe 1024; done`\
`for ((i=1;i<=8;i++)); do OMP_NUM_THREADS=$i; ./TestProductMatrix.exe 2048; done`\
`for ((i=1;i<=8;i++)); do OMP_NUM_THREADS=$i; ./TestProductMatrix.exe 512; done`\
`for ((i=1;i<=8;i++)); do OMP_NUM_THREADS=$i; ./TestProductMatrix.exe 4096; done`

  OMP_NUM_THREADS | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
1                 | 3366.12 | 3276.19 | 3942.55 | 3330.35
2                 | 6614.45 | 6561.51 | 7804.47 | 6483.75
3                 | 9156.41 | 9680.42 | 11249.7 | 9480.16
4                 | 11893.7 | 11672.5 | 14251   | 12116.5
5                 | 14817.5 | 13555   | 16992.6 | 13494
6                 | 14943.8 | 13689.1 | 11427.8 | 13662.4
7                 | 15841.1 | 13918.8 | 17235.6 | 12352.5	
8                 | 17070.8 | 14271.8 | 19454.9 | 12079.9
16                | 25887.1 | 19885.3 | 24660.8 | 12019.1

On constate qu'à n=4096,  les performances chutent rapidement à nombre de cœurs élevés.


### Produit par blocs

`make TestMatrixProduct.exe`\ après avoir changé szBlock dans ProdMatMat.cpp
`export OMP_NUM_THREADS=16`\
`./TestMatrixProduct.exe`\
`for i in 1024 2048 512 4096; do ./TestProductMatrix.exe $i; done`

  szBlock         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)| MFlops moyen
------------------|---------|----------------|----------------|---------------|--------
origine (=max)    | 26124.7 | 18822.7 | 22404   | 11829.5 | 19795,225
32                | 12516.3 | 12133   | 11871.7 | 11367.9 | 11972,225
64                | 22318.9 | 20484.5 | 18086.1 | 20002.4 | 20222,975
128               | 24144.8 | 17846.6 | 22336.3 | 19345.6 | 20918,325
256               | 26400.1 | 24448   | 23821.8 | 25079.5 | 24937,35
512               | 24977.7 | 23286.6 | 23657.8 | 25191.5 | 24278,4
1024              | 26175.4 | 23417.5 | 22593.1 | 22865.3 | 23762,825

On constate la meilleure performance en moyenne pour n=256.\
On peut tout de même améliorer la performance encore plus car le produit par blocs n'est pas parallélisé.

### Bloc + OMP
`make TestMatrixProduct.exe`\ après avoir changé szBlock dans ProdMatMat.cpp
`export OMP_NUM_THREADS=8; for i in 1024 2048 512 4096; do ./TestProductMatrix.exe $i; done; export OMP_NUM_THREADS=16; for i in 1024 2048 512 4096; do ./TestProductMatrix.exe $i; done`

  szBlock      | OMP_NUM | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
---------------|---------|---------|----------------|----------------|---------------

| szBlock | OMP_NUM_THREADS | MFlops  | MFlops(n=2048) | MFlops(n=512) | MFlops(n=4096) | MFlops moyen |
|---------|-----------------|---------|----------------|---------------|----------------|--------------|
|A.nbCols | 1               | 3517.63 | 3342.21        | 4002.79       | 3409.7         | 3568.0825    |
| 32      | 8               | 16217.4 | 23128.7        | 18191.1       | 21766.5        | 19825.925    |
| 32      | 16              | 27656.3 | 27827.2        | 22544.7       | 26565.9        | 26148.525    |
| 64      | 8               | 19734.9 | 21082.9        | 15794.7       | 21264.8        | 19469.325    |
| 64      | 16              | 27731.9 | 28074.6        | 18977.9       | 28232.3        | 25754.175    |
| 128     | 8               | 24217.4 | 22662          | 14229.7       | 23203          | 21078.025    |
| 128     | 16              | 20616.4 | 29814.4        | 9838.46       | 22739.4        | 20752.165    |
| 256     | 8               | 15812.4 | 20158.4        | 8525.6        | 20377          | 16218.35     |
| 256     | 16              | 17175.5 | 21491.1        | 8296.8        | 11257.5        | 14555.225    |
| 512     | 8               | 7342.11 | 14003.2        | 4508.85       | 12400.5        | 9563.665     |
| 512     | 16              | 8034.72 | 13979.9        | 4463.85       | 11582.7        | 9515.2925    |
| 1024    | 8               | 3768.48 | 6061.4         | 4639          | 11081.1        | 6387.495     |
| 1024    | 16              | 3724.58 | 6062.1         | 4488.7        | 11019.6        | 6323.745     |

Ici on constate que les performances sont meilleures pour n = 32.

On compare la version scalaire parallélisée à la meilleure version (szBlock = 32) par blocs parallélisée, le tout à 16 threads.

Version                | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096) | MFlops moyen |
-----------------------|---------|----------------|----------------|----------------|--------------|
Scalaire               | 25887.1 | 19885.3        | 24660.8        | 12019.1        | 20613.075    |
Par blocs (szBlock=32) | 27656.3 | 27827.2        | 22544.7        | 26565.9        | 26148.525    |

On constate que la version par blocs est bien plus rapide que la version scalaire, surtout pour les n grands.

## Ensemble de Bhudda

`make bhudda.exe`\
`for i in 1 2 3 4 6 8 12 16; do export OMP_NUM_THREADS=$i; ./bhudda.exe; done`
