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

/** @file common/matrix.h
 *  A matrix.
 */

#ifndef COMMON_MATRIX_H
#define COMMON_MATRIX_H

namespace Common {

/** A matrix, storing its elements in column-major order. */
class Matrix {
public:
	Matrix(int rows, int columns);
	Matrix(const Matrix &right);
	~Matrix();

	Matrix &operator=(const Matrix &right);


	int getRows   () const;
	int getColumns() const;

	/** Get the matrix elements in a flat array, column-major order. */
	const float *get() const;
	/** Set the matrix elements from flat array, column-major order. */
	void set(const float *m) const;

	float &operator()(int row, int column);
	float  operator()(int row, int column) const;


	Matrix operator+(const Matrix &right) const;
	Matrix operator-(const Matrix &right) const;
	Matrix operator*(const Matrix &right) const;

	Matrix operator*(const float &right) const;


	Matrix &operator+=(const Matrix &right);
	Matrix &operator-=(const Matrix &right);
	Matrix &operator*=(const Matrix &right);

	Matrix &operator*=(const float &right);


	float getDeterminant() const;

	Matrix getReduced(int row, int col) const;

	Matrix getTranspose() const;
	Matrix getInverse() const;

	void transpose();
	void invert();

	bool isInvertible() const;

protected:
	int _rows;
	int _columns;

	float *_elements;

private:
	static void multiply(float *out, const Matrix &a, const Matrix &b);
	static void multiply_4x4_4x4(float *out, const float *a, const float *b);
};

} // End of namespace Common

#endif // COMMON_MATRIX_H
