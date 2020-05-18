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

# GLM (OpenGL Mathematics) 0.9.9.8 (<https://glm.g-truc.net/>)

noinst_HEADERS += \
    external/glm/ext/matrix_clip_space.hpp \
    external/glm/ext/matrix_clip_space.inl \
    external/glm/ext/matrix_common.hpp \
    external/glm/ext/matrix_common.inl \
    external/glm/ext/matrix_double2x2.hpp \
    external/glm/ext/matrix_double2x2_precision.hpp \
    external/glm/ext/matrix_double2x3.hpp \
    external/glm/ext/matrix_double2x3_precision.hpp \
    external/glm/ext/matrix_double2x4.hpp \
    external/glm/ext/matrix_double2x4_precision.hpp \
    external/glm/ext/matrix_double3x2.hpp \
    external/glm/ext/matrix_double3x2_precision.hpp \
    external/glm/ext/matrix_double3x3.hpp \
    external/glm/ext/matrix_double3x3_precision.hpp \
    external/glm/ext/matrix_double3x4.hpp \
    external/glm/ext/matrix_double3x4_precision.hpp \
    external/glm/ext/matrix_double4x2.hpp \
    external/glm/ext/matrix_double4x2_precision.hpp \
    external/glm/ext/matrix_double4x3.hpp \
    external/glm/ext/matrix_double4x3_precision.hpp \
    external/glm/ext/matrix_double4x4.hpp \
    external/glm/ext/matrix_double4x4_precision.hpp \
    external/glm/ext/matrix_float2x2.hpp \
    external/glm/ext/matrix_float2x2_precision.hpp \
    external/glm/ext/matrix_float2x3.hpp \
    external/glm/ext/matrix_float2x3_precision.hpp \
    external/glm/ext/matrix_float2x4.hpp \
    external/glm/ext/matrix_float2x4_precision.hpp \
    external/glm/ext/matrix_float3x2.hpp \
    external/glm/ext/matrix_float3x2_precision.hpp \
    external/glm/ext/matrix_float3x3.hpp \
    external/glm/ext/matrix_float3x3_precision.hpp \
    external/glm/ext/matrix_float3x4.hpp \
    external/glm/ext/matrix_float3x4_precision.hpp \
    external/glm/ext/matrix_float4x2.hpp \
    external/glm/ext/matrix_float4x2_precision.hpp \
    external/glm/ext/matrix_float4x3.hpp \
    external/glm/ext/matrix_float4x3_precision.hpp \
    external/glm/ext/matrix_float4x4.hpp \
    external/glm/ext/matrix_float4x4_precision.hpp \
    external/glm/ext/matrix_int2x2.hpp \
    external/glm/ext/matrix_int2x2_sized.hpp \
    external/glm/ext/matrix_int2x3.hpp \
    external/glm/ext/matrix_int2x3_sized.hpp \
    external/glm/ext/matrix_int2x4.hpp \
    external/glm/ext/matrix_int2x4_sized.hpp \
    external/glm/ext/matrix_int3x2.hpp \
    external/glm/ext/matrix_int3x2_sized.hpp \
    external/glm/ext/matrix_int3x3.hpp \
    external/glm/ext/matrix_int3x3_sized.hpp \
    external/glm/ext/matrix_int3x4.hpp \
    external/glm/ext/matrix_int3x4_sized.hpp \
    external/glm/ext/matrix_int4x2.hpp \
    external/glm/ext/matrix_int4x2_sized.hpp \
    external/glm/ext/matrix_int4x3.hpp \
    external/glm/ext/matrix_int4x3_sized.hpp \
    external/glm/ext/matrix_int4x4.hpp \
    external/glm/ext/matrix_int4x4_sized.hpp \
    external/glm/ext/matrix_projection.hpp \
    external/glm/ext/matrix_projection.inl \
    external/glm/ext/matrix_relational.hpp \
    external/glm/ext/matrix_relational.inl \
    external/glm/ext/matrix_transform.hpp \
    external/glm/ext/matrix_transform.inl \
    external/glm/ext/matrix_uint2x2.hpp \
    external/glm/ext/matrix_uint2x2_sized.hpp \
    external/glm/ext/matrix_uint2x3.hpp \
    external/glm/ext/matrix_uint2x3_sized.hpp \
    external/glm/ext/matrix_uint2x4.hpp \
    external/glm/ext/matrix_uint2x4_sized.hpp \
    external/glm/ext/matrix_uint3x2.hpp \
    external/glm/ext/matrix_uint3x2_sized.hpp \
    external/glm/ext/matrix_uint3x3.hpp \
    external/glm/ext/matrix_uint3x3_sized.hpp \
    external/glm/ext/matrix_uint3x4.hpp \
    external/glm/ext/matrix_uint3x4_sized.hpp \
    external/glm/ext/matrix_uint4x2.hpp \
    external/glm/ext/matrix_uint4x2_sized.hpp \
    external/glm/ext/matrix_uint4x3.hpp \
    external/glm/ext/matrix_uint4x3_sized.hpp \
    external/glm/ext/matrix_uint4x4.hpp \
    external/glm/ext/matrix_uint4x4_sized.hpp \
    external/glm/ext/quaternion_common.hpp \
    external/glm/ext/quaternion_common.inl \
    external/glm/ext/quaternion_common_simd.inl \
    external/glm/ext/quaternion_double.hpp \
    external/glm/ext/quaternion_double_precision.hpp \
    external/glm/ext/quaternion_exponential.hpp \
    external/glm/ext/quaternion_exponential.inl \
    external/glm/ext/quaternion_float.hpp \
    external/glm/ext/quaternion_float_precision.hpp \
    external/glm/ext/quaternion_geometric.hpp \
    external/glm/ext/quaternion_geometric.inl \
    external/glm/ext/quaternion_relational.hpp \
    external/glm/ext/quaternion_relational.inl \
    external/glm/ext/quaternion_transform.hpp \
    external/glm/ext/quaternion_transform.inl \
    external/glm/ext/quaternion_trigonometric.hpp \
    external/glm/ext/quaternion_trigonometric.inl \
    external/glm/ext/scalar_common.hpp \
    external/glm/ext/scalar_common.inl \
    external/glm/ext/scalar_constants.hpp \
    external/glm/ext/scalar_constants.inl \
    external/glm/ext/scalar_int_sized.hpp \
    external/glm/ext/scalar_integer.hpp \
    external/glm/ext/scalar_integer.inl \
    external/glm/ext/scalar_packing.hpp \
    external/glm/ext/scalar_packing.inl \
    external/glm/ext/scalar_relational.hpp \
    external/glm/ext/scalar_relational.inl \
    external/glm/ext/scalar_uint_sized.hpp \
    external/glm/ext/scalar_ulp.hpp \
    external/glm/ext/scalar_ulp.inl \
    external/glm/ext/vector_bool1.hpp \
    external/glm/ext/vector_bool1_precision.hpp \
    external/glm/ext/vector_bool2.hpp \
    external/glm/ext/vector_bool2_precision.hpp \
    external/glm/ext/vector_bool3.hpp \
    external/glm/ext/vector_bool3_precision.hpp \
    external/glm/ext/vector_bool4.hpp \
    external/glm/ext/vector_bool4_precision.hpp \
    external/glm/ext/vector_common.hpp \
    external/glm/ext/vector_common.inl \
    external/glm/ext/vector_double1.hpp \
    external/glm/ext/vector_double1_precision.hpp \
    external/glm/ext/vector_double2.hpp \
    external/glm/ext/vector_double2_precision.hpp \
    external/glm/ext/vector_double3.hpp \
    external/glm/ext/vector_double3_precision.hpp \
    external/glm/ext/vector_double4.hpp \
    external/glm/ext/vector_double4_precision.hpp \
    external/glm/ext/vector_float1.hpp \
    external/glm/ext/vector_float1_precision.hpp \
    external/glm/ext/vector_float2.hpp \
    external/glm/ext/vector_float2_precision.hpp \
    external/glm/ext/vector_float3.hpp \
    external/glm/ext/vector_float3_precision.hpp \
    external/glm/ext/vector_float4.hpp \
    external/glm/ext/vector_float4_precision.hpp \
    external/glm/ext/vector_int1.hpp \
    external/glm/ext/vector_int1_sized.hpp \
    external/glm/ext/vector_int2.hpp \
    external/glm/ext/vector_int2_sized.hpp \
    external/glm/ext/vector_int3.hpp \
    external/glm/ext/vector_int3_sized.hpp \
    external/glm/ext/vector_int4.hpp \
    external/glm/ext/vector_int4_sized.hpp \
    external/glm/ext/vector_integer.hpp \
    external/glm/ext/vector_integer.inl \
    external/glm/ext/vector_packing.hpp \
    external/glm/ext/vector_packing.inl \
    external/glm/ext/vector_relational.hpp \
    external/glm/ext/vector_relational.inl \
    external/glm/ext/vector_uint1.hpp \
    external/glm/ext/vector_uint1_sized.hpp \
    external/glm/ext/vector_uint2.hpp \
    external/glm/ext/vector_uint2_sized.hpp \
    external/glm/ext/vector_uint3.hpp \
    external/glm/ext/vector_uint3_sized.hpp \
    external/glm/ext/vector_uint4.hpp \
    external/glm/ext/vector_uint4_sized.hpp \
    external/glm/ext/vector_ulp.hpp \
    external/glm/ext/vector_ulp.inl \
    $(EMPTY)
