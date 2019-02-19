// Copyright 2015 GameSparks Ltd 2015, Inc. All Rights Reserved.
#include <GameSparks/GS.h>
#include <GameSparks/GSRequest.h>
#include <cassert>
#include "GSClientConfig.h"

using namespace GameSparks;
using namespace GameSparks::Core;

// dummy default constructor for VC12 std::list - do not call, it will create an invalid object!
GSRequest::GSRequest()
	: m_GSInstance()
	, m_Durable(false)
    , m_userData()
{
	// this should never be called !
	assert(false);
}

GSRequest::GSRequest(GS& gsInstance, const gsstl::string& requestType)
	: m_GSInstance(&gsInstance)
	, m_Durable(false)
    , m_userData()
{
	AddString("@class", requestType);
}

GameSparks::Core::GSRequest::GSRequest(GS& gsInstance, cJSON* data)
	: GSObject(data)
	, m_GSInstance(&gsInstance)
    , m_Durable(false)
    , m_userData()
{

}

bool GameSparks::Core::GSRequest::operator==(const GSRequest& other) const
{
	return GetJSON() == other.GetJSON();
}


void GameSparks::Core::GSRequest::Send(const BaseCallbacksPtr& callbacks, Seconds timeoutSeconds)
{
	assert(callbacks);
	m_expiresInSeconds = timeoutSeconds > 0 ? timeoutSeconds : GSClientConfig::instance().getRequestTimeout();
	m_callbacks = callbacks;
	m_GSInstance->Send(*this);
	m_callbacks = 0; // release the callback, so that this GSRequest instance can be re-used - Note: a copy is inside m_GSInstance
}

void GameSparks::Core::GSRequest::Send(Seconds timeoutSeconds)
{
	m_expiresInSeconds = timeoutSeconds > 0 ? timeoutSeconds : GSClientConfig::instance().getRequestTimeout();
	m_GSInstance->Send(*this);
}

void GameSparks::Core::GSRequest::Complete(const GSObject& response)
{
	m_Response = response;

	if (m_callbacks)
	{
		if (response.ContainsKey("error"))
		{
			m_callbacks->OnError(*m_GSInstance, response);
		}
		else
		{
			m_callbacks->OnSucess(*m_GSInstance, response);
		}
	}
}