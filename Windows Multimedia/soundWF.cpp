#include <iostream>
#include <Windows.h>
#include <mmeapi.h>

#pragma comment(lib, "winmm.lib")

#define FREQUENZA_DI_OUTPUT 44100
#define NUMERO_DI_CAMPIONI 20000
const int numeroDiBit = sizeof(short) * 8;
const int numeroMassimo = pow(2, numeroDiBit) / 2 - 100;
const double PI = 2.0 * acos(0.0);
using namespace std;

int main(int argc, char* argv[])
{
	//enumerazione dispositivi di output
	WAVEOUTCAPS deviceDesc;
	int i = 0;
	while (waveOutGetDevCaps(i, &deviceDesc, sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR)
	{
		wcout << deviceDesc.szPname << endl;
		i++;
	}
	cout << "\nDispositivi di output rilevati: " << i << endl;

	//apertura del dispositivo in uso, quindi con id 0
	HWAVEOUT handleDevice;

	//formato dell'onda sonora
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = 1;
	waveFormat.nSamplesPerSec = FREQUENZA_DI_OUTPUT;
	waveFormat.wBitsPerSample = numeroDiBit;
	waveFormat.nBlockAlign = waveFormat.nChannels * waveFormat.wBitsPerSample / 8;
	waveFormat.nAvgBytesPerSec = FREQUENZA_DI_OUTPUT * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	if (waveOutOpen(&handleDevice, 0, &waveFormat, NULL, 0, CALLBACK_NULL) == S_OK) //apertura device
	{
		while (1)
		{
			short* bufferAudio = new short[NUMERO_DI_CAMPIONI];
			const double tempoDiScansione = 1.0 / (double)FREQUENZA_DI_OUTPUT;
			double frequenzaIniziale = 70.0;

			for (int i = 0; i < NUMERO_DI_CAMPIONI; i++) //sintetizzo un'onda
			{
				*(bufferAudio + i) = numeroMassimo * sin(frequenzaIniziale * 2.0 * PI * tempoDiScansione * i);
				frequenzaIniziale += 0.01;
			}

			WAVEHDR waveHeader;
			waveHeader.dwBufferLength = NUMERO_DI_CAMPIONI * sizeof(short);
			waveHeader.lpData = (HPSTR)bufferAudio;
			waveHeader.dwLoops = 0;
			waveHeader.dwFlags = 0;

			waveOutPrepareHeader(handleDevice, &waveHeader, sizeof(WAVEHDR));
			waveOutWrite(handleDevice, &waveHeader, sizeof(WAVEHDR));
			Sleep(1000);
			waveOutUnprepareHeader(handleDevice, &waveHeader, sizeof(WAVEHDR));
			delete[] bufferAudio;
		}
	}
}