/*
 *  thin render - Mobile render engine based on OpenGL ES 2.0
 *  Copyright (C) 2013  Fundació i2CAT, Internet i Innovació digital a Catalunya
 *
 *  This file is part of thin render.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Author:         Marc Fernandez Vanaclocha <marc.fernandez@i2cat.net>
 */

#ifndef MATHS_H
#define MATHS_H
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/orthonormalize.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/norm.hpp"
#include "glm/gtx/fast_square_root.hpp"
#include "../utils/glm/gtc/type_precision.hpp"
#include <cmath>

/**
 * Maths static methods implements all the mathematical game requirments,
 * any mathematical operation that will be used often, will be placed here
 */
namespace Maths
{
	const static float PI = 3.141592653589793f;

	static inline float sinf(const float & alpha){
		return sin(alpha);
	}
	static inline float cosf(const float & alpha){
		return cos(alpha);
	}
	static inline float sqrtf(const float & x){
		return sqrt(x);
	}
    static inline float signf(const float & x){
        return x < 0.0f ? -1.0f : 1.0f;
    }
    static inline float powTwo(const float & x){
        return x*x;
    }
};

#endif
