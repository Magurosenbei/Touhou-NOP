#pragma once

/* Notes
Plays sounds / background music accordingly

etc..
*/
#include "Singleton.h"

#define BGM 0
#define EXPLODE 1
#define HIT 2
#define DIE 3
#define GRAZE 4
#define GETITEM 5

struct Sound
{
	string Path;
	IDirectMusicSegment8 *	m_pSegment;
	IDirectMusicAudioPath8 * m_p3DAudioPath;
	IDirectSound3DBuffer8 *	 m_pDirectSoundBuffer3D;
	unsigned int id;
	bool Loaded;
	bool Bgm;
	~Sound()
	{
		Path = "";
		Loaded = false;
		Bgm = false;
		if(m_pSegment != NULL)
			m_pSegment->Release();
		if(m_p3DAudioPath != NULL)
			m_p3DAudioPath->Release();
		if(m_pDirectSoundBuffer3D != NULL)
			m_pDirectSoundBuffer3D->Release();
		m_pSegment = NULL;
		m_p3DAudioPath = NULL;
		m_pDirectSoundBuffer3D = NULL;
	}
};

class AudioEngine : public Singleton <AudioEngine>
{
protected:
	IDirectMusicPerformance8* m_pPerformance;
	IDirectMusicLoader8* m_pLoader;
	IDirectSound3DListener8* m_pListener;

	IDirectMusicPerformance8* mt_pPerformance;
	IDirectMusicLoader8* mt_pLoader;
	IDirectSound3DListener8* mt_pListener;
	CPtrArray <Sound> Audio;

	unsigned int placer;
	HWND m_Handle;
	bool m_bgmPlaying;
public:
	AudioEngine();
	static bool InitDirectXAudio();	// Initializing the components for use
	int LoadSegment(string filename, bool Background, int ID = -1);	// Load WAV files for playing.	Gives out ID for Memo	
	void BuildSegments();
	void PlaySegment(unsigned int ID, int times);
	void StopSegment(unsigned int ID, bool stopall);
	void Set3DSoundPos(unsigned int ID, Vector & SoundPos);
	bool GetListenerpos(Vector * Pos);
	void SetListenerPos(Vector & Pos);
	void LoadDefaultTracks();
	void Shutdown();
	void SwitchBGMOff()
	{
		m_pPerformance->StopEx(Audio[BGM].m_pSegment,NULL,NULL);
	}
	void SwitchBGMOn()
	{
		m_pPerformance->PlaySegmentEx(Audio[BGM].m_pSegment,NULL,NULL, DMUS_SEGF_DEFAULT, 0, NULL,NULL, Audio[BGM].m_p3DAudioPath);
	}
	bool BGMPlaying();
};

extern AudioEngine * g_AEngine;