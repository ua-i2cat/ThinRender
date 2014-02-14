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
 #include "../log/Log.h"
 
 
 namespace SignalProcessing{
 
	//extended kalman filter using accelerometer, gyroscope and magnetometer tri-axis sensors
	//based on paper: "An Extended Kalman Filter for Quaternion-Based Orientation Estimation Using MARG Sensors"
	glm::quat q = glm::quat();//result quaternion orientation onece there are at last 4 iterations
	glm::vec3 earthAccelerometer;
	glm::vec3 earthMagnetometer;
	glm::vec3 previousGyroscope;
	//inverse of sampling frequency
	float period = 0.030f;
	
	void init(float samplingFrequency){
		period = 1.0f/samplingFrequency;
		q = glm::quat();
		earthAccelerometer = glm::vec3(0.0f,1.0f,0.0f);
		earthMagnetometer = glm::vec3(0.0f,1.0f,0.0f);
		previousGyroscope = glm::vec3(0.0f,0.0f,0.0f);
	}
	
	glm::quat deviceOrientation(glm::vec3 gyroscopeMeasured, glm::vec3 accelerometerMeasured, glm::vec3 magnetometerMeasured){
		//rotation matrix from previous quaternion orientation
		//logInf("deviceOrientation: gyroscopeMeasured {%f, %f, %f} accelerometerMeasured {%f, %f, %f} magnetometerMeasured {%f, %f, %f}", gyroscopeMeasured.x,gyroscopeMeasured.y, gyroscopeMeasured.z,
			//	accelerometerMeasured.x,accelerometerMeasured.y, accelerometerMeasured.z,
				//magnetometerMeasured.x,magnetometerMeasured.y, magnetometerMeasured.z);

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

		/*logInf("earthAccelerometerTemporalDerivedX {%f, %f, %f}", earthAccelerometerTemporalDerivedX.x,earthAccelerometerTemporalDerivedX.y,earthAccelerometerTemporalDerivedX.z);
		logInf("earthMagnetometerTemporalDerivedX {%f, %f, %f}", earthMagnetometerTemporalDerivedX.x,earthMagnetometerTemporalDerivedX.y,earthMagnetometerTemporalDerivedX.z);
		
		logInf("earthAccelerometerTemporalDerivedY {%f, %f, %f}", earthAccelerometerTemporalDerivedY.x,earthAccelerometerTemporalDerivedY.y,earthAccelerometerTemporalDerivedY.z);
		logInf("earthMagnetometerTemporalDerivedY {%f, %f, %f}", earthMagnetometerTemporalDerivedY.x,earthMagnetometerTemporalDerivedY.y,earthMagnetometerTemporalDerivedY.z);

		logInf("earthAccelerometerTemporalDerivedZ {%f, %f, %f}", earthAccelerometerTemporalDerivedZ.x,earthAccelerometerTemporalDerivedZ.y,earthAccelerometerTemporalDerivedZ.z);
		logInf("earthMagnetometerTemporalDerivedZ {%f, %f, %f}", earthMagnetometerTemporalDerivedZ.x,earthMagnetometerTemporalDerivedZ.y,earthMagnetometerTemporalDerivedZ.z);

		logInf("earthAccelerometerTemporalDerivedW {%f, %f, %f}", earthAccelerometerTemporalDerivedW.x,earthAccelerometerTemporalDerivedW.y,earthAccelerometerTemporalDerivedW.z);
		logInf("earthMagnetometerTemporalDerivedW {%f, %f, %f}", earthMagnetometerTemporalDerivedW.x,earthMagnetometerTemporalDerivedW.y,earthMagnetometerTemporalDerivedW.z);
		*///calculate the square error between previous earth accelerometer and magnetometer
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
		
		
		//gauss-newton algorithm
		glm::vec3 accX, accY, accZ, accW, magX, magY, magZ, magW;

		accX = earthAccelerometerTemporalDerivedX;
		accY = earthAccelerometerTemporalDerivedY;
		accZ = earthAccelerometerTemporalDerivedZ;
		accW = earthAccelerometerTemporalDerivedW;

		magX = earthMagnetometerTemporalDerivedX;
		magY = earthMagnetometerTemporalDerivedY;
		magZ = earthMagnetometerTemporalDerivedZ;
		magW = earthMagnetometerTemporalDerivedW;

		float mat11 = accX.x * accX.x + accX.y * accX.y + accX.z * accX.z + magX.x * magX.x + magX.y * magX.y + magX.z * magX.z;
		float mat12 = accX.x * accY.x + accX.y * accY.y + accX.z * accY.z + magX.x * magY.x + magX.y * magY.y + magX.z * magY.z;
		float mat13 = accX.x * accZ.x + accX.y * accZ.y + accX.z * accZ.z + magX.x * magZ.x + magX.y * magZ.y + magX.z * magZ.z;
		float mat14 = accX.x * accW.x + accX.y * accW.y + accX.z * accW.z + magX.x * magW.x + magX.y * magW.y + magX.z * magW.z;

		float mat21 = accY.x * accX.x + accY.y * accX.y + accY.z * accX.z + magY.x * magX.x + magY.y * magX.y + magY.z * magX.z;
		float mat22 = accY.x * accY.x + accY.y * accY.y + accY.z * accY.z + magY.x * magY.x + magY.y * magY.y + magY.z * magY.z;
		float mat23 = accY.x * accZ.x + accY.y * accZ.y + accY.z * accZ.z + magY.x * magZ.x + magY.y * magZ.y + magY.z * magZ.z;
		float mat24 = accY.x * accW.x + accY.y * accW.y + accY.z * accW.z + magY.x * magW.x + magY.y * magW.y + magY.z * magW.z;

		float mat31 = accZ.x * accX.x + accZ.y * accX.y + accZ.z * accX.z + magZ.x * magX.x + magZ.y * magX.y + magZ.z * magX.z;
		float mat32 = accZ.x * accY.x + accZ.y * accY.y + accZ.z * accY.z + magZ.x * magY.x + magZ.y * magY.y + magZ.z * magY.z;
		float mat33 = accZ.x * accZ.x + accZ.y * accZ.y + accZ.z * accZ.z + magZ.x * magZ.x + magZ.y * magZ.y + magZ.z * magZ.z;
		float mat34 = accZ.x * accW.x + accZ.y * accW.y + accZ.z * accW.z + magZ.x * magW.x + magZ.y * magW.y + magZ.z * magW.z;

		float mat41 = accW.x * accX.x + accW.y * accX.y + accW.z * accX.z + magW.x * magX.x + magW.y * magX.y + magW.z * magX.z;
		float mat42 = accW.x * accY.x + accW.y * accY.y + accW.z * accY.z + magW.x * magY.x + magW.y * magY.y + magW.z * magY.z;
		float mat43 = accW.x * accZ.x + accW.y * accZ.y + accW.z * accZ.z + magW.x * magZ.x + magW.y * magZ.y + magW.z * magZ.z;
		float mat44 = accW.x * accW.x + accW.y * accW.y + accW.z * accW.z + magW.x * magW.x + magW.y * magW.y + magW.z * magW.z;

		glm::mat4 jacobianInverseTransposeMultiplication = glm::inverse(glm::mat4(
				mat11, mat12, mat13, mat14,
				mat21, mat22, mat23, mat24,
				mat31, mat32, mat33, mat34,
				mat41, mat42, mat43, mat44
		));

		glm::vec4 jacobianResult = glm::vec4 (
				accW.x * xAccDiff + accW.y * yAccDiff + accW.z * zAccDiff + magW.x * xMagDiff + magW.y * yMagDiff + magW.z * zMagDiff,
				accX.x * xAccDiff + accX.y * yAccDiff + accX.z * zAccDiff + magX.x * xMagDiff + magX.y * yMagDiff + magX.z * zMagDiff,
				accY.x * xAccDiff + accY.y * yAccDiff + accY.z * zAccDiff + magY.x * xMagDiff + magY.y * yMagDiff + magY.z * zMagDiff,
				accZ.x * xAccDiff + accZ.y * yAccDiff + accZ.z * zAccDiff + magZ.x * xMagDiff + magZ.y * yMagDiff + magZ.z * zMagDiff
		);
		jacobianResult = jacobianInverseTransposeMultiplication * jacobianResult;

		q.x = q.x - jacobianResult.x;
		q.y = q.y - jacobianResult.y;
		q.z = q.z - jacobianResult.z;
		q.w = q.w - jacobianResult.w;

		//now execute the kalman filter with the previos quaternion calculated with gauss newton algorithm
		glm::quat result;
		result.x = 0.5f * ( previousGyroscope.z * q.y - previousGyroscope.y * q.z + previousGyroscope.x * q.w);
		result.y = 0.5f * (-previousGyroscope.z * q.x + previousGyroscope.x * q.z + previousGyroscope.y * q.w);
		result.z = 0.5f * ( previousGyroscope.y * q.x - previousGyroscope.x * q.y + previousGyroscope.z * q.w);
		result.w = 0.5f * (-previousGyroscope.x * q.x - previousGyroscope.y * q.y - previousGyroscope.z * q.z);

		previousGyroscope.x =  gyroscopeMeasured.x - previousGyroscope.x;
		previousGyroscope.y =  gyroscopeMeasured.y - previousGyroscope.y;
		previousGyroscope.z =  gyroscopeMeasured.z - previousGyroscope.z;

		q = result;
		q = glm::normalize(q);
		return q;
	}
 };
 
 #endif
