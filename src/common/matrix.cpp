/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "common/matrix.h"

#include <cassert>
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

	delete[] _elements;

	_rows    = right._rows;
	_columns = right._columns;

	_elements = new float[_rows * _columns];

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
	assert(_columns == right._rows);

	Matrix tmp(_rows, right._columns);

	float *d = tmp._elements;
	for (int i = 0; i < _rows; i++) {
		for (int j = 0; j < right._columns; j++, d++) {
			*d = 0;
			for (int n = 0; n < _columns; n++)
				*d += (*this)[i][n] * (*this)[n][j];
		}
	}

	return tmp;
}

Matrix &Matrix::operator+=(const Matrix &right) {
	assert(_rows    == right._rows);
	assert(_columns == right._columns);

	for (int i = 0; i < (_rows * _columns); i++)
		_elements[i] += right._elements[i];

	return *this;
}

Matrix &Matrix::operator-=(const Matrix &right) {
	assert(_rows    == right._rows);
	assert(_columns == right._columns);

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

} // End of namespace Common
