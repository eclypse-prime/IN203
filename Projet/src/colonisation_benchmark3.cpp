//Benchmark MPI seulement

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

#include <omp.h>
#include <stdint.h>
#include <mpi.h>

int main(int argc, char ** argv)
{

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of processes
    int nbp;
    MPI_Comm_size(MPI_COMM_WORLD, &nbp);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

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
    



    MPI_Status status;


    if (rank == 0) {

        SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO);

        window = SDL_CreateWindow("Galaxie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    width, height, SDL_WINDOW_SHOWN);
    }

    galaxie g(width, height, param.apparition_civ);
    galaxie g_next(width, height);
    galaxie_renderer gr(window, rank);

    if (rank == 0) {
        for(int i = 1; i < nbp; i++) {
            if(i == 1) {
                MPI_Send(g.data(), width*(1+height/(nbp-1)), MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
            else if (i == nbp-1) {
                MPI_Send(g.data()+((i-2)*width*(height/(nbp-1))), width*(1+height/(nbp-1)), MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
            else {
                MPI_Send(g.data()+((i-2)*width*(height/(nbp-1))), width*(2+height/(nbp-1)), MPI_CHAR, i, 0, MPI_COMM_WORLD);
            }
        }
    }
    else {
        if(rank == 1 || rank == nbp-1) {
            galaxie g(width, 1 + height/(nbp-1));
        }
        else {
            galaxie g(width, 2 + height/(nbp-1));
        }

        if(rank == 1) {
            MPI_Recv(g.data(), width*(1+height/(nbp-1)), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        }
        else if (rank == nbp-1) {
            MPI_Recv(g.data(), width*(1+height/(nbp-1)), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        }
        else {
            MPI_Recv(g.data(), width*(2+height/(nbp-1)), MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
        }
    }

    std::chrono::steady_clock::time_point global_start, global_end;

    global_start = std::chrono::steady_clock::now();

    for(int n = 0; n < 500; n++) {

        if (rank == 0) {
            gr.render(g);
            for(int i = 1; i < nbp; i++) {
                MPI_Recv(g.data() + (i-1)*width*(height/(nbp-1)), width*(height/(nbp-1)), MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
            }
            
            for(int i = 1; i < nbp; i++) {
                if (i != 1) {
                    MPI_Send(g.data() + (i-1)*width*(height/(nbp-1)) - width, width, MPI_CHAR, i, 0, MPI_COMM_WORLD);
                }
                if (i != nbp-1) {
                    MPI_Send(g.data() + (i)*width*(height/(nbp-1)), width, MPI_CHAR, i, 0, MPI_COMM_WORLD);
                }
            }

        }
        else {
            mise_a_jour(param, width, height/(nbp-1), g.data(), g_next.data());
            if (rank == 1) {
                MPI_Send(g_next.data(), width*(height/(nbp-1)), MPI_CHAR, 0, 0, MPI_COMM_WORLD);
            }
            else {
                MPI_Send(g_next.data()+width, width*(height/(nbp-1)), MPI_CHAR, 0, 0, MPI_COMM_WORLD);

                if (rank != 1) {
                    MPI_Recv(g.data(), width, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
                }
                if (rank != nbp-1) {
                    MPI_Recv(g.data() + width*(height/(nbp-1)), width, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
                }
            }
            g_next.swap(g);
        }

        

        

        //_sleep(2000);
        if(rank == 0) {
            if (SDL_PollEvent(&event) && event.type == SDL_QUIT) {
                std::cout << std::endl << "The end" << std::endl;
                break;
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);
    global_end = std::chrono::steady_clock::now();
    std::chrono::duration<double> global_elaps = global_end - global_start;

    if (rank == 0) {
        std::cout << nbp << ", " << global_elaps.count() << ", " << 500/global_elaps.count() << std::endl;
    }

    if (rank == 0) {
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    // Finalize the MPI environment.
    MPI_Finalize();


    return MPI_SUCCESS;
}
