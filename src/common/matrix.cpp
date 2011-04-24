/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/matrix.cpp
 *  A matrix.
 */

#include "common/error.h"
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

const float *Matrix::get() const {
	return _elements;
}

void Matrix::set(const float *m) const {
	memcpy(_elements, m, _rows * _columns * sizeof(float));
}

float &Matrix::operator()(int row, int column) {
	return _elements[(column * _rows) + row];
}

float Matrix::operator()(int row, int column) const {
	return _elements[(column * _rows) + row];
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

	multiply(tmp._elements, *this, right);

	return tmp;
}

Matrix &Matrix::operator+=(const Matrix &right) {
	assert((_rows == right._rows) && (_columns == right._columns));

	for (int i = 0; i < (_rows * _columns); i++)
		_elements[i] += right._elements[i];

	return *this;
}

Matrix &Matrix::operator-=(const Matrix &right) {
	assert((_rows == right._rows) && (_columns == right._columns));

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
	assert(_columns == right._rows);

	float *t = new float[_rows * right._columns];

	multiply(t, *this, right);

	delete[] _elements;

	_columns  = right._columns;
	_elements = t;

	return *this;
}

Matrix Matrix::getTranspose() const {
	Matrix tmp(_columns, _rows);

	for (int i = 0; i < _columns; i++)
		for (int j = 0; j < _rows; j++)
			tmp(i, j) = (*this)(j, i);

	return tmp;
}

void Matrix::transpose() {
	*this = getTranspose();
}

float Matrix::getDeterminant() const {
	assert(_rows == _columns);

	if (_rows == 1)
		return _elements[0];

	if (_rows == 2)
		return _elements[0] * _elements[3] - _elements[1] * _elements[2];

	if (_rows == 3)
		return _elements[0] * _elements[4] * _elements[8] +
		       _elements[3] * _elements[7] * _elements[2] +
		       _elements[6] * _elements[1] * _elements[5] -
		       _elements[0] * _elements[7] * _elements[5] -
		       _elements[3] * _elements[1] * _elements[8] -
		       _elements[6] * _elements[4] * _elements[2];

	if (_rows == 4)
		return _elements[ 0] * _elements[ 5] * _elements[10] * _elements[15] +
		       _elements[ 0] * _elements[ 9] * _elements[14] * _elements[ 7] +
		       _elements[ 0] * _elements[13] * _elements[ 6] * _elements[11] +
		       _elements[ 4] * _elements[ 1] * _elements[14] * _elements[11] +
		       _elements[ 4] * _elements[ 9] * _elements[ 2] * _elements[15] +
		       _elements[ 4] * _elements[13] * _elements[10] * _elements[ 3] +
		       _elements[ 8] * _elements[ 1] * _elements[ 6] * _elements[15] +
		       _elements[ 8] * _elements[ 5] * _elements[14] * _elements[ 3] +
		       _elements[ 8] * _elements[13] * _elements[ 2] * _elements[ 7] +
		       _elements[12] * _elements[ 1] * _elements[10] * _elements[ 7] +
		       _elements[12] * _elements[ 5] * _elements[ 2] * _elements[11] +
		       _elements[12] * _elements[ 9] * _elements[ 6] * _elements[ 3] -
		       _elements[ 0] * _elements[ 5] * _elements[14] * _elements[11] -
		       _elements[ 0] * _elements[ 9] * _elements[ 6] * _elements[15] -
		       _elements[ 0] * _elements[13] * _elements[10] * _elements[ 7] -
		       _elements[ 4] * _elements[ 1] * _elements[10] * _elements[15] -
		       _elements[ 4] * _elements[ 9] * _elements[14] * _elements[ 3] -
		       _elements[ 4] * _elements[13] * _elements[ 2] * _elements[11] -
		       _elements[ 8] * _elements[ 1] * _elements[14] * _elements[ 7] -
		       _elements[ 8] * _elements[ 5] * _elements[ 2] * _elements[15] -
		       _elements[ 8] * _elements[13] * _elements[ 6] * _elements[ 3] -
		       _elements[12] * _elements[ 1] * _elements[ 6] * _elements[11] -
		       _elements[12] * _elements[ 5] * _elements[10] * _elements[ 3] -
		       _elements[12] * _elements[ 9] * _elements[ 2] * _elements[ 7];

	// General case
	float det = 0.0f;
	for (int j = 0; j < _columns; j++) {
		if (j % 2)
			det += (*this)(0, j) * -1 * getReduced(0,j).getDeterminant();
		else
			det += (*this)(0, j) *  1 * getReduced(0,j).getDeterminant();
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

			reducedMatrix(redMaRow, redMaCol) = (*this)(i, j);
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
			inv(i, j) = (1.0f / det) * getReduced(j, i).getDeterminant();

			if (((i + j) % 2) == 1)
				inv(i, j) = -inv(i, j);
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

void Matrix::multiply(float *out, const Matrix &a, const Matrix &b) {
	if ((a._rows == 4) && (a._columns == 4) && (b._rows == 4) && (b._columns == 4)) {
		multiply_4x4_4x4(out, a._elements, b._elements);
		return;
	}

	for (int i = 0; i < b._columns; i++) {
		for (int j = 0; j < a._rows; j++, out++) {
			*out = a(j, 0) * b(0, i);
			for (int n = 1; n < a._columns; n++)
				*out += a(j, n) * b(n, i);
		}
	}
}

void Matrix::multiply_4x4_4x4(float *out, const float *a, const float *b) {
	out[ 0] = (a[ 0] * b[ 0]) + (a[ 4] * b[ 1]) + (a[ 8] * b[ 2]) + (a[12] * b[ 3]);
	out[ 1] = (a[ 1] * b[ 0]) + (a[ 5] * b[ 1]) + (a[ 9] * b[ 2]) + (a[13] * b[ 3]);
	out[ 2] = (a[ 2] * b[ 0]) + (a[ 6] * b[ 1]) + (a[10] * b[ 2]) + (a[14] * b[ 3]);
	out[ 3] = (a[ 3] * b[ 0]) + (a[ 7] * b[ 1]) + (a[11] * b[ 2]) + (a[15] * b[ 3]);
	out[ 4] = (a[ 0] * b[ 4]) + (a[ 4] * b[ 5]) + (a[ 8] * b[ 6]) + (a[12] * b[ 7]);
	out[ 5] = (a[ 1] * b[ 4]) + (a[ 5] * b[ 5]) + (a[ 9] * b[ 6]) + (a[13] * b[ 7]);
	out[ 6] = (a[ 2] * b[ 4]) + (a[ 6] * b[ 5]) + (a[10] * b[ 6]) + (a[14] * b[ 7]);
	out[ 7] = (a[ 3] * b[ 4]) + (a[ 7] * b[ 5]) + (a[11] * b[ 6]) + (a[15] * b[ 7]);
	out[ 8] = (a[ 0] * b[ 8]) + (a[ 4] * b[ 9]) + (a[ 8] * b[10]) + (a[12] * b[11]);
	out[ 9] = (a[ 1] * b[ 8]) + (a[ 5] * b[ 9]) + (a[ 9] * b[10]) + (a[13] * b[11]);
	out[10] = (a[ 2] * b[ 8]) + (a[ 6] * b[ 9]) + (a[10] * b[10]) + (a[14] * b[11]);
	out[11] = (a[ 3] * b[ 8]) + (a[ 7] * b[ 9]) + (a[11] * b[10]) + (a[15] * b[11]);
	out[12] = (a[ 0] * b[12]) + (a[ 4] * b[13]) + (a[ 8] * b[14]) + (a[12] * b[15]);
	out[13] = (a[ 1] * b[12]) + (a[ 5] * b[13]) + (a[ 9] * b[14]) + (a[13] * b[15]);
	out[14] = (a[ 2] * b[12]) + (a[ 6] * b[13]) + (a[10] * b[14]) + (a[14] * b[15]);
	out[15] = (a[ 3] * b[12]) + (a[ 7] * b[13]) + (a[11] * b[14]) + (a[15] * b[15]);
}

} // End of namespace Common
