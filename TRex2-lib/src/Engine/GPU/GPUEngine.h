//
// This file is part of T-Rex, a Complex Event Processing Middleware.
// See http://home.dei.polimi.it/margara
//
// Authors: Alessandro Margara, Daniele Rogora
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#ifndef GPU_ENGINE_H
#define GPU_ENGINE_H

#include <iostream>
#include <set>
#include <vector>
#include <unistd.h>
#include "GPUProcessor.h"
#include "MemoryManager.h"
#include "../ResultListener.h"
#include "../../Common/Consts.h"
#include "../../Packets/RulePkt.h"
#include "../Engine.h"
using namespace std;

typedef struct GPUSharedStruct {
	pthread_cond_t *processCond;
	pthread_cond_t *resultCond;
	pthread_mutex_t *processMutex;
	pthread_mutex_t *resultMutex;
	int *stillProcessing;
	bool finish;
	vector<GPUProcessor *> *processors;
	set<PubPkt *> results;
	PubPkt *event;
	int lowerBound;
	int upperBound;
} GPUShared;

class GPUEngine: public Engine {

public:
	/**
	 * Constructor
	 */
	GPUEngine();

	/**
	 * Destructor
	 */
	~GPUEngine();

	/**
	 * Processes the given event and adds new derived events in generated events
	 */
	void processPubPkt(PubPkt *event);
	
	/**
	 * Processes the given rule and adds it to the set of processed rules
	 */
	void processRulePkt(RulePkt *rule);
	
	/**
	 * Adds a new result listener
	 */
	void addResultListener(ResultListener *resultListener) { resultListeners.insert(resultListener); }
	/**
	 *  Removes the given ResultHandler
	 */
	void removeResultListener(ResultListener *resultListener) { resultListeners.erase(resultListener); }

private:
  /**
	 * Processes the given publication pkt, but this is called from the engine itself when the pubPkt is needed by other rules
	 */
	void processPubPkt(PubPkt *pkt, bool recursion);
	
	/**
	 * This is called when adding a new rule, so that the recursion is enabled only if needed, i.e. if the output of some any rule is taken as input by some other rule
	 */
	void setRecursionNeeded(RulePkt *pkt);
	
	/**
	 * This is used to limit the recursion depth, mainly to avoid infinite loop in case of cycles in the events graph	
	 */
	int recursionDepth;
	bool recursionNeeded;
	
	/**
	 * Set of events used as input by all the rules installed
	 */
	std::set<int> inputEvents;
	
	/**
	 * Set of all the events possibly generated by all the rules installed
	 */
	std::set<int> outputEvents;
	
	set<PubPkt *> generatedEvents;
	MemoryManager *mm;
  	std::set<ResultListener *> resultListeners;	// Result listeners associated with the processing engine
	vector<GPUProcessor *> *processors;
};

#endif
