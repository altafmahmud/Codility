#include "Solutions.hpp"

int main()
{
    MaxSumOfTiles maxSum;
    std::vector int_vec = {2, 3, 5, 2, 3, 4, 6, 4, 1};
    std::cout << "Maximum sum of tiles: " << maxSum.solution(int_vec) << '\n';
    int_vec = {1, 5, 3, 2, 6, 6, 10, 4, 7, 2, 1};
    std::cout << "Maximum sum of tiles: " << maxSum.solution(int_vec) << '\n';
    int_vec = {1, 2, 3, 3, 2};
    std::cout << "Maximum sum of tiles: " << maxSum.solution(int_vec) << '\n';
    int_vec = {5, 10, 3};
    std::cout << "Maximum sum of tiles: " << maxSum.solution(int_vec) << '\n';
    int_vec = {0, 0, 0, 0, 1};
    std::cout << "Maximum sum of tiles: " << maxSum.solution(int_vec) << '\n';

    SmallestMissingInteger smallest;
    int_vec = {1, 3, 6, 4, 1, 2};
    std::cout << "Smallest positive integer: " << smallest.smallestMissingInteger(int_vec) << '\n';
    int_vec = {1, 2, 3};
    std::cout << "Smallest positive integer: " << smallest.smallestMissingInteger(int_vec) << '\n';
    int_vec = {-1, -3, 1};
    std::cout << "Smallest positive integer: " << smallest.smallestMissingInteger(int_vec) << '\n';
    
    return 0;
}
