#ifndef SSUBVOICEMANAGER_HPP
#define SSUBVOICEMANAGER_HPP

/****************************************************************************
 * The SSubVoiceManager is responsible for processing MIDI messages and
 * setting the correct values for each SSubVoice. This class will take
 * input from the MidiHandler and other peripheral handlers.
****************************************************************************/

#include "SSubVoice.hpp"

#ifdef TARGET_BUILD
#define SSUB_NUM_VOICES 1
#else
#define SSUB_NUM_VOICES 6
#endif

class SSubVoiceManager : public IBufferCallback, public IKeyEventListener
{
	public:
		SSubVoiceManager();
		~SSubVoiceManager() override;

		void onKeyEvent (const KeyEvent& keyEvent) override;

		void call (float* writeBuffer) override;

		void setMonophonic (bool monophonic);

		// TODO very temp stuff, fix this with IPotEventListener, IButtonEventListener, ect
		void setCutoffFreq (float freq);
		void setResonance (float res);

	private:
		SSubVoice 	m_Voices[SSUB_NUM_VOICES];
		bool 		m_Monophonic;

		KeyEvent 	m_ActiveKeyEvents[SSUB_NUM_VOICES];
		unsigned int 	m_ActiveKeyEventIndex;
};

#endif // SSUBVOICEMANAGER_HPP
