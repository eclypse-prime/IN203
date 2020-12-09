// Produit matrice-vecteur
# include <cassert>
# include <vector>
# include <iostream>
# include <mpi.h>

// ---------------------------------------------------------------------
class Matrix : public std::vector<double>
{
public:
    Matrix (int dim);
    Matrix( int nrows, int ncols );
    Matrix( const Matrix& A ) = delete;
    Matrix( Matrix&& A ) = default;
    ~Matrix() = default;

    Matrix& operator = ( const Matrix& A ) = delete;
    Matrix& operator = ( Matrix&& A ) = default;
    
    double& operator () ( int i, int j ) {
        return m_arr_coefs[i + j*m_nrows];
    }
    double  operator () ( int i, int j ) const {
        return m_arr_coefs[i + j*m_nrows];
    }
    
    std::vector<double> operator * ( const std::vector<double>& u ) const;
    std::vector<double> dot(const std::vector<double>& u, int rank, int nbp);

    std::ostream& print( std::ostream& out ) const
    {
        const Matrix& A = *this;
        out << "[\n";
        for ( int i = 0; i < m_nrows; ++i ) {
            out << " [ ";
            for ( int j = 0; j < m_ncols; ++j ) {
                out << A(i,j) << " ";
            }
            out << " ]\n";
        }
        out << "]";
        return out;
    }
private:
    int m_nrows, m_ncols;
    std::vector<double> m_arr_coefs;
};
// ---------------------------------------------------------------------
inline std::ostream& 
operator << ( std::ostream& out, const Matrix& A )
{
    return A.print(out);
}
// ---------------------------------------------------------------------
inline std::ostream&
operator << ( std::ostream& out, const std::vector<double>& u )
{
    out << "[ ";
    for ( const auto& x : u )
        out << x << " ";
    out << " ]";
    return out;
}
// ---------------------------------------------------------------------
std::vector<double> 
Matrix::operator * ( const std::vector<double>& u ) const
{
    const Matrix& A = *this;
    assert( u.size() == unsigned(m_ncols) );
    std::vector<double> v(m_nrows, 0.);
    for ( int i = 0; i < m_nrows; ++i ) {
        for ( int j = 0; j < m_ncols; ++j ) {
            v[i] += A(i,j)*u[j];
        }            
    }
    return v;
}

// =====================================================================
Matrix::Matrix (int dim) : m_nrows(dim), m_ncols(dim),
                           m_arr_coefs(dim*dim)
{
    for ( int i = 0; i < dim; ++ i ) {
        for ( int j = 0; j < dim; ++j ) {
            (*this)(i,j) = (i+j)%dim;
        }
    }
}
// ---------------------------------------------------------------------
Matrix::Matrix( int nrows, int ncols ) : m_nrows(nrows), m_ncols(ncols),
                                         m_arr_coefs(nrows*ncols)
{
    int dim = (nrows > ncols ? nrows : ncols );
    for ( int i = 0; i < nrows; ++ i ) {
        for ( int j = 0; j < ncols; ++j ) {
            (*this)(i,j) = (i+j)%dim;
        }
    }    
}
// =====================================================================
std::vector<double> Matrix::dot(const std::vector<double>& u, int rank, int nbp) {
    const Matrix& A = *this;
    assert( u.size() == unsigned(m_ncols) );
    std::vector<double> v(m_nrows, 0.);
    for ( int i = 0; i < m_nrows; ++i ) {
        for ( int j = 0; j < m_ncols; ++j ) {
            if (j % nbp == rank) {
                v[i] += A(i,j)*u[j];
            }
        }
    }
    return v;
} 
// =====================================================================
int main( int argc, char* argv[] )
{
	MPI_Comm globComm;
	int nbp, rank;
	MPI_Init( &argc, &argv );
	MPI_Comm_dup(MPI_COMM_WORLD, &globComm);
	MPI_Comm_size(globComm, &nbp);
	MPI_Comm_rank(globComm, &rank);

    const int N = 120;

    Matrix A(N);
    
    if (rank==0) {
        std::cout  << "A : " << A << std::endl;
    }
    
    std::vector<double> u( N );
    
    for ( int i = 0; i < N; ++i ) u[i] = i+1;
    
    if (rank==0) {
    std::cout << " u : " << u << std::endl; 
    }
    
    //std::vector<double> v = A*u;
    std::vector<double> v = A.dot(u, rank, nbp);

    std::vector<double> result(N);

    MPI_Allreduce(v.data(), result.data(), N, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    if (rank==0) {
        std::cout << "A.u = " << result << std::endl;
    }

	MPI_Finalize();
    return EXIT_SUCCESS;
}
//[ 575960 568820 561800 554900 548120 541460 534920 528500 522200 516020 509960 504020 498200 492500 486920 481460 476120 470900 465800
//460820 455960 451220 446600 442100 437720 433460 429320 425300 421400 417620 413960 410420 407000 403700 400520 397460 394520 391700
//389000 386420 383960 381620 379400 377300 375320 373460 371720 370100 368600 367220 365960 364820 363800 362900 362120 361460 360920
//360500 360200 360020 359960 360020 360200 360500 360920 361460 362120 362900 363800 364820 365960 367220 368600 370100 371720 373460
//375320 377300 379400 381620 383960 386420 389000 391700 394520 397460 400520 403700 407000 410420 413960 417620 421400 425300 429320
//433460 437720 442100 446600 451220 455960 460820 465800 470900 476120 481460 486920 492500 498200 504020 509960 516020 522200 528500
//534920 541460 548120 554900 561800 568820  ]