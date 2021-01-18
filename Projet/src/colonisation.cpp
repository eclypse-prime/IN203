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

    std::cout << "Resume des parametres (proba par pas de temps): " << std::endl;
    std::cout << "\t Chance apparition civilisation techno : " << param.apparition_civ << std::endl;
    std::cout << "\t Chance disparition civilisation techno: " << param.disparition << std::endl;
    std::cout << "\t Chance expansion : " << param.expansion << std::endl;
    std::cout << "\t Chance inhabitable : " << param.inhabitable << std::endl;
    std::cout << "Proba minimale prise en compte : " << 1./RAND_MAX << std::endl;
    std::srand(std::time(nullptr));

    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

    window = SDL_CreateWindow("Galaxie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                width, height, SDL_WINDOW_SHOWN);

    galaxie g(width, height, param.apparition_civ);
    galaxie g_next(width, height);
    galaxie_renderer gr(window);

    int deltaT = (20*52840)/width;
    std::cout << "Pas de temps : " << deltaT << " annees" << std::endl;
    
    std::cout << std::endl;

    unsigned long long temps = 0;

    //Utilisation de steady_clock pour une meilleure précision lorsque la vitesse de calcul est élevée
    std::chrono::steady_clock::time_point start, end;

    while(1) {
        // Debut mesure temps total calcul + affichage
        start = std::chrono::steady_clock::now();

        // Création d'un thread affichage
        std::thread rend(&galaxie_renderer::render, &gr, g);

        //Calcul de la galaxie suivante
        mise_a_jour(param, width, height, g.data(), g_next.data());

        // Synchronisation avec le threads d'affichage
        rend.join();

        g_next.swap(g);

        // Fin mesure temps total calcul + affichage
        end = std::chrono::steady_clock::now();

        std::chrono::duration<double> elaps = end - start;
        
        temps += deltaT;

        std::cout << "Temps passe : "
                << std::setw(10) << temps << " annees"
                << std::fixed << std::setprecision(3)
                << "  " << "|  CPU(ms) : total " << elaps.count() * 1000
                << "  " << 1/elaps.count() << " FPS   "
                << "\r" << std::flush;
        
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
            std::cout << std::endl << "The end" << std::endl;
            break;
        }
    }


    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
