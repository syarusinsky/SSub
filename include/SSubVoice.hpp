#ifndef SSUBVOICE_HPP
#define SSUBVOICE_HPP

/*******************************************************************************
 * A single voice for the SSub synthesizer, a simple subtractive synthesizer.
*******************************************************************************/

#include "IKeyEventListener.hpp"
#include "IBufferCallback.hpp"
#include "AudioConstants.hpp"
#include "PolyBLEPOsc.hpp"
#include "ADSREnvelopeGenerator.hpp"
#include "LinearResponse.hpp"
#include "SSubFilter.hpp"

class SSubVoice : public IKeyEventListener, public IBufferCallback
{
	public:
		SSubVoice();
		~SSubVoice();

		void onKeyEvent (const KeyEvent& keyEvent) override;

		void call (float* writeBuffer) override;

		// TODO very temp stuff, fix this with IPotEventListener, IButtonEventListener, ect
		void setCutoffFreq (float freq);
		void setResonance (float res);

	private:
		PolyBLEPOsc 		m_Osc;
		LinearResponse 		m_LinearResponse;
		ADSREnvelopeGenerator 	m_AmpEnv;
		ADSREnvelopeGenerator 	m_FltEnv;
		SSubFilter 		m_Filter;
		float 			m_FilterFreq;
};

#endif // SSUBVOICE_HPP
