#ifndef SSUBFILTER_HPP
#define SSUBFILTER_HPP

/****************************************************************
 * A SSubFilter is four OnePoleFilters in series, with the
 * capability of resonance with its output soft-clipped. Each
 * SSubVoice has a SSubFilter at it's final output stage.
****************************************************************/

#include "OnePoleFilter.hpp"

class SSubFilter : public IFilter
{
	public:
		SSubFilter();
		~SSubFilter() override;

		float processSample (float sample) override;
		void setCoefficients (float frequency) override;

		void setResonance (float resonance) override;
		float getResonance() override;

	private:
		OnePoleFilter filter1;
		OnePoleFilter filter2;
		OnePoleFilter filter3;
		OnePoleFilter filter4;
		float m_Resonance;
		float m_PrevSample;
};

#endif // SSUBFILTER_HPP
