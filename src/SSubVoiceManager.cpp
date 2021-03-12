#include "SSubVoiceManager.hpp"

#include <cstdlib>
#include <string.h>

#define SSUB_INIT(voiceNum) SSubVoice()

#if SSUB_NUM_VOICES == 1
#define SSUB_INIT_ALL_VOICES SSUB_INIT(1)
#elif SSUB_NUM_VOICES == 2
#define SSUB_INIT_ALL_VOICES SSUB_INIT(1), SSUB_INIT(2)
#elif SSUB_NUM_VOICES == 3
#define SSUB_INIT_ALL_VOICES SSUB_INIT(1), SSUB_INIT(2), SSUB_INIT(3)
#elif SSUB_NUM_VOICES == 4
#define SSUB_INIT_ALL_VOICES SSUB_INIT(1), SSUB_INIT(2), SSUB_INIT(3), SSUB_INIT(4)
#elif SSUB_NUM_VOICES == 5
#define SSUB_INIT_ALL_VOICES SSUB_INIT(1), SSUB_INIT(2), SSUB_INIT(3), SSUB_INIT(4), SSUB_INIT(5)
#elif SSUB_NUM_VOICES == 6
#define SSUB_INIT_ALL_VOICES SSUB_INIT(1), SSUB_INIT(2), SSUB_INIT(3), SSUB_INIT(4), SSUB_INIT(5), SSUB_INIT(6)
#endif

SSubVoiceManager::SSubVoiceManager() :
	m_Voices{ SSUB_INIT_ALL_VOICES },
	m_Monophonic( false ),
	m_ActiveKeyEventIndex( 0 )
{
}

SSubVoiceManager::~SSubVoiceManager()
{
}

void SSubVoiceManager::onKeyEvent (const KeyEvent& keyEvent)
{
	if ( ! m_Monophonic ) // polyphonic implementation
	{
		if ( keyEvent.pressed() == KeyPressedEnum::PRESSED )
		{
			bool containsKeyEvent = false;
			for (unsigned int voice = 0; voice < SSUB_NUM_VOICES; voice++)
			{
				if ( m_ActiveKeyEvents[voice].isNoteAndType( keyEvent ) )
				{
					containsKeyEvent = true;
					m_ActiveKeyEvents[voice] = keyEvent;
					m_Voices[voice].onKeyEvent(keyEvent);

					return;
				}
			}

			if ( ! containsKeyEvent )
			{
				// ensure we aren't overwriting a pressed key
				unsigned int initialActiveKeyEventIndex = m_ActiveKeyEventIndex;
				while ( m_ActiveKeyEvents[m_ActiveKeyEventIndex].pressed() == KeyPressedEnum::PRESSED )
				{
					m_ActiveKeyEventIndex = (m_ActiveKeyEventIndex + 1) % SSUB_NUM_VOICES;

					if ( m_ActiveKeyEventIndex == initialActiveKeyEventIndex )
					{
						break;
					}
				}
				m_ActiveKeyEvents[m_ActiveKeyEventIndex] = keyEvent;
				m_Voices[m_ActiveKeyEventIndex].onKeyEvent(keyEvent);

				m_ActiveKeyEventIndex = (m_ActiveKeyEventIndex + 1) % SSUB_NUM_VOICES;

				return;
			}
		}
		else if ( keyEvent.pressed() == KeyPressedEnum::RELEASED )
		{
			for (unsigned int voice = 0; voice < SSUB_NUM_VOICES; voice++)
			{
				if ( m_ActiveKeyEvents[voice].isNoteAndType( keyEvent, KeyPressedEnum::PRESSED ) )
				{
					m_ActiveKeyEvents[voice] = keyEvent;
					m_Voices[voice].onKeyEvent( keyEvent );

					return;
				}
			}
		}
	}
	else // monophonic implementation
	{
		if ( keyEvent.pressed() == KeyPressedEnum::PRESSED )
		{
			// if a key is currently playing
			KeyPressedEnum activeKeyPressed = m_ActiveKeyEvents[0].pressed();
			if ( activeKeyPressed == KeyPressedEnum::PRESSED || activeKeyPressed == KeyPressedEnum::HELD )
			{
				// build a 'held' key event, since we don't want to retrigger the envelope generator
				KeyEvent newKeyEvent( KeyPressedEnum::HELD, keyEvent.note(), keyEvent.velocity() );

				if ( m_ActiveKeyEvents[0].note() < newKeyEvent.note() )
				{
					KeyEvent oldKeyEvent( KeyPressedEnum::HELD, m_ActiveKeyEvents[0].note(), m_ActiveKeyEvents[0].velocity() );

					// look for a place to store the old key event, since we only want to play the highest note
					for (unsigned int voice = 1; voice < SSUB_NUM_VOICES; voice++)
					{
						if ( m_ActiveKeyEvents[voice].pressed() == KeyPressedEnum::RELEASED )
						{
							m_ActiveKeyEvents[voice] = oldKeyEvent;
							break;
						}
					}

					m_ActiveKeyEvents[0] = newKeyEvent;
					m_Voices[0].onKeyEvent( newKeyEvent );

					return;
				}
				else if ( m_ActiveKeyEvents[0].note() > newKeyEvent.note() )
				{
					// look for a place to store this key event, since we only want to play the highest note
					for (unsigned int voice = 1; voice < SSUB_NUM_VOICES; voice++)
					{
						if ( m_ActiveKeyEvents[voice].pressed() == KeyPressedEnum::RELEASED )
						{
							m_ActiveKeyEvents[voice] = newKeyEvent;
							break;
						}
					}

					return;
				}
			}
			else // there is no note currently active
			{
				m_ActiveKeyEvents[0] = keyEvent;
				m_Voices[0].onKeyEvent( keyEvent );

				return;
			}
		}
		else if ( keyEvent.pressed() == KeyPressedEnum::RELEASED )
		{
			// look for this note in the active key events array
			for (unsigned int voice = 0; voice < SSUB_NUM_VOICES; voice++)
			{
				// if there is a note that matches and isn't released
				KeyPressedEnum voiceKeyPressed = m_ActiveKeyEvents[voice].pressed();
				unsigned int voiceKeyNote = m_ActiveKeyEvents[voice].note();
				if ( voiceKeyPressed != KeyPressedEnum::RELEASED && voiceKeyNote == keyEvent.note() )
				{
					// if the main active voice is released, replace with lower note
					if (voice == 0)
					{
						int highestNote = -1; // negative 1 means no highest note found
						for (unsigned int voice2 = 1; voice2 < SSUB_NUM_VOICES; voice2++)
						{
							KeyPressedEnum voice2KeyPressed = m_ActiveKeyEvents[voice2].pressed();
							int voice2KeyNote = m_ActiveKeyEvents[voice2].note();
							if ( voice2KeyPressed == KeyPressedEnum::HELD && voice2KeyNote > highestNote )
							{
								highestNote = voice2;
							}
						}

						if ( highestNote > 0 ) // if an active lower key is found
						{
							// store the lower key
							KeyEvent newActiveKeyEvent = m_ActiveKeyEvents[highestNote];

							// replace the lower key with a released key event
							unsigned int keyNote = m_ActiveKeyEvents[highestNote].note();
							unsigned int keyVelocity = m_ActiveKeyEvents[highestNote].velocity();
							KeyEvent inactiveKeyEvent( KeyPressedEnum::RELEASED, keyNote, keyVelocity );
							m_ActiveKeyEvents[highestNote] = inactiveKeyEvent;

							// replace the currently active note with the lower key
							m_ActiveKeyEvents[0] = newActiveKeyEvent;
							m_Voices[0].onKeyEvent( newActiveKeyEvent );

							return;
						}
						else // if there are no active lower keys
						{
							m_ActiveKeyEvents[0] = keyEvent;
							m_Voices[0].onKeyEvent( keyEvent );

							return;
						}
					}
					else // if one of the lower notes is released, replace with released key event
					{
						m_ActiveKeyEvents[voice] = keyEvent;

						return;
					}
				}
			}
		}
	}
}

void SSubVoiceManager::call (float* writeBuffer)
{
	// first clear write buffer
	memset( writeBuffer, 0, sizeof(float) * ABUFFER_SIZE );

	// write the contents of each voice to the audio buffer
	for ( unsigned int voice = 0; voice < SSUB_NUM_VOICES; voice++ )
	{
		m_Voices[voice].call( writeBuffer );
	}

#if SSUB_NUM_VOICES > 1
	for ( unsigned int sample = 0; sample < ABUFFER_SIZE; sample++ )
	{
		writeBuffer[sample] = ( writeBuffer[sample] * (1.0f / SSUB_NUM_VOICES) );
	}
#endif
}

void SSubVoiceManager::setMonophonic (bool monophonic)
{
	m_Monophonic = monophonic;
}

// TODO very temp stuff, fix this with IPotEventListener, IButtonEventListener, ect
void SSubVoiceManager::setCutoffFreq (float freq)
{
	for ( unsigned int voice = 0; voice < SSUB_NUM_VOICES; voice++ )
	{
		m_Voices[voice].setCutoffFreq( freq );
	}
}
void SSubVoiceManager::setResonance (float res)
{
	for ( unsigned int voice = 0; voice < SSUB_NUM_VOICES; voice++ )
	{
		m_Voices[voice].setResonance( res );
	}
}
