/*
 * Copyright (c) 2013-2014 Chun-Ying Huang
 *
 * This file is part of GamingAnywhere (GA).
 *
 * GA is free software; you can redistribute it and/or modify it
 * under the terms of the 3-clause BSD License as published by the
 * Free Software Foundation: http://directory.fsf.org/wiki/License:BSD_3Clause
 *
 * GA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the 3-clause BSD License along with GA;
 * if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "ga-common.h"
#include "ga-videolivesource.h"

EventTriggerId GAVideoLiveSource::eventTriggerId = 0;
unsigned GAVideoLiveSource::referenceCount = 0;

GAVideoLiveSource * GAVideoLiveSource
::createNew(UsageEnvironment& env/* TODO: more params */) {
	return new GAVideoLiveSource(env);
}

GAVideoLiveSource
::GAVideoLiveSource(UsageEnvironment& env)
		: FramedSource(env) {
	//
	if (referenceCount == 0) {
		// Any global initialization of the device would be done here:
		// TODO: create the pipeline
		// TODO: register event trigger function
	}
	++referenceCount;
	// Any instance-specific initialization of the device would be done here:
	if (eventTriggerId == 0) {
		eventTriggerId = envir().taskScheduler().createEventTrigger(deliverFrame0);
	}
}

GAVideoLiveSource
::~GAVideoLiveSource() {
	// Any instance-specific 'destruction' (i.e., resetting) of the device would be done here:
	--referenceCount;
	if (referenceCount == 0) {
		// Any global 'destruction' (i.e., resetting) of the device would be done here:
		// TODO: destroy the pipeline
		// Reclaim our 'event trigger'
		envir().taskScheduler().deleteEventTrigger(eventTriggerId);
		eventTriggerId = 0;
	}
}

void GAVideoLiveSource
::deliverFrame0(void* clientData) {
	((GAVideoLiveSource*) clientData)->deliverFrame();
}

void GAVideoLiveSource
::doGetNextFrame() {
	// This function is called (by our 'downstream' object) when it asks for new data.
	// Note: If, for some reason, the source device stops being readable (e.g., it gets closed), then you do the following:
	if (0 /* the source stops being readable */ /*%%% TO BE WRITTEN %%%*/) {
		handleClosure(NULL);
		return;
	}
	// If a new frame of data is immediately available to be delivered, then do this now:
	if (0 /* a new frame of data is immediately available to be delivered*/ /*%%% TO BE WRITTEN %%%*/) {
		deliverFrame();
	}
	// No new data is immediately available to be delivered.  We don't do anything more here.
	// Instead, our event trigger must be called (e.g., from a separate thread) when new data becomes available.
}

void GAVideoLiveSource
::deliverFrame() {
	// This function is called when new frame data is available from the device.
	// We deliver this data by copying it to the 'downstream' object, using the following parameters (class members):
	// 'in' parameters (these should *not* be modified by this function):
	//     fTo: The frame data is copied to this address.
	//         (Note that the variable "fTo" is *not* modified.  Instead,
	//          the frame data is copied to the address pointed to by "fTo".)
	//     fMaxSize: This is the maximum number of bytes that can be copied
	//         (If the actual frame is larger than this, then it should
	//          be truncated, and "fNumTruncatedBytes" set accordingly.)
	// 'out' parameters (these are modified by this function):
	//     fFrameSize: Should be set to the delivered frame size (<= fMaxSize).
	//     fNumTruncatedBytes: Should be set iff the delivered frame would have been
	//         bigger than "fMaxSize", in which case it's set to the number of bytes
	//         that have been omitted.
	//     fPresentationTime: Should be set to the frame's presentation time
	//         (seconds, microseconds).  This time must be aligned with 'wall-clock time' - i.e., the time that you would get
	//         by calling "gettimeofday()".
	//     fDurationInMicroseconds: Should be set to the frame's duration, if known.
	//         If, however, the device is a 'live source' (e.g., encoded from a camera or microphone), then we probably don't need
	//         to set this variable, because - in this case - data will never arrive 'early'.
	// Note the code below.

	if (!isCurrentlyAwaitingData()) return; // we're not ready for the data yet

	u_int8_t* newFrameDataStart = (u_int8_t*)0xDEADBEEF; //%%% TO BE WRITTEN %%%
	unsigned newFrameSize = 0; //%%% TO BE WRITTEN %%%

	// Deliver the data here:
	if (newFrameSize > fMaxSize) {
		fFrameSize = fMaxSize;
		fNumTruncatedBytes = newFrameSize - fMaxSize;
	} else {
		fFrameSize = newFrameSize;
	}
	gettimeofday(&fPresentationTime, NULL); // If you have a more accurate time - e.g., from an encoder - then use that instead.
	// If the device is *not* a 'live source' (e.g., it comes instead from a file or buffer), then set "fDurationInMicroseconds" here.
	memmove(fTo, newFrameDataStart, fFrameSize);

	// After delivering the data, inform the reader that it is now available:
	FramedSource::afterGetting(this);
}

