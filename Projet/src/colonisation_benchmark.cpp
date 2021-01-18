//Benchmark OMP + Threads

#include <cstdlib>
#include <string>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <ctime>
#include <iomanip>      // std::setw
#include <chrono>

#include "parametres.hpp"
#include "galaxie.hpp"

#include <thread>
#include <omp.h>

int main(int argc, char ** argv)
{
    char commentaire[4096];
    int width, height;
    SDL_Event event;
    SDL_Window   * window;

    parametres param;


    std::ifstream fich("parametre.txt");
    fich >> width;
    fich.getline(commentaire, 4096);
    fich >> height;
    fich.getline(commentaire, 4096);
    fich >> param.apparition_civ;
    fich.getline(commentaire, 4096);
    fich >> param.disparition;
    fich.getline(commentaire, 4096);
    fich >> param.expansion;
    fich.getline(commentaire, 4096);
    fich >> param.inhabitable;
    fich.getline(commentaire, 4096);
    fich.close();

    std::srand(std::time(nullptr));

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Galaxie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                width, height, SDL_WINDOW_SHOWN);

    galaxie g(width, height, param.apparition_civ);
    galaxie g_next(width, height);
    galaxie_renderer gr(window);



    //Utilisation de steady_clock pour une meilleure précision lorsque la vitesse de calcul est élevée
//    std::chrono::steady_clock::time_point start1, start2, end1, end2, start, end;
    std::chrono::steady_clock::time_point global_start, global_end;

    global_start = std::chrono::steady_clock::now();

    for (int i = 0; i < 500; i++) {
        
        std::thread rend(&galaxie_renderer::render, &gr, g);

        mise_a_jour(param, width, height, g.data(), g_next.data());

        rend.join();

        g_next.swap(g);

        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            std::cout << std::endl << "The end" << std::endl;
            break;
        }
    }

    global_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> global_elaps = global_end - global_start;
    std::cout << omp_get_max_threads() << ", " << global_elaps.count() << ", " << 500/global_elaps.count() << std::endl;


    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
