#include "omp.h"
#include <iostream>
#include <chrono>
template <
    class result_t = std::chrono::milliseconds,
    class clock_t = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds>
auto since(std::chrono::time_point<clock_t, duration_t> const &start)
{
  return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}
int main(int argc, char **argv)
{
  if (argc < 2)
  {
    std::cout << "No integer argument provided." << std::endl;
    return 0;
  }

  int threads = std::stoi(argv[1]);
  int squareSize = 1680;
  int **first = (int **)malloc(squareSize * sizeof(int *));
  int **second = (int **)malloc(squareSize * sizeof(int *));
  int **res = (int **)malloc(squareSize * sizeof(int *));

  for (int i = 0; i < squareSize; i++)
  {
    first[i] = (int *)malloc(squareSize * sizeof(int));
    second[i] = (int *)malloc(squareSize * sizeof(int));
    res[i] = (int *)malloc(squareSize * sizeof(int));
  }

  for (int i = 0; i < squareSize; i++)
  {
    for (int j = 0; j < squareSize; j++)
    {
      first[i][j] = rand() % 100 - 50;
      second[i][j] = rand() % 100 - 50;
      res[i][j] = 0;
    }
  }
  auto start = std::chrono::steady_clock::now();

#pragma omp parallel for num_threads(threads)
  for (int rowFirst = 0; rowFirst < squareSize; rowFirst++)
  {
    for (int columnSecond = 0; columnSecond < squareSize; columnSecond++)
    {
      for (int index = 0; index < squareSize; index++)
      {
        res[rowFirst][columnSecond] += first[rowFirst][index] * second[index][columnSecond];
      }
    }
  }
  std::cout << "Elapsed time(ms)=" << since(start).count() << std::endl;
}