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
static const int MAX_GOOD = 10; 

/**
 * calculate the utility for a single good
 */
double getSingleUtility(int productionProfile, int good) 
{
    switch(productionProfile) 
    {
        case 0:
            return good;
        case 1:
           return MAX_GOOD - good; 
        case 2:
           if(good % 2 == 1)
               return 8; 
           else
               return 3;   
        case 3:
           if(good % 2 == 0)
               return 8; 
           else
               return 3;   
    }
}

/**
 * calculate the utility for all goods owned by the individual
 */
double getTotalUtility(int productionProfile, std::deque<int>& goods)
{
    double utility = 0.0; 
    for(int i = 0; i < goods.size(); ++i)
    {
        utility += getSingleUtility(productionProfile, goods[i]); 
    }

    return utility; 
}

/**
 * removes a set of goods from the individual's ownership and adds them to myConsumption
 */ 
void consumeGoods(int howMany, std::deque<int>& goods, int& myConsumption)
{
    for(int i = 0; i < howMany; i++)
    {
        myConsumption += goods.front();
        goods.pop_front(); 
    }
}

/** 
 * This function creates additional goods based on the individual's production profile
 */
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
        std::cout << rank << " produced " << PRODUCTIVITY << " more goods and now has a value of$" << getTotalUtility(productionProfile, goods) << std::endl; 
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

