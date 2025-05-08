  #include "stdafx.h"
#include "AudioEngine.h"

AudioEngine * g_AEngine;

AudioEngine :: AudioEngine()
{
	placer = 0;
	Audio.clear();
	m_pListener =NULL;
	m_pPerformance = NULL;
	m_pLoader = NULL;
	m_Handle = hWnd;
	m_bgmPlaying = false;
}

bool AudioEngine :: InitDirectXAudio()
{
	CoInitialize(NULL); // create the loader object
	if(FAILED(CoCreateInstance(CLSID_DirectMusicLoader, NULL, CLSCTX_INPROC, IID_IDirectMusicLoader8, (void**)&g_AEngine->m_pLoader)))
		return false;
	if(FAILED(CoCreateInstance(CLSID_DirectMusicPerformance, NULL, CLSCTX_INPROC, IID_IDirectMusicPerformance8, (void**)&g_AEngine->m_pPerformance))) // create the performance object
		return false;
	if(FAILED(g_AEngine->m_pPerformance->InitAudio(NULL, NULL, NULL, DMUS_APATH_DYNAMIC_3D, 64, DMUS_AUDIOF_ALL, NULL))) // intialize DirectMusic and DirectSound
		return false;
	return true;
}

int AudioEngine :: LoadSegment(string filename, bool Background, int ID)
{
	if(ID < 0)
	{
		for(unsigned int index = 0; index < Audio.size(); index++)
			if(Audio[index].Path == filename)
				return index;
		DebugPrint("\nCreating Audio ");
		DebugPrint(filename.c_str());
		DebugPrint("\n");
		Sound * Create = new Sound;
		Create->Path = filename;
		Create->Loaded = false;
		Create->Bgm = Background;
		Create->id = placer++;
		Audio.insert(Create);
	}
	else
	{
		if((Background && m_bgmPlaying) || Audio[ID].Path == filename)
			return ID;
		DebugPrint("\nReplacing Audio ");
		DebugPrint(filename.c_str());

		Audio[ID].Path = filename;
		Audio[ID].Bgm = Background;
		if(Audio[ID].Loaded)
		{
			Audio[ID].Loaded = false;
			DebugPrint("\nReplacing Audio ");
			DebugPrint(filename.c_str());
		}
		return ID;
	}
	return (int(Audio.size()) - 1);
}

void AudioEngine :: BuildSegments()
{
	for(unsigned int index = 0; index < Audio.size(); index++)
	{
		if(!Audio[index].Loaded)
		{
			DebugPrint("\nBuilding Audio ");
			DebugPrint(Audio[index].Path.c_str());
			Audio[index].Loaded = true;
			WCHAR wcharStr[MAX_PATH];
    
			MultiByteToWideChar(CP_ACP, 0, Audio[index].Path.c_str(), -1, wcharStr, MAX_PATH); // convert filename to unicode string

			if(FAILED(m_pLoader->LoadObjectFromFile(CLSID_DirectMusicSegment, IID_IDirectMusicSegment8, wcharStr, (void**)&Audio[index].m_pSegment)))
				Audio[index].Loaded = false;

			Audio[index].m_pSegment->Download(m_pPerformance);

			if(!Audio[index].Bgm)
			{
				if(FAILED(m_pPerformance->CreateStandardAudioPath(DMUS_APATH_DYNAMIC_3D, 64, TRUE, &Audio[index].m_p3DAudioPath))) // create a standard 3D audiopath
					Audio[index].Loaded = false;
				if(FAILED(Audio[index].m_p3DAudioPath->GetObjectInPath( 0,DMUS_PATH_BUFFER,0,GUID_NULL,0, IID_IDirectSound3DBuffer,(void**)&Audio[index].m_pDirectSoundBuffer3D)))
					Audio[index].Loaded = false;
			}
			else
			{
				Audio[index].m_p3DAudioPath = NULL;
				Audio[index].m_pDirectSoundBuffer3D = NULL;
			}
			if(m_pListener != NULL)
				Audio[index].m_p3DAudioPath->GetObjectInPath
				( 
					0,									// Performance channel.
					DMUS_PATH_PRIMARY_BUFFER,			// Stage in the path.
					0,									// Index of buffer in chain.
					GUID_NULL,							// Class of object.
					0,									// Index of object in buffer; ignored.
					IID_IDirectSound3DListener,			// GUID of desired interface.
					(void**)&m_pListener				// Pointer that receives interface.
				);
		}
	}
}

void AudioEngine :: PlaySegment(unsigned int ID, int times)
{
	if(m_bgmPlaying && ID == BGM && Audio[ID].Loaded)
		return;
	for(unsigned int index = 0; index < Audio.size(); index++)
	{
		if(Audio[index].id == ID && Audio[index].Loaded)
		{
			if(ID == BGM)
			{
				m_bgmPlaying = true;
				DebugPrint("\nPlaying Audio ");
				DebugPrint(Audio[index].Path.c_str());
				DebugPrint("\n");
			}
			if(times > -1)
			{
				Audio[index].m_pSegment->SetRepeats(times);
				m_pPerformance->PlaySegmentEx(Audio[index].m_pSegment,NULL,NULL, DMUS_SEGF_DEFAULT, 0, NULL,NULL, Audio[index].m_p3DAudioPath);
			}
			else
			{
				if(m_pListener != NULL)
				{
					Vector * temp = NULL;
					if(GetListenerpos(temp))
						Audio[index].m_pDirectSoundBuffer3D->SetPosition(temp->x, temp->y, temp->z, DS3D_IMMEDIATE);
				}
				Audio[index].m_pSegment->SetRepeats(DMUS_SEG_REPEAT_INFINITE);
				m_pPerformance->PlaySegmentEx(Audio[index].m_pSegment,NULL,NULL, DMUS_SEGF_SECONDARY, 0, NULL,NULL, NULL);
			}
			return;
		}
	}
}

void AudioEngine :: StopSegment(unsigned int ID, bool stopall)
{
	if(ID == BGM || stopall)
		m_bgmPlaying = false;
	DebugPrint("\nStoping Sound ");
	DebugPrint(Audio[ID].Path.c_str());
	for(unsigned int index = 0; index < Audio.size(); index++)
	{
		if(!stopall)
			if(Audio[index].id == ID)
				m_pPerformance->StopEx(Audio[index].m_pSegment,NULL,NULL);
		else
			m_pPerformance->StopEx(Audio[index].m_pSegment,NULL,NULL);
	}
}

void AudioEngine :: Set3DSoundPos(unsigned int ID, Vector & SoundPos)
{
	for(unsigned int index = 0; index < Audio.size(); index++)
	{
		if(Audio[index].id == ID && Audio[index].Loaded)
			if(Audio[index].m_pDirectSoundBuffer3D != NULL)
				Audio[index].m_pDirectSoundBuffer3D->SetPosition(SoundPos.x, 0.0f, SoundPos.y, DS3D_IMMEDIATE);
	}
}

bool AudioEngine :: GetListenerpos(Vector * Pos)
{
	if(m_pListener != NULL)
	{
		D3DVECTOR vec;
		if(FAILED(m_pListener->GetPosition(&vec)))
			return false;
		Pos = new Vector(vec.x, vec.y, vec.z);
		return true;
	}
	return false;
}

void AudioEngine :: SetListenerPos(Vector & Pos)
{
	if(m_pListener != NULL)
		m_pListener->SetPosition(Pos.x, 0.0f, Pos.y, DS3D_IMMEDIATE);
}

void AudioEngine :: LoadDefaultTracks()
{
	SetListenerPos(Vector());
	LoadSegment("Audio/Eastern Night.wav", true);
	BuildSegments();
	Set3DSoundPos(BGM, Vector(0, 0, 0));

	LoadSegment("Audio/SFX/EnemyExplode01.wav", false);
	BuildSegments();
	Set3DSoundPos(EXPLODE, Vector());
	
	LoadSegment("Audio/SFX/Hit_1.wav", false);
	BuildSegments();
	Set3DSoundPos(HIT, Vector());

	LoadSegment("Audio/SFX/die.wav", false);
	BuildSegments();
	Set3DSoundPos(DIE, Vector());

	LoadSegment("Audio/SFX/Graze.wav", false);
	BuildSegments();
	Set3DSoundPos(GRAZE, Vector());

	LoadSegment("Audio/SFX/GetItem.wav", false);
	BuildSegments();
	Set3DSoundPos(GETITEM, Vector());
}

void AudioEngine :: Shutdown()
{	 
	StopSegment(0,true);
	Audio.clear();
	m_pPerformance->Release();
	m_pPerformance = NULL;

	m_pLoader->Release();
	m_pLoader = NULL;

	CoUninitialize();
}

bool AudioEngine :: BGMPlaying()
{
	return m_bgmPlaying;
}