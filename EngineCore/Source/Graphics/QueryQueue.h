#ifndef _QUERYQUEUE_H_
#define _QUERYQUEUE_H_

#include "Renderer.h"

struct QueryQueueElement
{
    int elementID;
	QueryID queryID;
	unsigned long long *pResult;
	bool bActive;
};

//////////////////////////////////////////////////////////////
//
// QueryQueue Class
//
//////////////////////////////////////////////////////////////

class QueryQueue
{
	int m_NumFreeQueries;
	int m_MaxQueries;
	int m_NumQueries;

	std::vector<QueryQueueElement> m_vQueryList;
	std::list<QueryQueueElement*> m_lpActiveQueries;
	std::list<QueryQueueElement*> m_lpFreeQueries;

public:
	//Constructors
	QueryQueue() { m_MaxQueries = 10; m_NumQueries = 0; m_NumFreeQueries = 0; m_vQueryList.reserve(m_MaxQueries); }
	QueryQueue(int maxQueries) { m_MaxQueries = maxQueries; m_NumQueries = 0; m_NumFreeQueries = 0; m_vQueryList.reserve(m_MaxQueries); }
    ~QueryQueue();

	//Methods
	int getQuery(unsigned long long *pQueryResult);
	bool BeginQuery(int idQueueQueryElement);
	bool EndQuery(int idQueueQueryElement);
	void UpdateQueryQueue();
	void WaitQueries();

    bool remainActives() { return m_lpActiveQueries.size() > 0; }
};

#endif