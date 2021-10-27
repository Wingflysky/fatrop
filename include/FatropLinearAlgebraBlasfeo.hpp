
#ifndef FATROP_BLASFEO_INCLUDED
#define FATROP_BLASFEO_INCLUDED

// macros
#define MAT blasfeo_dmat
#define MEMSIZE_MAT blasfeo_memsize_dmat
#define MEMSIZE_MAT blasfeo_memsize_dmat
#define CREATE_MAT blasfeo_create_dmat
#define MATEL BLASFEO_DMATEL

#include <iostream>
extern "C"
{
#include <blasfeo.h>
}
#include "FatropMemory.hpp"
using namespace std;
namespace fatrop
{
    class fatrop_memory_matrix : public fatrop_memory_el_base
    {
        public:
        fatrop_memory_matrix(int n_rows, int n_cols, int N, fatrop_memory_allocator &fma) : N_(N), n_rows_(n_rows), n_cols_(n_cols)
        {
            fma.add(*this);
        }
        /** \brief calculate memory size*/
        int memory_size() const
        {
            int result = 0;
            // size to store structs
            result += N_ * sizeof(MAT);
            // sufficient space for cache alignment
            result = (result + LEVEL1_DCACHE_LINE_SIZE - 1) / LEVEL1_DCACHE_LINE_SIZE * LEVEL1_DCACHE_LINE_SIZE + LEVEL1_DCACHE_LINE_SIZE;
            // size to store date
            result += N_ * MEMSIZE_MAT(n_rows_, n_cols_);
            return result;
        };
        /** \brief set up memory element and advance pointer */
        void set_up(char *&data_p)
        {
            MAT *bf_ptr = (MAT *)data_p;
            this->mat = bf_ptr;
            bf_ptr += N_;
            // align with cache line
            long long l_ptr = (long long)bf_ptr;
            l_ptr = (l_ptr + LEVEL1_DCACHE_LINE_SIZE - 1) / LEVEL1_DCACHE_LINE_SIZE * LEVEL1_DCACHE_LINE_SIZE;
            data_p = (char *)l_ptr;
            double *d_ptr_begin = (double *)data_p;
            for (int i = 0; i < N_; i++)
            {
                CREATE_MAT(n_rows_, n_cols_, mat + i, data_p);
                data_p += (mat + i)->memsize;
            }
            double * d_ptr_end = (double *)data_p;
            for (double *d_ptr = d_ptr_begin; d_ptr < d_ptr_end; d_ptr++)
            {
                *d_ptr = 0.0;
            }
        };
        inline explicit operator MAT*() const{
            return this->mat;
        }

    private:
        MAT *mat;
        const int N_;
        const int n_rows_;
        const int n_cols_;
    };
} // namespace fatrop
#endif //FATROP_BLASFEO_INCLUDED