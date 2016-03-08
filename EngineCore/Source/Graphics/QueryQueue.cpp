#include "..\Msvc\CthughaStd.h"
#include "QueryQueue.h"

QueryQueue::~QueryQueue() 
{
    m_vQueryList.clear();

    m_lpFreeQueries.clear();
    m_lpActiveQueries.clear();
}

int QueryQueue::getQuery(unsigned long long *pQueryResult)
{
	if(m_lpFreeQueries.size() > 0)
	{
        QueryQueueElement* query = m_lpFreeQueries.front();
        m_lpFreeQueries.pop_front();
        
		//query->queryID = g_pApp->m_pRenderer->createQuery();
        query->pResult = pQueryResult;
        query->bActive = true;

        m_lpActiveQueries.push_front(query);

        return query->elementID;
	}
	else
	{
        if(m_NumQueries < m_MaxQueries)
        {
            QueryQueueElement query;

            query.bActive = true;
            query.pResult = pQueryResult;
            query.elementID = m_vQueryList.size();
            query.queryID = g_pApp->m_pRenderer->createQuery();

            m_vQueryList.push_back(query);

            m_lpActiveQueries.push_front(&m_vQueryList.back());

            m_NumQueries++;

            return query.elementID;
        }
        else
        {
            bool freeFound = false;
            QueryQueueElement* query;

            while(!freeFound)
            {
                for(std::list<QueryQueueElement*>::iterator it = m_lpActiveQueries.end(); it != m_lpActiveQueries.begin(); --it)
                {
                    query = *it;

                    if(g_pApp->m_pRenderer->queryResultAvailable(query->queryID))
                    {
                        freeFound = true;
                        break;
                    }
                }
            }

            *query->pResult = g_pApp->m_pRenderer->getResultQuery(query->queryID);
            query->pResult = pQueryResult;

            return query->elementID;
        }
	}

    return -1;
}

bool QueryQueue::BeginQuery(int idQueueQueryElement)
{
    if(idQueueQueryElement != -1 && idQueueQueryElement < m_NumQueries)
    {
        g_pApp->m_pRenderer->beginQuery(m_vQueryList[idQueueQueryElement].queryID);

        return true;
    }

    return false;
}

bool QueryQueue::EndQuery(int idQueueQueryElement)
{    
	g_pApp->m_pRenderer->endQuery(idQueueQueryElement);

    return true;
}

void QueryQueue::UpdateQueryQueue()
{
    QueryQueueElement* query;
    std::list<QueryQueueElement*>::iterator it = m_lpActiveQueries.begin();

    while(it != m_lpActiveQueries.end())
    {
        query = *it;
        
        if(g_pApp->m_pRenderer->queryResultAvailable(query->queryID))
        {
            *query->pResult = g_pApp->m_pRenderer->getResultQuery(query->queryID);
            query->bActive = false;
            query->pResult = NULL;

            std::list<QueryQueueElement*>::iterator auxIt = it;
            ++it;

            m_lpActiveQueries.erase(auxIt);
            m_lpFreeQueries.push_back(query);

            m_NumFreeQueries++;
        }
        else
        {
            ++it;
        }
    }
}

void QueryQueue::WaitQueries()
{
	QueryQueueElement* query;
    std::list<QueryQueueElement*>::iterator it = m_lpActiveQueries.begin();

    while(it != m_lpActiveQueries.end())
    {
        query = *it;

		while (!g_pApp->m_pRenderer->queryResultAvailable(query->queryID));

		*query->pResult = g_pApp->m_pRenderer->getResultQuery(query->queryID);

        query->bActive = false;
        query->pResult = NULL;

        std::list<QueryQueueElement*>::iterator auxIt = it;
        ++it;

        m_lpActiveQueries.erase(auxIt);
        m_lpFreeQueries.push_back(query);

        m_NumFreeQueries++;
    }

	g_pApp->m_pRenderer->deleteQueries();
	m_lpFreeQueries.clear();
	m_vQueryList.clear();
	m_NumQueries = 0;
	m_NumFreeQueries = 0;
}