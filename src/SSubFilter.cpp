#include "SSubFilter.hpp"

SSubFilter::SSubFilter() :
	filter1(),
	filter2(),
	filter3(),
	filter4(),
	m_Resonance(0.0f),
	m_PrevSample(0.0f)
{
	this->setCoefficients(20000.0f);
}

SSubFilter::~SSubFilter()
{
}

float SSubFilter::processSample (float sample)
{
	m_PrevSample = (m_PrevSample * -m_Resonance) + sample;
	float out1 = filter1.processSample(m_PrevSample);
	float out2 = filter2.processSample(out1);
	float out3 = filter3.processSample(out2);
	float out4 = filter4.processSample(out3);
	m_PrevSample = out4;

	// soft clipping
	if (m_PrevSample > 1.0f)
	{
		m_PrevSample = 1.0f;
	}
	if (m_PrevSample < -1.0f)
	{
		m_PrevSample = -1.0f;
	}
	m_PrevSample = (1.5f * m_PrevSample) - (0.5f * m_PrevSample * m_PrevSample * m_PrevSample);

	return m_PrevSample;
}

void SSubFilter::setCoefficients (float frequency)
{
	filter1.setCoefficients(frequency);
	filter2.setCoefficients(frequency);
	filter3.setCoefficients(frequency);
	filter4.setCoefficients(frequency);
}

void SSubFilter::setResonance (float resonance)
{
	m_Resonance = resonance;
}

float SSubFilter::getResonance()
{
	return m_Resonance;
}
