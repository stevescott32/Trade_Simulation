Final Project for Parallel Programming
Steven Scott
CS 5500, Dan Watson

Simulation of the Economic Gains from Trade using Parallel Processors 

Set-up: this simulation is designed to display the economic gains from trade. Each processor represents an individual in a market economy. Each individual has a profile, which defines its preference set and production function. Basically, an individual has the ability to make certain products/services. Additionally, each individual prefers some products more than others. To make  these preferences easier to compare quickly, each individual has a value function that takes a given set of products/services and returns a "dollar" value - a higher dollar value represents a greater amount of utility. Individuals prefer more utility to less utility. 

Instead of having a list of products/services to trade, goods are represented by integers. This simplifies message passing and maintains generality.   

