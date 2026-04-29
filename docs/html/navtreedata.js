/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "Lattice Engine", "index.html", [
    [ "manual", "md_vendor_2glm_2manual.html", [
      [ "GLM 1.0.0 Manual", "md_vendor_2glm_2manual.html#autotoc_md11", [
        [ "Table of Contents", "md_vendor_2glm_2manual.html#autotoc_md13", null ],
        [ "<a class=\"anchor\" id=\"section0\"></a> Licenses", "md_vendor_2glm_2manual.html#autotoc_md15", [
          [ "The Happy Bunny License (Modified MIT License)", "md_vendor_2glm_2manual.html#autotoc_md16", null ],
          [ "The MIT License", "md_vendor_2glm_2manual.html#autotoc_md17", null ]
        ] ],
        [ "<a class=\"anchor\" id=\"section1\"></a> 1. Getting started", "md_vendor_2glm_2manual.html#autotoc_md19", [
          [ "<a class=\"anchor\" id=\"section1_1\"></a> 1.1. Using global headers", "md_vendor_2glm_2manual.html#autotoc_md20", null ],
          [ "<a class=\"anchor\" id=\"section1_2\"></a> 1.2. Using separated headers", "md_vendor_2glm_2manual.html#autotoc_md21", null ],
          [ "<a class=\"anchor\" id=\"section1_3\"></a> 1.3. Using extension headers", "md_vendor_2glm_2manual.html#autotoc_md22", null ],
          [ "<a class=\"anchor\" id=\"section1_4\"></a> 1.4. Dependencies", "md_vendor_2glm_2manual.html#autotoc_md23", null ],
          [ "<a class=\"anchor\" id=\"section1_5\"></a> 1.5. Finding GLM with CMake", "md_vendor_2glm_2manual.html#autotoc_md24", null ]
        ] ],
        [ "<a class=\"anchor\" id=\"section2\"></a> 2. Preprocessor configurations", "md_vendor_2glm_2manual.html#autotoc_md26", [
          [ "<a class=\"anchor\" id=\"section2_1\"></a> 2.1. GLM_FORCE_MESSAGES: Platform auto detection and default configuration", "md_vendor_2glm_2manual.html#autotoc_md27", null ],
          [ "<a class=\"anchor\" id=\"section2_2\"></a> 2.2. GLM_FORCE_PLATFORM_UNKNOWN: Force GLM not to detect the build platform", "md_vendor_2glm_2manual.html#autotoc_md28", null ],
          [ "<a class=\"anchor\" id=\"section2_3\"></a> 2.3. GLM_FORCE_COMPILER_UNKNOWN: Force GLM not to detect the C++ compiler", "md_vendor_2glm_2manual.html#autotoc_md29", null ],
          [ "<a class=\"anchor\" id=\"section2_4\"></a> 2.4. GLM_FORCE_ARCH_UNKNOWN: Force GLM not to detect the build architecture", "md_vendor_2glm_2manual.html#autotoc_md30", null ],
          [ "<a class=\"anchor\" id=\"section2_5\"></a> 2.5. GLM_FORCE_CXX_UNKNOWN: Force GLM not to detect the C++ standard", "md_vendor_2glm_2manual.html#autotoc_md31", null ],
          [ "<a class=\"anchor\" id=\"section2_6\"></a> 2.6. GLM_FORCE_CXX**: C++ language detection", "md_vendor_2glm_2manual.html#autotoc_md32", null ],
          [ "<a class=\"anchor\" id=\"section2_7\"></a> 2.7. GLM_FORCE_EXPLICIT_CTOR: Requiring explicit conversions", "md_vendor_2glm_2manual.html#autotoc_md33", null ],
          [ "<a class=\"anchor\" id=\"section2_8\"></a> 2.8. GLM_FORCE_INLINE: Force inline", "md_vendor_2glm_2manual.html#autotoc_md34", null ],
          [ "<a class=\"anchor\" id=\"section2_9\"></a> 2.9. GLM_FORCE_ALIGNED_GENTYPES: Force GLM to enable aligned types", "md_vendor_2glm_2manual.html#autotoc_md35", null ],
          [ "<a class=\"anchor\" id=\"section2_10\"></a> 2.10. GLM_FORCE_DEFAULT_ALIGNED_GENTYPES: Force GLM to use aligned types by default", "md_vendor_2glm_2manual.html#autotoc_md36", null ],
          [ "<a class=\"anchor\" id=\"section2_11\"></a> 2.11. GLM_FORCE_INTRINSICS: Using SIMD optimizations", "md_vendor_2glm_2manual.html#autotoc_md37", null ],
          [ "<a class=\"anchor\" id=\"section2_12\"></a> 2.12. GLM_FORCE_PRECISION_**: Default precision", "md_vendor_2glm_2manual.html#autotoc_md38", null ],
          [ "<a class=\"anchor\" id=\"section2_13\"></a> 2.13. GLM_FORCE_SINGLE_ONLY: Removed explicit 64-bits floating point types", "md_vendor_2glm_2manual.html#autotoc_md39", null ],
          [ "<a class=\"anchor\" id=\"section2_14\"></a> 2.14. GLM_FORCE_SWIZZLE: Enable swizzle operators", "md_vendor_2glm_2manual.html#autotoc_md40", [
            [ "2.14.1. Swizzle functions for standard C++ 98", "md_vendor_2glm_2manual.html#autotoc_md41", null ],
            [ "2.14.2. Swizzle operations for C++ 98 with language extensions", "md_vendor_2glm_2manual.html#autotoc_md42", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section2_15\"></a> 2.15. GLM_FORCE_XYZW_ONLY: Only exposes x, y, z and w components", "md_vendor_2glm_2manual.html#autotoc_md43", null ],
          [ "<a class=\"anchor\" id=\"section2_16\"></a> 2.16. GLM_FORCE_LEFT_HANDED: Force left handed coordinate system", "md_vendor_2glm_2manual.html#autotoc_md44", null ],
          [ "<a class=\"anchor\" id=\"section2_17\"></a> 2.17. GLM_FORCE_DEPTH_ZERO_TO_ONE: Force the use of a clip space between 0 to 1", "md_vendor_2glm_2manual.html#autotoc_md45", null ],
          [ "<a class=\"anchor\" id=\"section2_18\"></a> 2.18. GLM_FORCE_SIZE_T_LENGTH: Vector and matrix static size", "md_vendor_2glm_2manual.html#autotoc_md46", null ],
          [ "<a class=\"anchor\" id=\"section2_19\"></a> 2.19. GLM_FORCE_UNRESTRICTED_GENTYPE: Removing genType restriction", "md_vendor_2glm_2manual.html#autotoc_md47", null ],
          [ "<a class=\"anchor\" id=\"section2_20\"></a> 2.20. GLM_FORCE_SILENT_WARNINGS: Silent C++ warnings from language extensions", "md_vendor_2glm_2manual.html#autotoc_md48", null ],
          [ "<a class=\"anchor\" id=\"section2_21\"></a> 2.21. GLM_FORCE_QUAT_DATA_XYZW: Force GLM to store quat data as x,y,z,w instead of w,x,y,z", "md_vendor_2glm_2manual.html#autotoc_md49", null ]
        ] ],
        [ "<a class=\"anchor\" id=\"section3\"></a> 3. Stable extensions", "md_vendor_2glm_2manual.html#autotoc_md51", [
          [ "<a class=\"anchor\" id=\"section3_1\"></a> 3.1. Scalar types", "md_vendor_2glm_2manual.html#autotoc_md52", [
            [ "3.1.1. GLM_EXT_scalar_int_sized", "md_vendor_2glm_2manual.html#autotoc_md53", null ],
            [ "3.1.2. GLM_EXT_scalar_uint_sized", "md_vendor_2glm_2manual.html#autotoc_md54", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_2\"></a> 3.2. Scalar functions", "md_vendor_2glm_2manual.html#autotoc_md55", [
            [ "3.2.1. GLM_EXT_scalar_common", "md_vendor_2glm_2manual.html#autotoc_md56", null ],
            [ "3.2.2. GLM_EXT_scalar_relational", "md_vendor_2glm_2manual.html#autotoc_md57", null ],
            [ "3.2.3. GLM_EXT_scalar_constants", "md_vendor_2glm_2manual.html#autotoc_md58", null ],
            [ "3.2.4. GLM_EXT_scalar_ulp", "md_vendor_2glm_2manual.html#autotoc_md59", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_3\"></a> 3.3. Vector types", "md_vendor_2glm_2manual.html#autotoc_md60", [
            [ "3.3.1. GLM_EXT_vector_float1", "md_vendor_2glm_2manual.html#autotoc_md61", null ],
            [ "3.3.2. GLM_EXT_vector_float2", "md_vendor_2glm_2manual.html#autotoc_md62", null ],
            [ "3.3.3. GLM_EXT_vector_float3", "md_vendor_2glm_2manual.html#autotoc_md63", null ],
            [ "3.3.4. GLM_EXT_vector_float4", "md_vendor_2glm_2manual.html#autotoc_md64", null ],
            [ "3.3.5. GLM_EXT_vector_double1", "md_vendor_2glm_2manual.html#autotoc_md65", null ],
            [ "3.3.6. GLM_EXT_vector_double2", "md_vendor_2glm_2manual.html#autotoc_md66", null ],
            [ "3.3.7. GLM_EXT_vector_double3", "md_vendor_2glm_2manual.html#autotoc_md67", null ],
            [ "3.3.8. GLM_EXT_vector_double4", "md_vendor_2glm_2manual.html#autotoc_md68", null ],
            [ "3.3.9. GLM_EXT_vector_int1", "md_vendor_2glm_2manual.html#autotoc_md69", null ],
            [ "3.3.10. GLM_EXT_vector_int2", "md_vendor_2glm_2manual.html#autotoc_md70", null ],
            [ "3.3.11. GLM_EXT_vector_int3", "md_vendor_2glm_2manual.html#autotoc_md71", null ],
            [ "3.3.12. GLM_EXT_vector_int4", "md_vendor_2glm_2manual.html#autotoc_md72", null ],
            [ "3.3.13. GLM_EXT_vector_int1", "md_vendor_2glm_2manual.html#autotoc_md73", null ],
            [ "3.3.14. GLM_EXT_vector_uint2", "md_vendor_2glm_2manual.html#autotoc_md74", null ],
            [ "3.3.15. GLM_EXT_vector_uint3", "md_vendor_2glm_2manual.html#autotoc_md75", null ],
            [ "3.3.16. GLM_EXT_vector_uint4", "md_vendor_2glm_2manual.html#autotoc_md76", null ],
            [ "3.3.17. GLM_EXT_vector_bool1", "md_vendor_2glm_2manual.html#autotoc_md77", null ],
            [ "3.3.18. GLM_EXT_vector_bool2", "md_vendor_2glm_2manual.html#autotoc_md78", null ],
            [ "3.3.19. GLM_EXT_vector_bool3", "md_vendor_2glm_2manual.html#autotoc_md79", null ],
            [ "3.3.20. GLM_EXT_vector_bool4", "md_vendor_2glm_2manual.html#autotoc_md80", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_4\"></a> 3.4. Vector types with precision qualifiers", "md_vendor_2glm_2manual.html#autotoc_md81", [
            [ "3.4.1. GLM_EXT_vector_float1_precision", "md_vendor_2glm_2manual.html#autotoc_md82", null ],
            [ "3.4.2. GLM_EXT_vector_float2_precision", "md_vendor_2glm_2manual.html#autotoc_md83", null ],
            [ "3.4.3. GLM_EXT_vector_float3_precision", "md_vendor_2glm_2manual.html#autotoc_md84", null ],
            [ "3.4.4. GLM_EXT_vector_float4_precision", "md_vendor_2glm_2manual.html#autotoc_md85", null ],
            [ "3.4.5. GLM_EXT_vector_double1_precision", "md_vendor_2glm_2manual.html#autotoc_md86", null ],
            [ "3.4.6. GLM_EXT_vector_double2_precision", "md_vendor_2glm_2manual.html#autotoc_md87", null ],
            [ "3.4.7. GLM_EXT_vector_double3_precision", "md_vendor_2glm_2manual.html#autotoc_md88", null ],
            [ "3.4.8. GLM_EXT_vector_double4_precision", "md_vendor_2glm_2manual.html#autotoc_md89", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_5\"></a> 3.5. Vector functions", "md_vendor_2glm_2manual.html#autotoc_md90", [
            [ "3.5.1. GLM_EXT_vector_common", "md_vendor_2glm_2manual.html#autotoc_md91", null ],
            [ "3.5.2. GLM_EXT_vector_relational", "md_vendor_2glm_2manual.html#autotoc_md92", null ],
            [ "3.5.3. GLM_EXT_vector_ulp", "md_vendor_2glm_2manual.html#autotoc_md93", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_6\"></a> 3.6. Matrix types", "md_vendor_2glm_2manual.html#autotoc_md94", [
            [ "3.6.1. GLM_EXT_matrix_float2x2", "md_vendor_2glm_2manual.html#autotoc_md95", null ],
            [ "3.6.2. GLM_EXT_matrix_float2x3", "md_vendor_2glm_2manual.html#autotoc_md96", null ],
            [ "3.6.3. GLM_EXT_matrix_float2x4", "md_vendor_2glm_2manual.html#autotoc_md97", null ],
            [ "3.6.4. GLM_EXT_matrix_float3x2", "md_vendor_2glm_2manual.html#autotoc_md98", null ],
            [ "3.6.5. GLM_EXT_matrix_float3x3", "md_vendor_2glm_2manual.html#autotoc_md99", null ],
            [ "3.6.6. GLM_EXT_matrix_float3x4", "md_vendor_2glm_2manual.html#autotoc_md100", null ],
            [ "3.6.7. GLM_EXT_matrix_float4x2", "md_vendor_2glm_2manual.html#autotoc_md101", null ],
            [ "3.6.8. GLM_EXT_matrix_float4x3", "md_vendor_2glm_2manual.html#autotoc_md102", null ],
            [ "3.6.9. GLM_EXT_matrix_float4x4", "md_vendor_2glm_2manual.html#autotoc_md103", null ],
            [ "3.6.10. GLM_EXT_matrix_double2x2", "md_vendor_2glm_2manual.html#autotoc_md104", null ],
            [ "3.6.11. GLM_EXT_matrix_double2x3", "md_vendor_2glm_2manual.html#autotoc_md105", null ],
            [ "3.6.12. GLM_EXT_matrix_double2x4", "md_vendor_2glm_2manual.html#autotoc_md106", null ],
            [ "3.6.13. GLM_EXT_matrix_double3x2", "md_vendor_2glm_2manual.html#autotoc_md107", null ],
            [ "3.6.14. GLM_EXT_matrix_double3x3", "md_vendor_2glm_2manual.html#autotoc_md108", null ],
            [ "3.6.15. GLM_EXT_matrix_double3x4", "md_vendor_2glm_2manual.html#autotoc_md109", null ],
            [ "3.6.16. GLM_EXT_matrix_double4x2", "md_vendor_2glm_2manual.html#autotoc_md110", null ],
            [ "3.6.17. GLM_EXT_matrix_double4x3", "md_vendor_2glm_2manual.html#autotoc_md111", null ],
            [ "3.6.18. GLM_EXT_matrix_double4x4", "md_vendor_2glm_2manual.html#autotoc_md112", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_7\"></a> 3.7. Matrix types with precision qualifiers", "md_vendor_2glm_2manual.html#autotoc_md113", [
            [ "3.7.1. GLM_EXT_matrix_float2x2_precision", "md_vendor_2glm_2manual.html#autotoc_md114", null ],
            [ "3.7.2. GLM_EXT_matrix_float2x3_precision", "md_vendor_2glm_2manual.html#autotoc_md115", null ],
            [ "3.7.3. GLM_EXT_matrix_float2x4_precision", "md_vendor_2glm_2manual.html#autotoc_md116", null ],
            [ "3.7.4. GLM_EXT_matrix_float3x2_precision", "md_vendor_2glm_2manual.html#autotoc_md117", null ],
            [ "3.7.5. GLM_EXT_matrix_float3x3_precision", "md_vendor_2glm_2manual.html#autotoc_md118", null ],
            [ "3.7.6. GLM_EXT_matrix_float3x4_precision", "md_vendor_2glm_2manual.html#autotoc_md119", null ],
            [ "3.7.7. GLM_EXT_matrix_float4x2_precision", "md_vendor_2glm_2manual.html#autotoc_md120", null ],
            [ "3.7.8. GLM_EXT_matrix_float4x3_precision", "md_vendor_2glm_2manual.html#autotoc_md121", null ],
            [ "3.7.9. GLM_EXT_matrix_float4x4_precision", "md_vendor_2glm_2manual.html#autotoc_md122", null ],
            [ "3.7.10. GLM_EXT_matrix_double2x2_precision", "md_vendor_2glm_2manual.html#autotoc_md123", null ],
            [ "3.7.11. GLM_EXT_matrix_double2x3_precision", "md_vendor_2glm_2manual.html#autotoc_md124", null ],
            [ "3.7.12. GLM_EXT_matrix_double2x4_precision", "md_vendor_2glm_2manual.html#autotoc_md125", null ],
            [ "3.7.13. GLM_EXT_matrix_double3x2_precision", "md_vendor_2glm_2manual.html#autotoc_md126", null ],
            [ "3.7.14. GLM_EXT_matrix_double3x3_precision", "md_vendor_2glm_2manual.html#autotoc_md127", null ],
            [ "3.7.15. GLM_EXT_matrix_double3x4_precision", "md_vendor_2glm_2manual.html#autotoc_md128", null ],
            [ "3.7.16. GLM_EXT_matrix_double4x2_precision", "md_vendor_2glm_2manual.html#autotoc_md129", null ],
            [ "3.7.17. GLM_EXT_matrix_double4x3_precision", "md_vendor_2glm_2manual.html#autotoc_md130", null ],
            [ "3.7.18. GLM_EXT_matrix_double4x4_precision", "md_vendor_2glm_2manual.html#autotoc_md131", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_8\"></a> 3.8. Matrix functions", "md_vendor_2glm_2manual.html#autotoc_md132", [
            [ "3.8.1. GLM_EXT_matrix_relational", "md_vendor_2glm_2manual.html#autotoc_md133", null ],
            [ "3.8.2. GLM_EXT_matrix_transform", "md_vendor_2glm_2manual.html#autotoc_md134", null ],
            [ "3.8.3. GLM_EXT_matrix_clip_space", "md_vendor_2glm_2manual.html#autotoc_md135", null ],
            [ "3.8.4. GLM_EXT_matrix_projection", "md_vendor_2glm_2manual.html#autotoc_md136", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_9\"></a> 3.9. Quaternion types", "md_vendor_2glm_2manual.html#autotoc_md137", [
            [ "3.9.1. GLM_EXT_quaternion_float", "md_vendor_2glm_2manual.html#autotoc_md138", null ],
            [ "3.9.2. GLM_EXT_quaternion_double", "md_vendor_2glm_2manual.html#autotoc_md139", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_10\"></a> 3.10. Quaternion types with precision qualifiers", "md_vendor_2glm_2manual.html#autotoc_md140", [
            [ "3.10.1. GLM_EXT_quaternion_float_precision", "md_vendor_2glm_2manual.html#autotoc_md141", null ],
            [ "3.10.2. GLM_EXT_quaternion_double_precision", "md_vendor_2glm_2manual.html#autotoc_md142", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section3_11\"></a> 3.11. Quaternion functions", "md_vendor_2glm_2manual.html#autotoc_md143", [
            [ "3.11.1. GLM_EXT_quaternion_common", "md_vendor_2glm_2manual.html#autotoc_md144", null ],
            [ "3.11.2. GLM_EXT_quaternion_geometric", "md_vendor_2glm_2manual.html#autotoc_md145", null ],
            [ "3.11.3. GLM_EXT_quaternion_trigonometric", "md_vendor_2glm_2manual.html#autotoc_md146", null ],
            [ "3.11.4. GLM_EXT_quaternion_exponential", "md_vendor_2glm_2manual.html#autotoc_md147", null ],
            [ "3.11.5. GLM_EXT_quaternion_relational", "md_vendor_2glm_2manual.html#autotoc_md148", null ],
            [ "3.11.6. GLM_EXT_quaternion_transform", "md_vendor_2glm_2manual.html#autotoc_md149", null ]
          ] ]
        ] ],
        [ "<a class=\"anchor\" id=\"section4\"></a> 4. Recommended extensions", "md_vendor_2glm_2manual.html#autotoc_md151", [
          [ "<a class=\"anchor\" id=\"section4_1\"></a> 4.1. GLM_GTC_bitfield", "md_vendor_2glm_2manual.html#autotoc_md152", null ],
          [ "<a class=\"anchor\" id=\"section4_2\"></a> 4.2. GLM_GTC_color_space", "md_vendor_2glm_2manual.html#autotoc_md153", null ],
          [ "<a class=\"anchor\" id=\"section4_3\"></a> 4.3. GLM_GTC_constants", "md_vendor_2glm_2manual.html#autotoc_md154", null ],
          [ "<a class=\"anchor\" id=\"section4_4\"></a> 4.4. GLM_GTC_epsilon", "md_vendor_2glm_2manual.html#autotoc_md155", null ],
          [ "<a class=\"anchor\" id=\"section4_5\"></a> 4.5. GLM_GTC_integer", "md_vendor_2glm_2manual.html#autotoc_md156", null ],
          [ "<a class=\"anchor\" id=\"section4_6\"></a> 4.6. GLM_GTC_matrix_access", "md_vendor_2glm_2manual.html#autotoc_md157", null ],
          [ "<a class=\"anchor\" id=\"section4_7\"></a> 4.7. GLM_GTC_matrix_integer", "md_vendor_2glm_2manual.html#autotoc_md158", null ],
          [ "<a class=\"anchor\" id=\"section4_8\"></a> 4.8. GLM_GTC_matrix_inverse", "md_vendor_2glm_2manual.html#autotoc_md159", null ],
          [ "<a class=\"anchor\" id=\"section4_9\"></a> 4.9. GLM_GTC_matrix_transform", "md_vendor_2glm_2manual.html#autotoc_md160", null ],
          [ "<a class=\"anchor\" id=\"section4_10\"></a> 4.10. GLM_GTC_noise", "md_vendor_2glm_2manual.html#autotoc_md161", null ],
          [ "<a class=\"anchor\" id=\"section4_11\"></a> 4.11. GLM_GTC_packing", "md_vendor_2glm_2manual.html#autotoc_md162", null ],
          [ "<a class=\"anchor\" id=\"section4_12\"></a> 4.12. GLM_GTC_quaternion", "md_vendor_2glm_2manual.html#autotoc_md163", null ],
          [ "<a class=\"anchor\" id=\"section4_13\"></a> 4.13. GLM_GTC_random", "md_vendor_2glm_2manual.html#autotoc_md164", null ],
          [ "<a class=\"anchor\" id=\"section4_14\"></a> 4.14. GLM_GTC_reciprocal", "md_vendor_2glm_2manual.html#autotoc_md165", null ],
          [ "<a class=\"anchor\" id=\"section4_15\"></a> 4.15. GLM_GTC_round", "md_vendor_2glm_2manual.html#autotoc_md166", null ],
          [ "<a class=\"anchor\" id=\"section4_16\"></a> 4.16. GLM_GTC_type_aligned", "md_vendor_2glm_2manual.html#autotoc_md167", null ],
          [ "<a class=\"anchor\" id=\"section4_17\"></a> 4.17. GLM_GTC_type_precision", "md_vendor_2glm_2manual.html#autotoc_md168", null ],
          [ "<a class=\"anchor\" id=\"section4_18\"></a> 4.18. GLM_GTC_type_ptr", "md_vendor_2glm_2manual.html#autotoc_md169", null ],
          [ "<a class=\"anchor\" id=\"section4_19\"></a> 4.19. GLM_GTC_ulp", "md_vendor_2glm_2manual.html#autotoc_md170", null ],
          [ "<a class=\"anchor\" id=\"section4_20\"></a> 4.20. GLM_GTC_vec1", "md_vendor_2glm_2manual.html#autotoc_md171", null ]
        ] ],
        [ "<a class=\"anchor\" id=\"section5\"></a> 5. OpenGL interoperability", "md_vendor_2glm_2manual.html#autotoc_md173", [
          [ "<a class=\"anchor\" id=\"section5_1\"></a> 5.1. GLM replacements for deprecated OpenGL functions", "md_vendor_2glm_2manual.html#autotoc_md174", null ],
          [ "<a class=\"anchor\" id=\"section5_2\"></a> 5.2. GLM replacements for GLU functions", "md_vendor_2glm_2manual.html#autotoc_md175", null ]
        ] ],
        [ "<a class=\"anchor\" id=\"section6\"></a> 6. Known issues", "md_vendor_2glm_2manual.html#autotoc_md177", [
          [ "<a class=\"anchor\" id=\"section6_1\"></a> 6.1. not function", "md_vendor_2glm_2manual.html#autotoc_md178", null ],
          [ "<a class=\"anchor\" id=\"section6_2\"></a> 6.2. Precision qualifiers support", "md_vendor_2glm_2manual.html#autotoc_md179", null ]
        ] ],
        [ "<a class=\"anchor\" id=\"section7\"></a> 7. FAQ", "md_vendor_2glm_2manual.html#autotoc_md181", [
          [ "<a class=\"anchor\" id=\"section7_1\"></a> 7.1 Why GLM follows GLSL specification and conventions?", "md_vendor_2glm_2manual.html#autotoc_md182", null ],
          [ "<a class=\"anchor\" id=\"section7_2\"></a> 7.2. Does GLM run GLSL program?", "md_vendor_2glm_2manual.html#autotoc_md183", null ],
          [ "<a class=\"anchor\" id=\"section7_3\"></a> 7.3. Does a GLSL compiler build GLM codes?", "md_vendor_2glm_2manual.html#autotoc_md184", null ],
          [ "<a class=\"anchor\" id=\"section7_4\"></a> 7.4. Should I use 'GTX' extensions?", "md_vendor_2glm_2manual.html#autotoc_md185", null ],
          [ "<a class=\"anchor\" id=\"section7_5\"></a> 7.5. Where can I ask my questions?", "md_vendor_2glm_2manual.html#autotoc_md186", null ],
          [ "<a class=\"anchor\" id=\"section7_6\"></a> 7.6. Where can I find the documentation of extensions?", "md_vendor_2glm_2manual.html#autotoc_md187", null ],
          [ "<a class=\"anchor\" id=\"section7_7\"></a> 7.7. Should I use 'using namespace glm;'?", "md_vendor_2glm_2manual.html#autotoc_md188", null ],
          [ "<a class=\"anchor\" id=\"section7_8\"></a> 7.8. Is GLM fast?", "md_vendor_2glm_2manual.html#autotoc_md189", null ],
          [ "<a class=\"anchor\" id=\"section7_9\"></a> 7.9. When I build with Visual C++ with /W4 warning level, I have warnings...", "md_vendor_2glm_2manual.html#autotoc_md190", null ],
          [ "<a class=\"anchor\" id=\"section7_10\"></a> 7.10. Why some GLM functions can crash because of division by zero?", "md_vendor_2glm_2manual.html#autotoc_md191", null ],
          [ "<a class=\"anchor\" id=\"section7_11\"></a> 7.11. What unit for angles is used in GLM?", "md_vendor_2glm_2manual.html#autotoc_md192", null ],
          [ "<a class=\"anchor\" id=\"section7_12\"></a> 7.12. Windows headers cause build errors...", "md_vendor_2glm_2manual.html#autotoc_md193", null ],
          [ "<a class=\"anchor\" id=\"section7_13\"></a> 7.13. Constant expressions support", "md_vendor_2glm_2manual.html#autotoc_md194", null ]
        ] ],
        [ "<a class=\"anchor\" id=\"section8\"></a> 8. Code samples", "md_vendor_2glm_2manual.html#autotoc_md196", [
          [ "<a class=\"anchor\" id=\"section8_1\"></a> 8.1. Compute a triangle normal", "md_vendor_2glm_2manual.html#autotoc_md197", null ],
          [ "<a class=\"anchor\" id=\"section8_2\"></a> 8.2. Matrix transform", "md_vendor_2glm_2manual.html#autotoc_md198", null ],
          [ "<a class=\"anchor\" id=\"section8_3\"></a> 8.3. Vector types", "md_vendor_2glm_2manual.html#autotoc_md199", null ],
          [ "<a class=\"anchor\" id=\"section8_4\"></a> 8.4. Lighting", "md_vendor_2glm_2manual.html#autotoc_md200", null ]
        ] ],
        [ "<a class=\"anchor\" id=\"section9\"></a> 9. Contributing to GLM", "md_vendor_2glm_2manual.html#autotoc_md202", [
          [ "<a class=\"anchor\" id=\"section9_1\"></a> 9.1. Submitting bug reports", "md_vendor_2glm_2manual.html#autotoc_md203", null ],
          [ "<a class=\"anchor\" id=\"section9_2\"></a> 9.2. Contributing to GLM with pull request", "md_vendor_2glm_2manual.html#autotoc_md204", [
            [ "Step 1: Setup our GLM Fork", "md_vendor_2glm_2manual.html#autotoc_md205", null ],
            [ "Step 2: Synchronizing our fork", "md_vendor_2glm_2manual.html#autotoc_md206", null ],
            [ "Step 3: Modifying our GLM Fork", "md_vendor_2glm_2manual.html#autotoc_md207", null ],
            [ "Step 4: Submitting a Pull Request", "md_vendor_2glm_2manual.html#autotoc_md208", null ]
          ] ],
          [ "<a class=\"anchor\" id=\"section9_3\"></a> 9.3. Coding style", "md_vendor_2glm_2manual.html#autotoc_md209", [
            [ "Indentation", "md_vendor_2glm_2manual.html#autotoc_md210", null ],
            [ "Spacing", "md_vendor_2glm_2manual.html#autotoc_md211", null ],
            [ "Blank lines", "md_vendor_2glm_2manual.html#autotoc_md212", null ],
            [ "Comments", "md_vendor_2glm_2manual.html#autotoc_md213", null ],
            [ "Cases", "md_vendor_2glm_2manual.html#autotoc_md214", null ]
          ] ]
        ] ],
        [ "<a class=\"anchor\" id=\"section10\"></a> 10. References", "md_vendor_2glm_2manual.html#autotoc_md216", [
          [ "<a class=\"anchor\" id=\"section10_1\"></a> 10.1. OpenGL specifications", "md_vendor_2glm_2manual.html#autotoc_md217", null ],
          [ "<a class=\"anchor\" id=\"section10_2\"></a> 10.2. External links", "md_vendor_2glm_2manual.html#autotoc_md218", null ],
          [ "<a class=\"anchor\" id=\"section10_3\"></a> 10.3. Projects using GLM", "md_vendor_2glm_2manual.html#autotoc_md219", null ],
          [ "<a class=\"anchor\" id=\"section10_4\"></a> 10.4. Tutorials using GLM", "md_vendor_2glm_2manual.html#autotoc_md220", null ],
          [ "<a class=\"anchor\" id=\"section10_5\"></a> 10.5. Equivalent for other languages", "md_vendor_2glm_2manual.html#autotoc_md221", null ],
          [ "<a class=\"anchor\" id=\"section10_6\"></a> 10.6. Alternatives to GLM", "md_vendor_2glm_2manual.html#autotoc_md222", null ],
          [ "<a class=\"anchor\" id=\"section10_7\"></a> 10.7. Acknowledgements", "md_vendor_2glm_2manual.html#autotoc_md223", null ]
        ] ]
      ] ]
    ] ],
    [ "Topics", "topics.html", "topics" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Functions", "globals_func.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"graphite_8h.html#ae5b43c72e43f9502e58a11f912e04587",
"group__modifier__keys.html#gabec28a498bc44bac62d772bdd6ddb6da",
"md_vendor_2glm_2manual.html#autotoc_md63",
"struct_lattice_1_1_ray3_d.html#af8bdb8c079fb75d481988ecf83c73f0f"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';