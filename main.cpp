#include <deque>
#include <iostream>
#include <mpi.h>
#include <random>
#include <vector>

#define MCW MPI_COMM_WORLD

// Program Settings
static const int ROUNDS = 10; 
static const int TRADE_ITERATIONS = 10; 
static const int PRODUCTIVITY = 10; 
static const int CONSUMPTION = 10; 

double getTotalUtility(int productionProfile, std::deque<int>& goods)
{
    double utility = 0.0; 

    return utility; 
}

void consumeGoods(int howMany, std::deque<int>& goods, int& myConsumption)
{
    for(int i = 0; i < howMany; i++)
    {
        myConsumption += goods.front();
        goods.pop_front(); 
    }
}

void produceGoods(int productionProfile, int howMany, std::deque<int>& goods, int size)
{
    for(int i = 0; i < howMany; i++)
    {
        goods.push_back(rand() % size); 
    }
}

int main(int argc, char** argv)
{
    int rank, size, data; 
    int myConsumption = 0; 

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    srand(rank); 

    int productionProfile = rand() % size; 
    int preferenceProfile = rand() % size; 
    std::deque<int> goods; 

    std::cout << "Rank " << rank << "- profile: " << preferenceProfile << 
    " | preference: " << productionProfile << std::endl; 

    for(int iteration = 0; iteration < TRADE_ITERATIONS; iteration++)
    {
        produceGoods(productionProfile, PRODUCTIVITY, goods, size); 
        //trade(); 
        consumeGoods(CONSUMPTION, goods, myConsumption);
    }

    MPI_Send(&rank, 1, MPI_INT, (rank+1)%size, 0, MCW);
    MPI_Recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);

    std::cout << "I am rank " << rank << " of " << size
             << "; my message came from "<< data << std::endl;

    MPI_Finalize();

    return 0;
}

