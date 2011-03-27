/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/matrix.cpp
 *  A matrix.
 */

#include "common/error.h"
#include "common/matrix.h"

#include <cstring>

namespace Common {

Matrix::Matrix(int rows, int columns) : _rows(rows), _columns(columns) {
	_elements = new float[_rows * _columns];
}

Matrix::Matrix(const Matrix &right) {
	_rows    = right._rows;
	_columns = right._columns;

	_elements = new float[_rows * _columns];

	memcpy(_elements, right._elements, _rows * _columns * sizeof(float));
}

Matrix::~Matrix() {
	delete[] _elements;
}

Matrix &Matrix::operator=(const Matrix &right) {
	if (this == &right)
		return *this;

	if ((_rows != right._rows) || (_columns != right._columns)) {
		delete[] _elements;

		_rows    = right._rows;
		_columns = right._columns;

		_elements = new float[_rows * _columns];
	}

	memcpy(_elements, right._elements, _rows * _columns * sizeof(float));

	return *this;
}

int Matrix::getRows() const {
	return _rows;
}

int Matrix::getColumns() const {
	return _columns;
}

float *Matrix::operator[](int row) {
	return _elements + (row * _columns);
}

const float *Matrix::operator[](int row) const {
	return _elements + (row * _columns);
}

Matrix Matrix::operator+(const Matrix &right) const {
	Matrix tmp = *this;

	tmp += right;

	return tmp;
}

Matrix Matrix::operator-(const Matrix &right) const {
	Matrix tmp = *this;

	tmp -= right;

	return tmp;
}

Matrix Matrix::operator*(const float &right) const {
	Matrix tmp = *this;

	tmp *= right;

	return tmp;
}

Matrix Matrix::operator*(const Matrix &right) const {
	if (_columns != right._rows)
		throw Exception("Matrix::operator*(): %d != %d", _columns, right._rows);

	Matrix tmp(_rows, right._columns);

	float *d = tmp._elements;
	for (int i = 0; i < _rows; i++) {
		for (int j = 0; j < right._columns; j++, d++) {
			*d = 0;
			for (int n = 0; n < _columns; n++)
				*d += (*this)[i][n] * (right)[n][j];
		}
	}

	return tmp;
}

Matrix &Matrix::operator+=(const Matrix &right) {
	if ((_rows != right._rows) || (_columns != right._columns))
		throw Exception("Matrix::operator+=(): %dx%d != %dx%d",
		                _rows, _columns,  right._rows, right._columns);

	for (int i = 0; i < (_rows * _columns); i++)
		_elements[i] += right._elements[i];

	return *this;
}

Matrix &Matrix::operator-=(const Matrix &right) {
	if ((_rows != right._rows) || (_columns != right._columns))
		throw Exception("Matrix::operator-=(): %dx%d != %dx%d",
		                _rows, _columns,  right._rows, right._columns);

	for (int i = 0; i < (_rows * _columns); i++)
		_elements[i] -= right._elements[i];

	return *this;
}

Matrix &Matrix::operator*=(const float &right) {
	for (int i = 0; i < (_rows * _columns); i++)
		_elements[i] *= right;

	return *this;
}

Matrix &Matrix::operator*=(const Matrix &right) {
	Matrix tmp = *this * right;

	*this = tmp;

	return *this;
}

Matrix Matrix::getTranspose() const {
	Matrix tmp(_columns, _rows);

	for (int i = 0; i < _columns; i++)
		for (int j = 0; j < _rows; j++)
			tmp[i][j] = (*this)[j][i];

	return tmp;
}

void Matrix::transpose() {
	*this = getTranspose();
}

float Matrix::getDeterminant() const {
	if (_rows != _columns)
		throw Exception("Matrix::getDeterminant(): %d != %d", _rows, _columns);

	if ((_rows == 1) && (_columns == 1))
		return (*this)[0][0];

	if ((_rows == 2) && (_columns == 2))
		return (*this)[0][0] * (*this)[1][1] - (*this)[0][1] * (*this)[1][0];

	if ((_rows == 3) && (_columns == 3))
		return (*this)[0][0] * (*this)[1][1] * (*this)[2][2] +
		       (*this)[0][1] * (*this)[1][2] * (*this)[2][0] +
		       (*this)[0][2] * (*this)[1][0] * (*this)[2][1] -
		       (*this)[0][0] * (*this)[1][2] * (*this)[2][1] -
		       (*this)[0][1] * (*this)[1][0] * (*this)[2][2] -
		       (*this)[0][2] * (*this)[1][1] * (*this)[2][0];

	if ((_rows == 4) && (_columns == 4))
		return (*this)[0][0] * (*this)[1][1] * (*this)[2][2] * (*this)[3][3] +
		       (*this)[0][0] * (*this)[1][2] * (*this)[2][3] * (*this)[3][1] +
		       (*this)[0][0] * (*this)[1][3] * (*this)[2][1] * (*this)[3][2] +
		       (*this)[0][1] * (*this)[1][0] * (*this)[2][3] * (*this)[3][2] +
		       (*this)[0][1] * (*this)[1][2] * (*this)[2][0] * (*this)[3][3] +
		       (*this)[0][1] * (*this)[1][3] * (*this)[2][2] * (*this)[3][0] +
		       (*this)[0][2] * (*this)[1][0] * (*this)[2][1] * (*this)[3][3] +
		       (*this)[0][2] * (*this)[1][1] * (*this)[2][3] * (*this)[3][0] +
		       (*this)[0][2] * (*this)[1][3] * (*this)[2][0] * (*this)[3][1] +
		       (*this)[0][3] * (*this)[1][0] * (*this)[2][2] * (*this)[3][1] +
		       (*this)[0][3] * (*this)[1][1] * (*this)[2][0] * (*this)[3][2] +
		       (*this)[0][3] * (*this)[1][2] * (*this)[2][1] * (*this)[3][0] -
		       (*this)[0][0] * (*this)[1][1] * (*this)[2][3] * (*this)[3][2] -
		       (*this)[0][0] * (*this)[1][2] * (*this)[2][1] * (*this)[3][3] -
		       (*this)[0][0] * (*this)[1][3] * (*this)[2][2] * (*this)[3][1] -
		       (*this)[0][1] * (*this)[1][0] * (*this)[2][2] * (*this)[3][3] -
		       (*this)[0][1] * (*this)[1][2] * (*this)[2][3] * (*this)[3][0] -
		       (*this)[0][1] * (*this)[1][3] * (*this)[2][0] * (*this)[3][2] -
		       (*this)[0][2] * (*this)[1][0] * (*this)[2][3] * (*this)[3][1] -
		       (*this)[0][2] * (*this)[1][1] * (*this)[2][0] * (*this)[3][3] -
		       (*this)[0][2] * (*this)[1][3] * (*this)[2][1] * (*this)[3][0] -
		       (*this)[0][3] * (*this)[1][0] * (*this)[2][1] * (*this)[3][2] -
		       (*this)[0][3] * (*this)[1][1] * (*this)[2][2] * (*this)[3][0] -
		       (*this)[0][3] * (*this)[1][2] * (*this)[2][0] * (*this)[3][1];

	// General case
	float det = 0.0f;
	for (int j = 0; j < _columns; j++) {
		if (j % 2)
			det += (*this)[0][j] * -1 * getReduced(0,j).getDeterminant();
		else
			det += (*this)[0][j] *  1 * getReduced(0,j).getDeterminant();
	}

	return det;
}

Matrix Matrix::getReduced(int row, int col) const {
	Matrix reducedMatrix(_rows - 1, _columns - 1);

	int redMaRow = 0, redMaCol = 0;
	for (int i = 0; i < _rows; i++) {
		if (i == row)
			continue;

		for (int j = 0; j < _columns; j++) {
			if (j == col)
				continue;

			reducedMatrix[redMaRow][redMaCol] = (*this)[i][j];
			redMaCol++;
		}

		redMaRow++;
		redMaCol = 0;
	}

	return reducedMatrix;
}

Matrix Matrix::getInverse() const {
	Matrix inv(_rows, _columns);

	float det = getDeterminant();
	if (det == 0)
		throw Exception("Matrix::getInverse(): Determinant == 0");

	for (int i = 0; i < _rows; i++) {
		for (int j = 0; j < _columns; j++) {
			inv[i][j] = (1.0f / det) * getReduced(j, i).getDeterminant();

			if (((i + j) % 2) == 1)
				inv[i][j] = -inv[i][j];
		}
	}

	return inv;
}

void Matrix::invert() {
	*this = getInverse();
}

bool Matrix::isInvertible() const {
	return getDeterminant() != 0;
}

} // End of namespace Common
