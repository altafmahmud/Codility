#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <numeric>

// There is an array A of N integers and three tiles. Each tile can cover two neighboring numbers from the array but cannot intersect with another tile.
// It also cannot be placed outside the array, even partially.

// Write a function:
// 	    Int solution(vector<int> &A);
// that, given an array A of N integers, returns the maximum sum of numbers that can be covered using at most three tiles.

// Examples:
// 1.	Given A = [2, 3, 5, 2, 3, 4, 6, 4, 1], the function should return 25. There is only one optimal placement of tiles: (3, 5), (3, 4), (6, 4).
// 2.	Given A = [1, 5, 3, 2, 6, 6, 10, 4, 7, 2, 1], the function should return 35. One of the three optimal placement of tiles is: (5, 3), (6, 10), (4, 7).
// 3.	Given A = [1, 2, 3, 3, 2], the function should return 10. There is one optimal placement of tiles: (2, 3), (3, 2). Only two tiles can be used because A is too small to contain another one.
// 4.	Given A = [5, 10, 3], the function should return 15. Only one tile can be used.

// Write an efficient algorithm for the following assumptions:
// •	N is an integer within the range [2.. 100,000]
// •	Each element of array A is an integer within the range [0.. 1,000,000]

class MaxSumOfTiles
{
public:
    [[nodiscard]] int solution(std::vector<int> &A)
    {
        const auto maxPairs = ((A.size() / 2) > 3) ? 3 : (A.size() / 2);
        std::vector<int> resVec;
        resVec.reserve(maxPairs * 2);
        for (auto idx = 0; idx < A.size(); ++idx)
        {
            indexMap.emplace(std::make_pair(A[idx], idx));
        }
        auto maxSum = 0;
        do
        {
            auto resVecIdx = 0;
            resVec.clear();
            bookedIdxMap.clear();
            blockerIdxSet.clear();
            isAlternativeTaken = false;
            for (const auto &[num, numIdx] : indexMap)
            {
                const auto bookedBy = indexBookedBy(numIdx);
                if (bookedBy >= 0)
                {
                    saveAsBlockerIdx(bookedBy);
                    continue;
                }
                const auto neighbourIdx = getMaxNeighbourIdx(A, numIdx);
                if (neighbourIdx < 0)
                {
                    continue;
                }
                resVec.push_back(num);
                resVec.push_back(A[neighbourIdx]);
                bookIndex(neighbourIdx, numIdx);
                resVecIdx += 2;
                if (resVecIdx >= resVec.capacity())
                {
                    break;
                }
            }
            const auto sum = std::accumulate(resVec.begin(), resVec.end(), 0, std::plus<>());
            maxSum = (sum > maxSum) ? sum : maxSum;
            // Alternative pairs will only be tried for those pairs who blocked either other candidates
            // to be selected in the ordered (desc.) array, or to choose their preferred neighbours,
            // all other possible alternatives will be removed
            auto altItr = alternativesMap.begin();
            while (altItr != alternativesMap.end())
            {
                auto index = altItr->first;
                if (!isBlocker(index))
                {
                    altItr = alternativesMap.erase(altItr);
                }
                else
                {
                    ++altItr;
                }
            }
        } while (!alternativesMap.empty());

        // Only for multiple executions of the program
        indexMap.clear();
        bookedIdxMap.clear();
        triedAlternativesMap.clear();

        return maxSum;
    }

private:
    [[nodiscard]] int getMaxNeighbourIdx(const std::vector<int> &A, const int index) noexcept
    {
        if (!isAlternativeTaken)
        {
            const auto itr = alternativesMap.find(index);
            if (itr != alternativesMap.end())
            {
                const auto idx = itr->second;
                alternativesMap.erase(index);
                blockerIdxSet.erase(index);
                saveAsTried(index, idx);
                isAlternativeTaken = true;
                return idx;
            }
        }
        auto neighbour1 = -1;
        auto neighbour2 = -1;
        auto idx = -1;
        if (index - 1 >= 0)
        {
            neighbour1 = A[index - 1];
        }
        if (index + 1 < A.size())
        {
            neighbour2 = A[index + 1];
        }
        auto neighbour1BookedBy = indexBookedBy(index - 1);
        auto neighbour2BookedBy = indexBookedBy(index + 1);
        if (neighbour1BookedBy >= 0 && neighbour2BookedBy >= 0)
        {
            return idx;
        }
        if (neighbour1 < 0 && neighbour2BookedBy >= 0)
        {
            return idx;
        }
        if (neighbour2 < 0 && neighbour2BookedBy >= 0)
        {
            return idx;
        }
        idx = (neighbour1 >= neighbour2) ? decideNeighbourIndex(neighbour1BookedBy, neighbour2BookedBy, index, false) : decideNeighbourIndex(neighbour1BookedBy, neighbour2BookedBy, index, true);

        return idx;
    }

    [[nodiscard]] int decideNeighbourIndex(const int neighbour1BookedBy, const int neighbour2BookedBy, const int index, const bool preference /* False for left neighbour, True otherwise */) noexcept
    {
        auto idx = -1;
        auto bookedBy01 = -1;
        auto bookedBy02 = -1;
        auto neighbourIdx01 = -1;
        auto neighbourIdx02 = -1;
        if (preference)
        {
            bookedBy01 = neighbour2BookedBy;
            bookedBy02 = neighbour1BookedBy;
            neighbourIdx01 = index + 1;
            neighbourIdx02 = index - 1;
        }
        else
        {
            bookedBy01 = neighbour1BookedBy;
            bookedBy02 = neighbour2BookedBy;
            neighbourIdx01 = index - 1;
            neighbourIdx02 = index + 1;
        }
        if (bookedBy01 < 0)
        {
            idx = neighbourIdx01;
            if (!isAltIndexTried(index, neighbourIdx02))
            {
                tryAltNeighbourIdx(index, neighbourIdx02);
            }
        }
        else
        {
            if (bookedBy01 != neighbourIdx01)
            {
                saveAsBlockerIdx(bookedBy01);
            }
            if (bookedBy02 < 0)
            {
                idx = neighbourIdx02;
            }
        }
        return idx;
    }

    bool bookIndex(const int neighbourIndex, const int index) noexcept
    {
        return bookedIdxMap.emplace(std::make_pair(neighbourIndex, index)).second;
    }

    [[nodiscard]] int indexBookedBy(const int bookedIndex) const noexcept
    {
        const auto itr = bookedIdxMap.find(bookedIndex);
        if (itr != bookedIdxMap.end())
        {
            return itr->second;
        }
        for (const auto &[key, val] : bookedIdxMap)
        {
            if (val == bookedIndex)
            {
                return val;
            }
        }
        return -1;
    }

    bool tryAltNeighbourIdx(const int index, const int altIndex) noexcept
    {
        return alternativesMap.emplace(std::make_pair(index, altIndex)).second;
    }

    bool saveAsTried(const int index, const int altIndex) noexcept
    {
        return triedAlternativesMap.emplace(std::make_pair(index, altIndex)).second;
    }

    [[nodiscard]] bool isAltIndexTried(const int index, const int altIndex) const noexcept
    {
        const auto itr = triedAlternativesMap.find(index);
        return (itr != triedAlternativesMap.end()) ? (itr->second == altIndex) : false;
    }

    bool saveAsBlockerIdx(const int index) noexcept
    {
        return blockerIdxSet.emplace(index).second;
    }

    [[nodiscard]] bool isBlocker(const int index) const noexcept
    {
        return blockerIdxSet.find(index) != blockerIdxSet.end();
    }

    std::multimap<int, int, std::greater<>> indexMap;
    std::unordered_map<int, int> bookedIdxMap;
    std::unordered_set<int> blockerIdxSet;
    std::unordered_map<int, int> alternativesMap;
    std::unordered_map<int, int> triedAlternativesMap;
    bool isAlternativeTaken = false;
};

// Smallest missing integer
class SmallestMissingInteger
{
public:
    int smallestMissingInteger(std::vector<int> &A)
    {
        std::sort(A.begin(), A.end());
        if (A.back() <= 0)
        {
            return 1;
        }
        size_t idx = 0;
        if (A.back() > 0 && A.front() < 0)
        {
            while (idx < A.size() && A[idx] < 0)
            {
                ++idx;
            }
        }
        ++idx;
        while (idx < A.size())
        {
            if (A[idx] - A[idx - 1] > 1)
            {
                break;
            }
            ++idx;
        }
        if (idx == A.size())
        {
            return A.back() + 1;
        }
        return A[idx] - 1;
    }
};