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
 
 #ifndef SIGNAL_PROCESSING_H
 #define SIGNAL_PROCESSING_H
 
 //include all glm library
 #include "../utils/Maths.h"
 
 
 namespace SignalProcessing{
 
	//extended kalman filter using accelerometer, gyroscope and magnetometer tri-axis sensors
	//based on paper: "An Extended Kalman Filter for Quaternion-Based Orientation Estimation Using MARG Sensors"
	glm::quat q;//result quaternion orientation onece there are at last 4 iterations
	glm::vec3 earthAccelerometer;
	glm::vec3 earthMagnetometer;
	//inverse of sampling frequency
	float period = 0.030f;
	
	void init(float samplingFrequency){
		period = 1.0f/samplingFrequency;
		q = glm::quat();
		earthAccelerometer = glm::vec3(0.0f,1.0f,0.0f);
		earthMagnetometer = glm::vec3(0.0f,1.0f,0.0f);
	}
	
	glm::quat deviceOrientation(glm::vec3 gyroscopeMeasured, glm::vec3 accelerometerMeasured, glm::vec3 magnetometerMeasured){
		//rotation matrix from previous quaternion orientation
		glm::mat3 rotationMatrix = glm::mat3(
			q.w*q.w + q.x*q.x - q.y*q.y - q.z*q.z, 		2.0f*(q.x*q.y - q.z*q.w), 				2.0f*(q.x*q.z + q.y*q.w),
			2.0f*(q.x*q.y + q.z*q.w),					q.w*q.w - q.x*q.x + q.y*q.y - q.z*q.z,	2.0f*(q.y*q.z - q.x*q.w),
			2.0f*(q.x*q.z - q.y*q.w),					2.0f*(q.y*q.z + q.x*q.w),				q.w*q.w - q.x*q.x - q.y*q.y + q.z*q.z);
		
		//derived matrix from different variables to get the Jacobian when using Gauss-Newton algorithm
		glm::mat3 rotationMatrixDerivedX = glm::mat3(
			2.0f*q.x, 	2.0f*q.y, 	2.0f*q.z,
			2.0f*q.y,	-2.0f*q.x,	-2.0f*q.w,
			2.0f*q.z,	2.0f*q.w,	-2.0f*q.x);
		
		glm::mat3 rotationMatrixDerivedY = glm::mat3(
			-2.0f*q.y,	2.0f*q.x,	2.0f*q.w,
			2.0f*q.x,	2.0f*q.y,	2.0f*q.z,
			-2.0f*q.w,	2.0f*q.z,	-2.0f*q.y);
		
		glm::mat3 rotationMatrixDerivedZ = glm::mat3(
			-2.0f*q.z, 	-2.0f*q.w, 	2.0f*q.x,
			2.0f*q.w,	-2.0f*q.z,	2.0f*q.y,
			2.0f*q.x,	2.0f*q.y,	2.0f*q.z);
		
		glm::mat3 rotationMatrixDerivedW = glm::mat3(
			2.0f*q.w,	-2.0f*q.z,	2.0f*q.y,
			2.0f*q.z,	2.0f*q.w,	-2.0f*q.x,
			-2.0f*q.y,	2.0f*q.x,	2.0f*q.w);
			
		//normalize accelerometer and magnetometer
		accelerometerMeasured = glm::normalize(accelerometerMeasured);
		magnetometerMeasured = glm::normalize(magnetometerMeasured);
		gyroscopeMeasured = gyroscopeMeasured * period;
		
		glm::vec3 earthAccelerometerTemporal = rotationMatrix * accelerometerMeasured;
		glm::vec3 earthMagnetometerTemporal = rotationMatrix * magnetometerMeasured;
		
		glm::vec3 earthAccelerometerTemporalDerivedX 	= 	rotationMatrixDerivedX	* 	accelerometerMeasured;
		glm::vec3 earthMagnetometerTemporalDerivedX 	= 	rotationMatrixDerivedX	* 	magnetometerMeasured;
		
		glm::vec3 earthAccelerometerTemporalDerivedY 	= 	rotationMatrixDerivedY	* 	accelerometerMeasured;
		glm::vec3 earthMagnetometerTemporalDerivedY 	= 	rotationMatrixDerivedY	* 	magnetometerMeasured;
		
		glm::vec3 earthAccelerometerTemporalDerivedZ 	= 	rotationMatrixDerivedZ	* 	accelerometerMeasured;
		glm::vec3 earthMagnetometerTemporalDerivedZ 	= 	rotationMatrixDerivedZ	* 	magnetometerMeasured;
		
		glm::vec3 earthAccelerometerTemporalDerivedW 	= 	rotationMatrixDerivedW 	* 	accelerometerMeasured;
		glm::vec3 earthMagnetometerTemporalDerivedW 	= 	rotationMatrixDerivedW 	* 	magnetometerMeasured;
		
		//calculate the square error between previous earth accelerometer and magnetometer
		//and current sampled accelerometer and magnetometer multiplied per rotation matrix
		float xAccDiff = earthAccelerometer.x - earthAccelerometerTemporal.x;
		float yAccDiff = earthAccelerometer.y - earthAccelerometerTemporal.y;
		float zAccDiff = earthAccelerometer.z - earthAccelerometerTemporal.z;
		float xMagDiff = earthMagnetometer.x - earthMagnetometerTemporal.x;
		float yMagDiff = earthMagnetometer.y - earthMagnetometerTemporal.y;
		float zMagDiff = earthMagnetometer.z - earthMagnetometerTemporal.z;
		
		//saving new variables
		earthAccelerometer = earthAccelerometerTemporal;
		earthMagnetometer = earthMagnetometerTemporal;
		
		float error = xAccDiff*xAccDiff + yAccDiff*yAccDiff + zAccDiff*zAccDiff + xMagDiff*xMagDiff + yMagDiff*yMagDiff + zMagDiff*zMagDiff;
		
		//gauss-newton algorithm
		glm::vec3 jacobianAcceleration = - earthAccelerometerTemporalDerivedX * earthAccelerometerTemporalDerivedY * earthAccelerometerTemporalDerivedZ * earthAccelerometerTemporalDerivedW;
		glm::vec3 jacobianMagnetometer = - earthMagnetometerTemporalDerivedX * earthMagnetometerTemporalDerivedY * earthMagnetometerTemporalDerivedZ * earthMagnetometerTemporalDerivedW;
		float jacobianInverseTransposeMultiplication = 	jacobianAcceleration.x*jacobianAcceleration.x + jacobianAcceleration.y*jacobianAcceleration.y + jacobianAcceleration.z*jacobianAcceleration.z + 
														jacobianMagnetometer.x*jacobianMagnetometer.x + jacobianMagnetometer.y*jacobianMagnetometer.y + jacobianMagnetometer.z*jacobianMagnetometer.z;
		
		q.x = q.x - (jacobianAcceleration.x + jacobianMagnetometer.x)*error/jacobianInverseTransposeMultiplication;
		q.y = q.y - (jacobianAcceleration.y + jacobianMagnetometer.y)*error/jacobianInverseTransposeMultiplication;
		q.z = q.z - (jacobianAcceleration.z + jacobianMagnetometer.z)*error/jacobianInverseTransposeMultiplication;
		q.w = q.w;
		q = glm::normalize(q);
		return q;
	}
 };
 
 #endif