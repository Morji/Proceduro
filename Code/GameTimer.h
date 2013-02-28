//=======================================================================================
// GameTimer.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef GAMETIMER_H
#define GAMETIMER_H


class GameTimer
{
public:
	GameTimer();

	float getGameTime()const;  // in seconds
	float getDeltaTime()const; // in seconds
	
	int	  getFps()const;
	long  getCurrTime()const;  // in millis

	void reset(); // Call before message loop.
	void start(); // Call when unpaused.
	void stop();  // Call when paused.
	void tick();  // Call every frame.
	void frame(); // Call every frame to compute fps

private:
	double mSecondsPerCount;
	double mDeltaTime;
	int	   mFps;

	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;

	bool mStopped;
};

#endif // GAMETIMER_H