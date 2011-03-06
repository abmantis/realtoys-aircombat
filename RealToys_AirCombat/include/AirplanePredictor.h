#ifndef _AIRPLANEPREDICTOR_H_
#define _AIRPLANEPREDICTOR_H_

#define PACKETDELAYQUEUEMAXIZE 5

class AirplanePredictor
{
public:

	AirplanePredictor(void)
	{
		mEstimatedDelay = 0;
		mLastPacketArrive = 0;
		mQueueSize = 0;
	}

	~AirplanePredictor(void)
	{
	}

	void add(Ogre::Vector3 currPos, Ogre::Vector3 currVel, Ogre::Vector3 packetPos, Ogre::Vector3 packetVel)
	{
		if(mLastPacketArrive > 0)
		{
			if(RakNet::GetTime() - mLastPacketArrive)
				return;

			mTimeSum = 0;	
			mPacketDelayQueue.Push(RakNet::GetTime() - mLastPacketArrive);
			
			mQueueSize = mPacketDelayQueue.Size();
			if(mQueueSize > PACKETDELAYQUEUEMAXIZE)
			{
				mPacketDelayQueue.Pop();
				mQueueSize -= 1;
			}

			for(unsigned int i = 0; i < mQueueSize; i++)
			{
				mTimeSum += mPacketDelayQueue[i];
			}
			mEstimatedDelay = mTimeSum*1.0f / mQueueSize;
			//std::cout << mEstimatedDelay << std::endl;

			mCoord1 = currPos;
			mCoord2 = packetPos + packetVel*mEstimatedDelay*0.001f;
			mCoord4 = packetPos + packetVel*mEstimatedDelay*0.001f;
			mCoord3 = (mCoord2 - currPos) / ( mEstimatedDelay*0.001f) ; 
		}

		


		
		//std::cout << "P " << mCoord4 << std::endl;
		//std::cout << "curr packet pos: " << packetPos << " In one sec: " << mCoord4;
		
		
		mLastPacketArrive = RakNet::GetTime();
	}
	/*
		param pos -and vel- are in/out params!!!
		in: current position and velocity
		out: new position -and velocity-

		timeSinceLastCall is the time passed since the last call to this function.
		timeSinceLastCall is only used when interpolation is not possible
	*/
	void get(Ogre::Vector3 &pos, Ogre::Vector3 &vel, float timeSinceLastCall)
	{
		mTimeSincePacket = RakNet::GetTime() - mLastPacketArrive ;
		mCurrentT =  mTimeSincePacket*1.0f / mEstimatedDelay;
		//should we use the spline?
		if(mQueueSize > 0 && mCurrentT <=1 )
		{			
			
			//std::cout << mCurrentT << std::endl;
			/*pos.x = (mCoord4.x - 3*mCoord3.x + 3*mCoord2.x - mCoord1.x) * mCurrentT * mCurrentT * mCurrentT
				+ (3*mCoord3.x - 6*mCoord2.x + 3*mCoord1.x) * mCurrentT * mCurrentT
				+ (3*mCoord2.x - 3*mCoord1.x) * mCurrentT
				+ mCoord1.x;

			pos.y = (mCoord4.y - 3*mCoord3.y + 3*mCoord2.y - mCoord1.y) * mCurrentT * mCurrentT * mCurrentT
				+ (3*mCoord3.y - 6*mCoord2.y + 3*mCoord1.y) * mCurrentT * mCurrentT
				+ (3*mCoord2.y - 3*mCoord1.y) *mCurrentT
				+ mCoord1.y;

			pos.z = (mCoord4.z - 3*mCoord3.z + 3*mCoord2.z - mCoord1.z) * mCurrentT * mCurrentT * mCurrentT
				+ (3*mCoord3.z - 6*mCoord2.z + 3*mCoord1.z) * mCurrentT * mCurrentT
				+ (3*mCoord2.z - 3*mCoord1.z) *mCurrentT
				+ mCoord1.z;*/

			/*pos = ( (mCoord4 - 3*mCoord3 + 3*mCoord2 - mCoord1) * mCurrentT * mCurrentT * mCurrentT )
				+ ( (3*mCoord3 - 6*mCoord2 + 3*mCoord1) * mCurrentT * mCurrentT )
				+ ( (3*mCoord2 - 3*mCoord1) * mCurrentT )
				+ mCoord1;
			std::cout << "C      " << pos << std::endl;*/

			pos = mCoord1 + mCoord3*mTimeSincePacket;

			
		}
		else
		{
			std::cout << "NOT IN!!! "<< mEstimatedDelay << std::endl;
			pos += vel*timeSinceLastCall;
		}

/*
		x = At^3 + Bt^2 + Ct + D
		y = Et^3 + Ft^3 + Gt + H

			t is the time variable. It ranges from 0 at the initial point to 1 at the end point.

		A = x3 – 3*x2 +3*x1 – x0
		B = 3*x2 – 6*x1 + 3*x0
		C = 3*x1 – 3*x0
		D = x0

		E = y3 – 3*y2 +3*y1 – y0
		F = 3*y2 – 6*y1 + 3y0
		G = 3*y1 – 3*y0
		H = y0







		pos.x = (x3 – 3*x2 + 3x1 – x0) *mTimeSincePacket^3
		+ (3*x2 – 6*x1 + 3*x0) *mTimeSincePacket^2
		+ (3*x1 – 3*x0) *mTimeSincePacket
		+ x0;

		pos.y = (y3 – 3*y2 + 3y1 – y0) *mTimeSincePacket^3
		+ (3*y2 – 6*y1 + 3*y0) *mTimeSincePacket^2
		+ (3*y1 – 3*y0) *mTimeSincePacket
		+ y0;

		pos.z = (z3 – 3z2 + 3z1 – z0) *mTimeSincePacket^3
		+ (3*z2 – 6*z1 + 3*z0) *mTimeSincePacket^2
		+ (3*z1 – 3*z0) *mTimeSincePacket
		+ z0;
*/
	}

private:
	Ogre::Vector3 mCoord1, mCoord2, mCoord3, mCoord4;
	
	float mEstimatedDelay;
	RakNet::Time mLastPacketArrive;
	RakNet::Time mTimeSum;
	RakNet::Time mTimeSincePacket;

	float mCurrentT;

	unsigned int mQueueSize;


	DataStructures::Queue<RakNet::Time> mPacketDelayQueue;	//saves delay times between each packet

	

};
#endif



//#ifndef _AIRPLANEPREDICTOR_H_
//#define _AIRPLANEPREDICTOR_H_
//
//#define PACKETDELAYQUEUEMAXIZE 5
//
//class AirplanePredictor
//{
//public:
//
//AirplanePredictor(void)
//{
//mEstimatedDelay = 0;
//mLastPacketArrive = 0;
//mQueueSize = 0;
//}
//
//~AirplanePredictor(void)
//{
//}
//
//void add(Ogre::Vector3 currPos, Ogre::Vector3 currVel, Ogre::Vector3 packetPos, Ogre::Vector3 packetVel)
//{
//if(mLastPacketArrive > 0)
//{
//mTimeSum = 0;	
//mPacketDelayQueue.Push(RakNet::GetTime() - mLastPacketArrive);
//mQueueSize = mPacketDelayQueue.Size();
//if(mQueueSize > PACKETDELAYQUEUEMAXIZE)
//{
//mPacketDelayQueue.Pop();
//mQueueSize -= 1;
//}
//
//for(unsigned int i = 0; i < mQueueSize; i++)
//{
//mTimeSum += mPacketDelayQueue[i];
//}
//mEstimatedDelay = mTimeSum*1.0f / mQueueSize;
////std::cout << mEstimatedDelay << std::endl;
//}
//
//mCoord1 = currPos;
//mCoord2 = currPos + currVel;		
////mCoord4 = packetPos + packetVel*mEstimatedDelay*0.001f;
////mCoord3 = mCoord4 - packetVel;
//mCoord3 = packetPos;
//mCoord4 = packetPos + packetVel;
//
//
//std::cout << "P " << mCoord4 << std::endl;
////std::cout << "curr packet pos: " << packetPos << " In one sec: " << mCoord4;
//
//
//mLastPacketArrive = RakNet::GetTime();
//}
///*
//param pos -and vel- are in/out params!!!
//in: current position and velocity
//out: new position -and velocity-
//
//timeSinceLastCall is the time passed since the last call to this function.
//timeSinceLastCall is only used when interpolation is not possible
//*/
//void get(Ogre::Vector3 &pos, Ogre::Vector3 &vel, float timeSinceLastCall)
//{
//	mTimeSincePacket = RakNet::GetTime() - mLastPacketArrive ;
//	mCurrentT = ( mTimeSincePacket) / mEstimatedDelay;
//	//should we use the spline?
//	if(mQueueSize > 0 && mCurrentT <=1 )
//	{			
//
//		//std::cout << mCurrentT << std::endl;
//		/*pos.x = (mCoord4.x - 3*mCoord3.x + 3*mCoord2.x - mCoord1.x) * mCurrentT * mCurrentT * mCurrentT
//		+ (3*mCoord3.x - 6*mCoord2.x + 3*mCoord1.x) * mCurrentT * mCurrentT
//		+ (3*mCoord2.x - 3*mCoord1.x) * mCurrentT
//		+ mCoord1.x;
//
//		pos.y = (mCoord4.y - 3*mCoord3.y + 3*mCoord2.y - mCoord1.y) * mCurrentT * mCurrentT * mCurrentT
//		+ (3*mCoord3.y - 6*mCoord2.y + 3*mCoord1.y) * mCurrentT * mCurrentT
//		+ (3*mCoord2.y - 3*mCoord1.y) *mCurrentT
//		+ mCoord1.y;
//
//		pos.z = (mCoord4.z - 3*mCoord3.z + 3*mCoord2.z - mCoord1.z) * mCurrentT * mCurrentT * mCurrentT
//		+ (3*mCoord3.z - 6*mCoord2.z + 3*mCoord1.z) * mCurrentT * mCurrentT
//		+ (3*mCoord2.z - 3*mCoord1.z) *mCurrentT
//		+ mCoord1.z;*/
//
//		pos = ( (mCoord4 - 3*mCoord3 + 3*mCoord2 - mCoord1) * mCurrentT * mCurrentT * mCurrentT )
//			+ ( (3*mCoord3 - 6*mCoord2 + 3*mCoord1) * mCurrentT * mCurrentT )
//			+ ( (3*mCoord2 - 3*mCoord1) * mCurrentT )
//			+ mCoord1;
//		std::cout << "C      " << pos << std::endl;
//	}
//	else
//	{
//		//std::cout << "NOT IN!!! "<< mEstimatedDelay << std::endl;
//		pos += vel*timeSinceLastCall;
//	}
//
//	/*
//	x = At^3 + Bt^2 + Ct + D
//	y = Et^3 + Ft^3 + Gt + H
//
//	t is the time variable. It ranges from 0 at the initial point to 1 at the end point.
//
//	A = x3 – 3*x2 +3*x1 – x0
//	B = 3*x2 – 6*x1 + 3*x0
//	C = 3*x1 – 3*x0
//	D = x0
//
//	E = y3 – 3*y2 +3*y1 – y0
//	F = 3*y2 – 6*y1 + 3y0
//	G = 3*y1 – 3*y0
//	H = y0
//
//
//
//
//
//
//
//	pos.x = (x3 – 3*x2 + 3x1 – x0) *mTimeSincePacket^3
//	+ (3*x2 – 6*x1 + 3*x0) *mTimeSincePacket^2
//	+ (3*x1 – 3*x0) *mTimeSincePacket
//	+ x0;
//
//	pos.y = (y3 – 3*y2 + 3y1 – y0) *mTimeSincePacket^3
//	+ (3*y2 – 6*y1 + 3*y0) *mTimeSincePacket^2
//	+ (3*y1 – 3*y0) *mTimeSincePacket
//	+ y0;
//
//	pos.z = (z3 – 3z2 + 3z1 – z0) *mTimeSincePacket^3
//	+ (3*z2 – 6*z1 + 3*z0) *mTimeSincePacket^2
//	+ (3*z1 – 3*z0) *mTimeSincePacket
//	+ z0;
//	*/
//}
//
//private:
//	Ogre::Vector3 mCoord1, mCoord2, mCoord3, mCoord4;
//
//	float mEstimatedDelay;
//	RakNet::Time mLastPacketArrive;
//	RakNet::Time mTimeSum;
//	RakNet::Time mTimeSincePacket;
//
//	float mCurrentT;
//
//	unsigned int mQueueSize;
//
//
//	DataStructures::Queue<RakNet::Time> mPacketDelayQueue;	//saves delay times between each packet
//
//
//
//};
//#endif
