#ifndef MATRIX_H
#define MATRIX_H
#include <cstdint>
#include <memory>
#include <cassert>
#include <iostream>
#include <exception>

template <size_t Rows, size_t Columns, typename T>
class Matrix;

template <size_t Rows, size_t Columns, typename T>
T determinantHelper(const Matrix<Rows, Columns, T>& m);

template <size_t Rows, size_t Columns, typename T = int>
class Matrix
{
public:
    Matrix() : arr(new T*[Rows])
    {
        static_assert(Rows && Columns);
        for (size_t row = 0; row < Rows; ++row)
        {
            arr[row] = new T[Columns];
        }
    }
    Matrix(const T& element) : Matrix()
    {
        for (size_t row = 0; row < Rows; ++row)
        {
            for (size_t column = 0; column < Columns; ++column)
            {
                arr[row][column] = element;
            }
        }
    }
    Matrix(const Matrix& other) : Matrix()
    {
        for (size_t row = 0; row < Rows; ++row)
        {
            for (size_t column = 0; column < Columns; ++column)
            {
                arr[row][column] = other.arr[row][column];
            }
        }
    }
    Matrix& operator=(const Matrix& other) &
    {
        for (size_t row = 0; row < Rows; ++row)
        {
            for (size_t column = 0; column < Columns; ++column)
            {
                arr[row][column] = other.arr[row][column];
            }
        }
        return *this;
    }
    ~Matrix()
    {
       for (size_t row = 0; row < Rows; ++row)
       {
            delete[] arr[row];
       }
       delete[] arr;
    }
    static Matrix getIdentityMatrix()
    {
        static_assert(Rows == Columns);
        Matrix res;
        for (size_t i = 0; i < Rows; ++i)
        {
            res.arr[i][i] = T(1);
        }
        return res;
    }
    void print() const
    {
        for (size_t row = 0; row < Rows; ++row)
        {
            for (size_t column = 0; column < Columns; ++column)
            {
                std::cout << arr[row][column] << ' ';
            }
            std::cout << std::endl;
        }
    }
private:
    struct RowReference 
    {
        T* rowPtr;

        T& operator[](size_t columnIndex)
        {
            return *(rowPtr + columnIndex);
        }
        const T& operator[](size_t columnIndex) const
        {
            return *(rowPtr + columnIndex);
        }
    };
public:
    RowReference operator[](size_t rowIndex)
    {
        return RowReference{arr[rowIndex]};
    }
    const RowReference operator[](size_t rowIndex) const
    {
        return RowReference{arr[rowIndex]};
    }
    size_t rows() const { return Rows; }
    size_t columns() const { return Columns; }
    Matrix& operator+=(const Matrix& other)
    {
        for (size_t row = 0; row < Rows; ++row)
        {
            for (size_t column = 0; column < Columns; ++column)
            {
                arr[row][column] += other.arr[row][column];
            }
        }
        return *this;
    }
    Matrix operator+(const Matrix& other) const
    {
        Matrix res = *this;
        res += other;
        return res;
    }
    Matrix& operator-=(const Matrix& other)
    {
        for (size_t row = 0; row < Rows; ++row)
        {
            for (size_t column = 0; column < Columns; ++column)
            {
                arr[row][column] -= other.arr[row][column];
            }
        }
        return *this;
    }
    Matrix operator-(const Matrix& other) const
    {
        Matrix res = *this;
        res -= other;
        return res;
    }
    Matrix& operator*=(const T& num)
    {
        for (int row = 0; row < Rows; ++row)
        {
            for (int column = 0; column < Columns; ++column)
            {
                arr[row][column] *= num;
            }
        }
        return *this;
    }
    Matrix& operator/=(const T& num)
    {
        for (int row = 0; row < Rows; ++row)
        {
            for (int column = 0; column < Columns; ++column)
            {
                arr[row][column] /= num;
            }
        }
        return *this;
    }
    Matrix operator*(const T& num) const
    {
        Matrix other = *this;
        other *= num;
        return other;
    }
    Matrix operator/(const T& num) const
    {
        Matrix other = *this;
        other /= num;
        return other;
    }
    Matrix<Columns, Rows, T> transpose() const
    {
        Matrix<Columns, Rows, T> res;
        for (size_t row = 0; row < Rows; ++row)
        {
            for (size_t column = 0; column < Columns; ++column)
            {
                res[column][row] = arr[row][column];
            }
        }
        return res;
    }
    T determinant() const
    {
        static_assert(Rows == Columns);
        if (Rows == 1) return arr[0][0];
        if (Rows == 2) return arr[0][0] * arr[1][1] - arr[1][0] * arr[0][1]; 
        return determinantHelper(*this);
    }
private:
    T** arr;
};

template <size_t Rows, size_t Columns, typename T = int>
T determinantHelper(const Matrix<Rows, Columns, T>& m)
{
    T res = 0;
    for (int i = 0; i < Rows; ++i) // i is column number
    {
        Matrix<Rows - 1, Columns - 1, T> mTmp;
        int tmpColumn = 0;
        for (int bigMatrixColumn = 0; bigMatrixColumn < Columns; 
            ++bigMatrixColumn)
        {
            if (bigMatrixColumn == i) continue;
            for (int row = 1; row < Columns; ++row)
            {
                mTmp[row - 1][tmpColumn] = m[row][bigMatrixColumn];
            }
            ++tmpColumn;
        }
        T tmpDeterminant = determinantHelper(mTmp) * m[0][i]; 
        i % 2 ? res -= tmpDeterminant : res += tmpDeterminant; 
    }
    return res;
}

template <typename T = int>
T determinantHelper(const Matrix<3, 3, int>& m)
{
    return m[0][0] * m[1][1] * m[2][2] + 
        m[1][0] * m[2][1] * m[0][2] + 
        m[2][0] * m[0][1] * m[1][2] -
        m[2][0] * m[1][1] * m[0][2] -
        m[1][0] * m[0][1] * m[2][2] -
        m[0][0] * m[2][1] * m[1][2]; 
}

template <size_t Rows1, size_t Columns1, typename T1, size_t Rows2, 
    size_t Columns2, typename T2>
bool operator==(const Matrix<Rows1, Columns1, T1>& m1, 
    const Matrix<Rows2, Columns2, T2>& m2)
{
    if constexpr (Rows1 != Rows2 || Columns1 != Columns2 || 
        !std::is_same_v<T1, T2>)
    {
        return false;
    }
    for (size_t row = 0; row < Rows1; ++row)
    {
        for (size_t column = 0; column < Columns1; ++column)
        {
            if (m1[row][column] != m2[row][column]) return false;
        }
    }
    return true;
}

template <size_t Rows1, size_t Columns1, typename T1, size_t Rows2, 
    size_t Columns2, typename T2>
bool operator!=(const Matrix<Rows1, Columns1, T1>& m1, 
    const Matrix<Rows2, Columns2, T2>& m2)
{
    return !(m1 == m2);
}

template <size_t M, size_t N, size_t K, typename T>
Matrix<M, K, T> operator*(const Matrix<M, N, T>& m1, const Matrix<N, K, T>& m2)
{
    Matrix<M, K, T> res;
    for (size_t row = 0; row < M; ++row)
    {
        for (size_t column = 0; column < K; ++column)
        {
            T value = T();
            for (size_t i = 0; i < N; ++i)
            {
                value += m1[row][i] * m2[i][column];
            }
            res[row][column] = value;
        }
    }
    return res;
}

#endif
