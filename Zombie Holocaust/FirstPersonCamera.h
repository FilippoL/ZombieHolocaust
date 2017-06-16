#ifndef _FP_CAM_H_
#define _FP_CAM_H_

#include<xnamath.h>


class FirstPersonCamera
{
	public:
		FirstPersonCamera(void);
		~FirstPersonCamera(void);


		void SetDistance( float distance, float minDistance, float maxDistance );
		void SetRotation( float x, float y, float minY, float maxY );
		void SetTarget( XMFLOAT3& target );
		void SetPosition(float x, float y, float z);
		XMFLOAT3 GetPosition( );
		void SetDirection(float x, float y, float z);
		void Move(float leftRight, float backFoward);

		void ApplyZoom( float zoomDelta );
		void ApplyRotation( float yawDelta, float pitchDelta );

		

		XMMATRIX GetViewMatrix( );

	private:
		XMFLOAT3 position_;
		XMFLOAT3 target_;
		XMFLOAT3 direction_;

		float distance_, minDistance_, maxDistance_;
		float xRotation_, yRotation_, yMin_, yMax_;

		XMVECTOR DefaultForward_ ;
		XMVECTOR DefaultRight_;
		XMVECTOR camForward_;
		XMVECTOR camRight_;

		XMMATRIX camView_;

		XMVECTOR camUp_;
};

#endif