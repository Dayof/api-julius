/*
 * RecoResult.h
 *
 *  Created on: Nov 17, 2009
 *      Author: 08080002701
 */

#ifndef RECORESULT_H_
#define RECORESULT_H_

#include <string>
using std::string;

namespace LapsAPI {

	class RecoResult
	{
	public:
		RecoResult();
		RecoResult(string uterrance, float confidence);
		string getUterrance();
		float getConfidence();
	private:
		string uterrance;
		float confidence;
	};
}

#endif /* RECORESULT_H_ */
