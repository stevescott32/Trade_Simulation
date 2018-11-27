#include <deque>
#include <iostream>
#include <mpi.h>
#include <random>
#include <vector>

#define MCW MPI_COMM_WORLD

// Program Settings
static const int ROUNDS = 10; 
static const int TRADE = 10; 
static const int PRODUCTIVITY = 10; 
static const int CONSUMPTION = 10; 
static const int MAX_GOOD = 10; 

/**
 * calculate the utility for a single good
 */
double getSingleUtility(int preferenceProfile, int good) 
{
    switch(preferenceProfile) 
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
        default:
            return 5;   
    }
}

/**
 * calculate the utility for all goods owned by the individual
 */
double getTotalUtility(int preferenceProfile, std::deque<int>& goods)
{
    double utility = 0.0; 
    for(int i = 0; i < goods.size(); ++i)
    {
        utility += getSingleUtility(preferenceProfile, goods[i]); 
    }
    return utility; 
}

/**
 * removes a set of goods from the individual's ownership and adds them to myConsumption
 */ 
void consumeGoods(int howMany, std::deque<int>& goods, int preferenceProfile, int& myConsumption)
{
    for(int i = 0; i < howMany; i++)
    {
        myConsumption += getSingleUtility(preferenceProfile, goods.front());
        goods.pop_front(); 
    }
}

/** 
 * This function creates additional goods based on the individual's production profile
 */
void produceGoods(int productionProfile, int preferenceProfile, int howMany, std::deque<int>& goods, int size, int& myProduction)
{
    for(int i = 0; i < howMany; i++)
    {
        int tmp = rand() % size; 
        myProduction += getSingleUtility(preferenceProfile, tmp);
        goods.push_back(tmp); 
    }
}

/**
 * This function gets the index of the least valuable good
 */
int getLeastValuable(int preferenceProfile, std::deque<int>& goods)
{
    int index = 0; 
    for(int i = 0; i < goods.size(); i++)
    {
        if(getSingleUtility(preferenceProfile, goods[i]) < getSingleUtility(preferenceProfile, goods[index]))
            index = i; 
    }
    return index; 
}

/**
 * This function trades a set of goods with another process 
 */
void trade(int howMany, int withWhom, int preferenceProfile, std::deque<int>& goods)
{
    for(int i = 0; i < howMany; i++)
    {
        int leastIndex = getLeastValuable(preferenceProfile, goods); 
        int least = goods[leastIndex]; 
        int tmp = least; 
        MPI_Send(&least, 1, MPI_INT, withWhom, 0, MCW);
        MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
        goods[leastIndex] = tmp; 
    }
}

int main(int argc, char** argv)
{
    int rank, size, data; 
    int myConsumption = 0; 
    int myProduction = 0; 

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MCW, &rank);
    MPI_Comm_size(MCW, &size);

    srand(rank); 

    int productionProfile = rand() % size; 
    int preferenceProfile = rand() % size; 
    std::deque<int> goods; 

    for(int iteration = 0; iteration < ROUNDS; iteration++)
    {
        produceGoods(productionProfile, preferenceProfile, PRODUCTIVITY, goods, size, myProduction); 
        trade(TRADE, (rank + 1)%size, preferenceProfile, goods); 
        consumeGoods(CONSUMPTION, goods, preferenceProfile, myConsumption);
    }

    std::cout << rank << " produced $" << myProduction << " and consumed $" << myConsumption << std::endl; 

    int increase = myConsumption - myProduction; 
    MPI_Send(&increase, 1, MPI_INT, 0, 0, MCW);
    if(rank == 0)
    {
        int tmp = 0; 
        int total = 0; 
        for(int processor = 0; processor < size; processor++)
        {
            MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, 0, MCW, MPI_STATUS_IGNORE);
            total += tmp; 
        }
        std::cout << "Trade increased the value by $" << total << " using " << size << " processors" << std::endl; 
    }

    MPI_Finalize();

    return 0;
}

