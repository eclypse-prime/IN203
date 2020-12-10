

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

  ordre           | time    | MFlops  | MFlops(n=2048) 
------------------|---------|---------|----------------
i,j,k (origine)   | 2.73764 | 782.476 |                
j,i,k             |  |  |    
i,k,j             |  |  |    
k,i,j             |  |  |    
j,k,i             |  |  |    
k,j,i             |  |  |    


*Discussion des résultats*



### OMP sur la meilleure boucle 

`make TestProduct.exe && OMP_NUM_THREADS=8 ./TestProduct.exe 1024`

  OMP_NUM         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
1                 |  |
2                 |  |
3                 |  |
4                 |  |
5                 |  |
6                 |  |
7                 |  |
8                 |  |




### Produit par blocs

`make TestProduct.exe && ./TestProduct.exe 1024`

  szBlock         | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
------------------|---------|----------------|----------------|---------------
origine (=max)    |  |
32                |  |
64                |  |
128               |  |
256               |  |
512               |  | 
1024              |  |




### Bloc + OMP



  szBlock      | OMP_NUM | MFlops  | MFlops(n=2048) | MFlops(n=512)  | MFlops(n=4096)
---------------|---------|---------|------------------------------------------------
A.nbCols       |  1      |         | 
512            |  8      |         | 







# Tips 

```
	env 
	OMP_NUM_THREADS=4 ./dot_product.exe
```

```
    $ for i in $(seq 1 4); do elap=$(OMP_NUM_THREADS=$i ./TestProductOmp.exe|grep "Temps CPU"|cut -d " " -f 7); echo -e "$i\t$elap"; done > timers.out
```
