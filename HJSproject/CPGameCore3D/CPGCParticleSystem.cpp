#include "CPGCParticleSystem.h"

CPGCParticleSystem::CPGCParticleSystem()
{
	m_pDevice = nullptr;
	m_pDContext = nullptr;
}

CPGCParticleSystem::~CPGCParticleSystem()
{
}

bool CPGCParticleSystem::create(std::wstring wszPSystemName, ID3D11Device* pDevice, ID3D11DeviceContext* pDContext)
{
	setName(wszPSystemName);
	m_pDevice = pDevice;
	m_pDContext = pDContext;

	return true;
}

bool CPGCParticleSystem::init()
{
	return true;
}

bool CPGCParticleSystem::update()
{
	float dt = CPGC_MAINTIMER.getOneFrameTimeF();

	m_fElapsedTime += dt;

	if (m_fElapsedTime < m_PSProp.fDuration + m_PSProp.fStDelay)
	{
		if (m_fElapsedTime >= m_PSProp.fStDelay)
		{
			for (auto it : m_pEmitterList)
			{
				it->update(dt * m_PSProp.fPlaySpeed);
			}
		}
	}
	else
	{
		if (m_PSProp.bInfinitePlay)
		{
			m_fElapsedTime = m_PSProp.fStDelay;
		}
		else
		{
			m_fElapsedTime = 0.0f;
			stateReset();
		}
	}

	return true;
}

bool CPGCParticleSystem::setMatrix(const CPM_Matrix* pWorld, const CPM_Matrix* pView, const CPM_Matrix* pProj, const CPM_Matrix* pParentWorld)
{
	if (m_fElapsedTime < m_PSProp.fDuration + m_PSProp.fStDelay)
	{
		if (m_fElapsedTime >= m_PSProp.fStDelay)
		{
			if (pWorld)			{ m_matLocalWorld = *pWorld; }
			if (pView)			{ m_matView = *pView; }
			if (pProj)			{ m_matProj = *pProj; }
			if (pParentWorld)	{ m_matParentWorld = *pParentWorld; }

			for (auto it : m_pEmitterList)
			{
				m_matWorld = m_matLocalWorld * m_matParentWorld;

				it->setMatrix(nullptr, pView, pProj, &m_matWorld);
			}
		}
	}

	return true;
}

bool CPGCParticleSystem::render()
{
	if (m_fElapsedTime < m_PSProp.fDuration + m_PSProp.fStDelay)
	{
		if (m_fElapsedTime >= m_PSProp.fStDelay)
		{
			for (auto it : m_pEmitterList)
			{
				it->render();
			}

		}
	}
	
	return true;
}

bool CPGCParticleSystem::release()
{
	for (auto it : m_pEmitterList)
	{
		it->release();
		delete it;
		it = nullptr;
	}

	m_pEmitterList.clear();

	m_pDevice = nullptr;
	m_pDContext = nullptr;

	return true;
}

void CPGCParticleSystem::stateReset()
{
	for (auto it : m_pEmitterList)
	{
		it->stateReset();
	}
}

CPRSEmitter* CPGCParticleSystem::getPtr(std::wstring wszEmitterName)
{
	for (auto it : m_pEmitterList)
	{
		if (wszEmitterName == it->getWszName()) { return it; }
	}

	return nullptr;
}
