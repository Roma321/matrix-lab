#include <stdio.h>
#include "mpi.h"
#include <chrono>
#include <stdio.h>
#include "mpi.h"
#include <iostream>
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
  int squareSize = 1680;
  int **first = (int **)malloc(squareSize * sizeof(int *));
  int **second = (int **)malloc(squareSize * sizeof(int *));
  int *g = (int *)malloc(squareSize * squareSize * sizeof(int));
  for (int i = 0; i < squareSize; i++)
  {
    first[i] = (int *)malloc(squareSize * sizeof(int));
    second[i] = (int *)malloc(squareSize * sizeof(int));
  }

  for (int i = 0; i < squareSize; i++)
  {
    for (int j = 0; j < squareSize; j++)
    {
      first[i][j] = rand() % 100 - 50;
      second[i][j] = rand() % 100 - 50;
    }
  }
  int threadNumber, threadAmount;
  int ibeg, iend;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &threadAmount);
  MPI_Comm_rank(MPI_COMM_WORLD, &threadNumber);

  ibeg = threadNumber * (squareSize / threadAmount);
  iend = (threadNumber + 1) * (squareSize / threadAmount);
  auto start = std::chrono::steady_clock::now();

  for (int rowFirst = ibeg; rowFirst < iend; rowFirst++)
  {
    for (int columnSecond = 0; columnSecond < squareSize; columnSecond++)
    {
      for (int index = 0; index < squareSize; index++)
      {
        g[rowFirst * squareSize + columnSecond] += first[rowFirst][index] * second[index][columnSecond];
      }
    }
  }
  MPI_Barrier(MPI_COMM_WORLD);
  int *gatheredRes = nullptr;
  if (threadNumber == 0)
  {
    gatheredRes = new int[squareSize * squareSize];
  }
  if (threadNumber == 0)
    std::cout << "Elapsed time(ms)=" << since(start).count() << std::endl;

  MPI_Gather(&(g[ibeg * squareSize]), (iend - ibeg) * squareSize, MPI_INT,
             gatheredRes, (iend - ibeg) * squareSize, MPI_INT, 0, MPI_COMM_WORLD);

  MPI_Finalize();
  for (int i = 0; i < squareSize; i++)
  {
    free(first[i]);
    free(second[i]);
  }
  free(first);
  free(second);
  free(g);
  delete[] gatheredRes;
  return 0;
}
