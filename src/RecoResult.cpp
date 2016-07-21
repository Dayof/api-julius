/*
 * RecoResult.cpp
 *
 *  Created on: Nov 17, 2009
 *      Author: 08080002701
 */

#include <RecoResult.h>

namespace LapsAPI{

	RecoResult::RecoResult() {
	}

	RecoResult::RecoResult(string uterrance, float confidence) {
		this->uterrance = uterrance;
		this->confidence = confidence;
	}

	string RecoResult::getUterrance(){
		return this->uterrance;
	}

	float RecoResult::getConfidence(){
		return this->confidence;
	}
}
