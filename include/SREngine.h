/**********************************************************
 * Recognizer.h
 *
 *  Created on: Aug 26, 2009 8:28:33 AM
 *      Author: Pedro Batista
 *      Federal University of Pará
 +********************************************************/

#ifndef SRENGINE_H_
#define SRENGINE_H_

#include <iostream>
#include <RecoResult.h>
#include <julius/julius.h>

namespace LapsAPI{

/**
 * Class to implement julius (decoder) and recognizer streams
 */
class SREngine {
	/**
	* This function will make the interface between the callback of julius
	* and Recognizer action
	*/
	friend void startOnRecognize(Recog *recog, void *recognizer);
public:
	/**
	 * Just allocate memory for recognizer
	 */
	SREngine();
	/**
	 * Creante an instance of the recognizer with configurations
	 * give in the jconfFilePath (jconf file)
	 */
	SREngine(char *jconfFilePath);
	/**
	 * Destructor just free memory
	 */
	~SREngine();
	/**
	 * Assumes that the input is from mic and when something is
	 * recognizer execute the methods added with OnRecognized
	 */
	void startRecognition();
	/**
	 * Stop recognition
	 */
	void stopRecognition();
	/**
	 * Pass the function that will be called when something is recognized
	 */
	void setOnRecognizeAction(void(*action)(RecoResult *recoResult));
private:
	/**
	 * Function called when something is recognized
	 */
	void onRecognize();
	/**
	 * Pointer function of the user function that will be called when
	 * something is recognized
	 */
	void (*actionRecognize)(RecoResult *recoResult);
	Recog *recog;
	Jconf *jconf;

	pthread_t threadRecog;
	pthread_mutex_t mutexPaused;
	pthread_cond_t condPaused;
	bool paused;
};

}

#endif /* SRENGINE_H_ */
