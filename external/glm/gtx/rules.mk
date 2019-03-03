# xoreos - A reimplementation of BioWare's Aurora engine
#
# xoreos is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# xoreos is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# xoreos is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with xoreos. If not, see <http://www.gnu.org/licenses/>.

# GLM (OpenGL Mathematics) 0.9.8.5 (<https://glm.g-truc.net/>)

noinst_HEADERS += \
    external/glm/gtx/associated_min_max.hpp \
    external/glm/gtx/bit.hpp \
    external/glm/gtx/closest_point.hpp \
    external/glm/gtx/color_space.hpp \
    external/glm/gtx/color_space_YCoCg.hpp \
    external/glm/gtx/common.hpp \
    external/glm/gtx/compatibility.hpp \
    external/glm/gtx/component_wise.hpp \
    external/glm/gtx/dual_quaternion.hpp \
    external/glm/gtx/euler_angles.hpp \
    external/glm/gtx/extend.hpp \
    external/glm/gtx/extended_min_max.hpp \
    external/glm/gtx/fast_exponential.hpp \
    external/glm/gtx/fast_square_root.hpp \
    external/glm/gtx/fast_trigonometry.hpp \
    external/glm/gtx/gradient_paint.hpp \
    external/glm/gtx/handed_coordinate_space.hpp \
    external/glm/gtx/hash.hpp \
    external/glm/gtx/integer.hpp \
    external/glm/gtx/intersect.hpp \
    external/glm/gtx/io.hpp \
    external/glm/gtx/log_base.hpp \
    external/glm/gtx/matrix_cross_product.hpp \
    external/glm/gtx/matrix_decompose.hpp \
    external/glm/gtx/matrix_interpolation.hpp \
    external/glm/gtx/matrix_major_storage.hpp \
    external/glm/gtx/matrix_operation.hpp \
    external/glm/gtx/matrix_query.hpp \
    external/glm/gtx/matrix_transform_2d.hpp \
    external/glm/gtx/mixed_product.hpp \
    external/glm/gtx/norm.hpp \
    external/glm/gtx/normal.hpp \
    external/glm/gtx/normalize_dot.hpp \
    external/glm/gtx/number_precision.hpp \
    external/glm/gtx/optimum_pow.hpp \
    external/glm/gtx/orthonormalize.hpp \
    external/glm/gtx/perpendicular.hpp \
    external/glm/gtx/polar_coordinates.hpp \
    external/glm/gtx/projection.hpp \
    external/glm/gtx/quaternion.hpp \
    external/glm/gtx/range.hpp \
    external/glm/gtx/raw_data.hpp \
    external/glm/gtx/rotate_normalized_axis.hpp \
    external/glm/gtx/rotate_vector.hpp \
    external/glm/gtx/scalar_multiplication.hpp \
    external/glm/gtx/scalar_relational.hpp \
    external/glm/gtx/spline.hpp \
    external/glm/gtx/std_based_type.hpp \
    external/glm/gtx/string_cast.hpp \
    external/glm/gtx/transform.hpp \
    external/glm/gtx/transform2.hpp \
    external/glm/gtx/type_aligned.hpp \
    external/glm/gtx/type_trait.hpp \
    external/glm/gtx/vector_angle.hpp \
    external/glm/gtx/vector_query.hpp \
    external/glm/gtx/wrap.hpp \
    external/glm/gtx/associated_min_max.inl \
    external/glm/gtx/bit.inl \
    external/glm/gtx/closest_point.inl \
    external/glm/gtx/color_space.inl \
    external/glm/gtx/color_space_YCoCg.inl \
    external/glm/gtx/common.inl \
    external/glm/gtx/compatibility.inl \
    external/glm/gtx/component_wise.inl \
    external/glm/gtx/dual_quaternion.inl \
    external/glm/gtx/euler_angles.inl \
    external/glm/gtx/extend.inl \
    external/glm/gtx/extended_min_max.inl \
    external/glm/gtx/fast_exponential.inl \
    external/glm/gtx/fast_square_root.inl \
    external/glm/gtx/fast_trigonometry.inl \
    external/glm/gtx/float_notmalize.inl \
    external/glm/gtx/gradient_paint.inl \
    external/glm/gtx/handed_coordinate_space.inl \
    external/glm/gtx/hash.inl \
    external/glm/gtx/integer.inl \
    external/glm/gtx/intersect.inl \
    external/glm/gtx/io.inl \
    external/glm/gtx/log_base.inl \
    external/glm/gtx/matrix_cross_product.inl \
    external/glm/gtx/matrix_decompose.inl \
    external/glm/gtx/matrix_interpolation.inl \
    external/glm/gtx/matrix_major_storage.inl \
    external/glm/gtx/matrix_operation.inl \
    external/glm/gtx/matrix_query.inl \
    external/glm/gtx/matrix_transform_2d.inl \
    external/glm/gtx/mixed_product.inl \
    external/glm/gtx/norm.inl \
    external/glm/gtx/normal.inl \
    external/glm/gtx/normalize_dot.inl \
    external/glm/gtx/number_precision.inl \
    external/glm/gtx/optimum_pow.inl \
    external/glm/gtx/orthonormalize.inl \
    external/glm/gtx/perpendicular.inl \
    external/glm/gtx/polar_coordinates.inl \
    external/glm/gtx/projection.inl \
    external/glm/gtx/quaternion.inl \
    external/glm/gtx/raw_data.inl \
    external/glm/gtx/rotate_normalized_axis.inl \
    external/glm/gtx/rotate_vector.inl \
    external/glm/gtx/scalar_relational.inl \
    external/glm/gtx/spline.inl \
    external/glm/gtx/std_based_type.inl \
    external/glm/gtx/string_cast.inl \
    external/glm/gtx/transform.inl \
    external/glm/gtx/transform2.inl \
    external/glm/gtx/type_aligned.inl \
    external/glm/gtx/type_trait.inl \
    external/glm/gtx/vector_angle.inl \
    external/glm/gtx/vector_query.inl \
    external/glm/gtx/wrap.inl \
    $(EMPTY)
