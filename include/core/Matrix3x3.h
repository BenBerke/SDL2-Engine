#pragma once
#include <iostream>
#include <cmath>

struct Matrix3x3{
    float m[3][3];

    Matrix3x3() = default;
    Matrix3x3(std::initializer_list<std::initializer_list<float>> rows) {
        int r = 0;
        for(auto& row : rows){
            int c = 0;
            for(auto val : row){
                m[r][c++] = val;
            }
        }

        ++r;
    }

    template<typename T>
    void SetValue(T x, T y, T v) {
        m[x][y] = v;
    }

    static Matrix3x3 Identity() {
        return {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
    }

    Matrix3x3 operator+(const Matrix3x3& other){
        Matrix3x3 newMatrix;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                newMatrix.m[i][j] = m[i][j] + other.m[i][j];
            }
        }

        return newMatrix;
    }
    Matrix3x3 operator-(const Matrix3x3& other){
        Matrix3x3 newMatrix;
        for(int i = 0; i < 3; i++){
            for(int j = 0; j < 3; j++){
                newMatrix.m[i][j] = m[i][j] - other.m[i][j];
            }
        }

        return newMatrix;
    }
    Matrix3x3 operator*(const Matrix3x3& other){
        Matrix3x3 newMatrix;
        for(int i = 0; i < 3; ++i){
            for(int j = 0; j < 3; ++j){
                newMatrix.m[i][j] = 0.0f;
                for (int k = 0; k < 3; ++k) {
                    newMatrix.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }

        return newMatrix;
    }

    Matrix3x3 operator+=(const Matrix3x3& other){
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                m[i][j] += other.m[i][j];
            }
        }

        return *this;
    }

    Matrix3x3 operator-=(const Matrix3x3& other){
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                m[i][j] -= other.m[i][j];
            }
        }
        
        return *this;
    }

    Matrix3x3& operator*=(const Matrix3x3& other) {
        Matrix3x3 result{}; 
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result.m[i][j] = 0.0f;
                for (int k = 0; k < 3; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
             m[i][j] = result.m[i][j];
            }
        }

        return *this;
    }

};