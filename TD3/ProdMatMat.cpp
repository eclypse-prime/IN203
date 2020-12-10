#include <algorithm>
#include <cassert>
#include <iostream>
#include <thread>
#if defined(_OPENMP)
#include <omp.h>
#endif
#include "ProdMatMat.hpp"

namespace {


void prodSubBlocks(int iRowBlkA, int iColBlkB, int iColBlkA, int szBlock,
                   const Matrix& A, const Matrix& B, Matrix& C) {

  //#pragma omp parallel for
  for (int j = iColBlkB; j < std::min(B.nbCols, iColBlkB + szBlock); j++) //2
    for (int k = iColBlkA; k < std::min(A.nbCols, iColBlkA + szBlock); k++) //3
      for (int i = iRowBlkA; i < std::min(A.nbRows, iRowBlkA + szBlock); ++i) //1
        C(i, j) += A(i, k) * B(k, j);
}

void prodBlocks (int szBlock, const Matrix& A, const Matrix& B, Matrix& C) {
  //#pragma omp parallel for
  for (int jj = 0; jj < B.nbCols; jj += szBlock)
    for (int kk = 0; kk < A.nbCols; kk += szBlock)
      for (int ii = 0; ii < A.nbRows; ii += szBlock)
        prodSubBlocks(ii, jj, kk, szBlock, A, B, C);
}
const int szBlock = 32;
}  // namespace

Matrix operator*(const Matrix& A, const Matrix& B) {
  Matrix C(A.nbRows, B.nbCols, 0.0);
  prodBlocks(32, A, B, C);
  //prodSubBlocks(0, 0, 0, std::max({A.nbRows, B.nbCols, A.nbCols}), A, B, C);
  return C;
}
