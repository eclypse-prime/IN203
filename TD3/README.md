

# TP3 de Arthur LIU

`pandoc -s --toc tp2.md --css=./github-pandoc.css -o tp2.html`





## lscpu

Architecture:        x86_64
CPU op-mode(s):      32-bit, 64-bit
Byte Order:          Little Endian
Address sizes:       36 bits physical, 48 bits virtual
CPU(s):              16
On-line CPU(s) list: 0-15
Thread(s) per core:  2
Core(s) per socket:  8
Socket(s):           1
Vendor ID:           AuthenticAMD
CPU family:          23
Model:               96
Model name:          AMD Ryzen 7 4800H with Radeon Graphics
Stepping:            1
CPU MHz:             2900.000
CPU max MHz:         2900.0000
BogoMIPS:            5800.00
Virtualization:      AMD-V
Hypervisor vendor:   Windows Subsystem for Linux
Virtualization type: container
Flags:               fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush mmx fxsr sse sse2 h
                     t syscall nx mmxext fxsr_opt pdpe1gb rdtscp lm pni pclmulqdq monitor ssse3 fma cx16 sse4_1 sse4_2 m
                     ovbe popcnt aes xsave osxsave avx f16c rdrand lahf_lm cmp_legacy svm extapic cr8_legacy abm sse4a m
                     isalignsse 3dnowprefetch osvw ibs skinit wdt tce topoext perfctr_core perfctr_nb bpext perfctr_llc
                     mwaitx fsgsbase bmi1 avx2 smep bmi2 cqm rdt_a rdseed adx smap clflushopt clwb sha_ni umip rdpid



## Produit scalaire 

*Expliquer les paramètres, les fichiers, l'optimisation de compil, NbSamples, ...*

OMP_NUM    | samples=1024 | 
-----------|--------------|----------
séquentiel |   | 
1          |   | 
2          |   | 
3          |   | 
4          |   | 
8          |   | 


*Discuter sur ce qu'on observe, la logique qui s'y cache.*




## Produit matrice-matrice



### Permutation des boucles

*Expliquer comment est compilé le code (ligne de make ou de gcc) : on aura besoin de savoir l'optim, les paramètres, etc. Par exemple :*

`make TestProduct.exe && ./TestProduct.exe 1024`


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
