/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/matrix.h
 *  A matrix.
 */

#ifndef COMMON_MATRIX_H
#define COMMON_MATRIX_H

namespace Common {

class Matrix {
public:
	Matrix(int rows, int columns);
	Matrix(const Matrix &right);
	~Matrix();

	Matrix &operator=(const Matrix &right);

	int getRows   () const;
	int getColumns() const;

	      float *operator[](int row);
	const float *operator[](int row) const;


	Matrix operator+(const Matrix &right) const;
	Matrix operator-(const Matrix &right) const;
	Matrix operator*(const Matrix &right) const;

	Matrix operator*(const float &right) const;


	Matrix &operator+=(const Matrix &right);
	Matrix &operator-=(const Matrix &right);
	Matrix &operator*=(const Matrix &right);

	Matrix &operator*=(const float &right);


	Matrix getTranspose() const;

	void transpose();


protected:
	int _rows;
	int _columns;

	float *_elements;
};

} // End of namespace Common

#endif // COMMON_MATRIX_H
