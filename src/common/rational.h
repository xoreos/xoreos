/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Rational number implementation.
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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
 */

#ifndef COMMON_RATIONAL_H
#define COMMON_RATIONAL_H

namespace Common {

/** A simple rational class that holds fractions. */
class Rational {
public:
	Rational();
	Rational(int num);
	Rational(int num, int denom);

	Rational &operator=(const Rational &right);
	Rational &operator=(int right);

	Rational &operator+=(const Rational &right);
	Rational &operator-=(const Rational &right);
	Rational &operator*=(const Rational &right);
	Rational &operator/=(const Rational &right);

	Rational &operator+=(int right);
	Rational &operator-=(int right);
	Rational &operator*=(int right);
	Rational &operator/=(int right);

	const Rational operator-() const;

	const Rational operator+(const Rational &right) const;
	const Rational operator-(const Rational &right) const;
	const Rational operator*(const Rational &right) const;
	const Rational operator/(const Rational &right) const;

	const Rational operator+(int right) const;
	const Rational operator-(int right) const;
	const Rational operator*(int right) const;
	const Rational operator/(int right) const;

	bool operator==(const Rational &right) const;
	bool operator!=(const Rational &right) const;
	bool operator>(const Rational &right) const;
	bool operator<(const Rational &right) const;
	bool operator>=(const Rational &right) const;
	bool operator<=(const Rational &right) const;

	bool operator==(int right) const;
	bool operator!=(int right) const;
	bool operator>(int right) const;
	bool operator<(int right) const;
	bool operator>=(int right) const;
	bool operator<=(int right) const;

	void invert();
	Rational getInverse() const;

	int toInt() const;
	double toDouble() const;

	int getNumerator() const { return _num; }
	int getDenominator() const { return _denom; }

private:
	int _num;
	int _denom;

	void cancel();
};

const Rational operator+(int left, const Rational &right);
const Rational operator-(int left, const Rational &right);
const Rational operator*(int left, const Rational &right);
const Rational operator/(int left, const Rational &right);

bool operator==(int left, const Rational &right);
bool operator!=(int left, const Rational &right);
bool operator>(int left, const Rational &right);
bool operator<(int left, const Rational &right);
bool operator>=(int left, const Rational &right);
bool operator<=(int left, const Rational &right);

} // End of namespace Common

#endif
