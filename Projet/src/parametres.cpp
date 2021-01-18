#include <cstdlib>
#include <cassert>
#include <ctime>
#include <iostream>
#include <random>
#include <omp.h>
#include "galaxie.hpp"
#include "parametres.hpp"

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

expansion calcul_expansion(const parametres& c)
{
    double val = random_threaded_double();
    if (val < 0.01*c.expansion)     // parmi c.expansion, on a 1% de chance d'expansion isotrope...
        return expansion_isotrope;
    if (val < c.expansion)          // ... et 99% de chance d'expansion dans 1 seule direction
        return expansion_unique;
    return pas_d_expansion;
}
//_ ______________________________________________________________________________________________ _
bool calcul_depeuplement(const parametres& c)
{
    double val = random_threaded_double(); 
    if (val < c.disparition)
        return true;
    return false;
}
//_ ______________________________________________________________________________________________ _
bool calcul_inhabitable(const parametres& c)
{
    double val = random_threaded_double();
    if (val < c.inhabitable)
        return true;
    return false;
}
//_ ______________________________________________________________________________________________ _
bool apparition_technologie(const parametres& p)
{
    double val = random_threaded_double(); 
    if (val < p.apparition_civ)
        return true;
    return false;
}
//_ ______________________________________________________________________________________________ _
bool a_un_systeme_proche_colonisable(int i, int j, int width, int height, const char* galaxie)
{
    assert(i >= 0);
    assert(j >= 0);
    assert(i < height);
    assert(j < width);

    bool isNeighboor = false;


    if ( (i>0) && (galaxie[(i-1)*width+j] == habitable)) isNeighboor = true;
    if ( (i<height-1) && (galaxie[(i+1)*width+j] == habitable)) isNeighboor = true;
    if ( (j>0) && (galaxie[i*width+j-1] == habitable)) isNeighboor = true;
    if ( (j<width-1) && (galaxie[i*width+j+1] == habitable)) isNeighboor = true;

    return isNeighboor;
}
//_ ________________________________________________________________________


void 
mise_a_jour(const parametres& params, int width, int height, const char* galaxie_previous, char* galaxie_next)
{
    memcpy(galaxie_next, galaxie_previous, width*height*sizeof(char));
    #pragma omp parallel for simd
    for (int i = 0; i < height; ++i)
    {
        for (int j = 0; j < width; ++j)
        {
            if (galaxie_previous[i*width+j] == habitee)
            {
                if ( a_un_systeme_proche_colonisable(i, j, width, height, galaxie_previous) )
                {
                    expansion e = calcul_expansion(params);
                    if (e == expansion_isotrope)
                    {
                      if ( (i > 0) && (galaxie_previous[(i-1)*width+j] != inhabitable) )
                        {
                            galaxie_next[(i-1)*width+j] = habitee;
                        }
                      if ( (i < height-1) && (galaxie_previous[(i+1)*width+j] != inhabitable) )
                        {
                            galaxie_next[(i+1)*width+j] = habitee;
                        }
                      if ( (j > 0) && (galaxie_previous[i*width+j-1] != inhabitable) )
                        {
                            galaxie_next[i*width+j-1] = habitee;
                        }
                      if ( (j < width-1) && (galaxie_previous[i*width+j+1] != inhabitable) )
                        {
                            galaxie_next[i*width+j+1] = habitee;
                        }
                    }
                    else if (e == expansion_unique)
                    {
                        // Calcul de la direction de l'expansion :
                        int ok = 0;
                        do
                        {
                            int dir = random_direction();
                            if ( (i>0) && (dir == 0) && (galaxie_previous[(i-1)*width+j] != inhabitable) )
                            {
                                galaxie_next[(i-1)*width+j] = habitee;
                                ok = 1;
                            }
                            if ( (i<height-1) && (dir == 1) && (galaxie_previous[(i+1)*width+j] != inhabitable) )
                            {
                                galaxie_next[(i+1)*width+j] = habitee;
                                ok = 1;
                            }
                            if ( (j>0) && (dir == 2) && (galaxie_previous[i*width+j-1] != inhabitable) )
                            {
                                galaxie_next[i*width+j-1] = habitee;
                                ok = 1;
                            }
                            if ( (j<width-1) && (dir == 3) && (galaxie_previous[i*width+j+1] != inhabitable) )
                            {
                                galaxie_next[i*width+j+1] = habitee;
                                ok = 1;
                            }
                        } while (ok == 0);
                    }// End if (e == expansion_unique)
                }// Fin si il y a encore un monde non habite et habitable
                if (calcul_depeuplement(params))
                {
                    galaxie_next[i*width+j] = habitable;
                }
                if (calcul_inhabitable(params))
                {
                    galaxie_next[i*width+j] = inhabitable;
                }
            }  // Fin si habitee
            else if (galaxie_previous[i*width+j] == habitable)
            {
                if (apparition_technologie(params))
                    galaxie_next[i*width+j] = habitee;
            }
            else { // inhabitable
              // nothing to do : le systeme a explose
            }
            // if (galaxie_previous...)
        }// for (j)
    }// for (i)

}
//_ ______________________________________________________________________________________________ _
