#include <Windows.h>
#include <mmeapi.h>
#include <vector>
#include <string>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#pragma comment(lib, "winmm.lib")

using namespace std;
namespace py = pybind11;

py::tuple enumaudiodevice()
{
	py::list devices;
	WAVEOUTCAPS deviceDesc;
	int i = 0;
	while (waveOutGetDevCaps(i, &deviceDesc, sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR)
	{
		size_t sizewchar = (wcslen(deviceDesc.szPname) * 2) + 1;
		size_t bytesConverted = 0;
		char* nomeDevice = (char*)malloc(sizewchar);
		wcstombs_s(&bytesConverted, nomeDevice, sizewchar, deviceDesc.szPname, sizewchar);
		devices.append(nomeDevice);
		free(nomeDevice);
		i++;
	}
	return py::tuple(devices);

}

HWAVEOUT openDeviceOutput(int id, int frequenza = 44100, int nbit = 16, int ncanali = 1)
{
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nChannels = ncanali;
	waveFormat.nSamplesPerSec = frequenza;
	waveFormat.wBitsPerSample = nbit;
	waveFormat.nBlockAlign = (ncanali * nbit) / 8;
	waveFormat.nAvgBytesPerSec = waveFormat.nBlockAlign * frequenza;
	waveFormat.cbSize = 0;
	
	HWAVEOUT handleDevice;
	if (waveOutOpen(&handleDevice, id, &waveFormat, NULL, 0, CALLBACK_NULL) == S_OK) return handleDevice;
	else return 0;
}

PYBIND11_MODULE(soundWM, m)
{
	m.def("enumaudiodevices", &enumaudiodevice, "Mostra i dispositivi di output disponibili");
	m.def("openDeviceOutput", &openDeviceOutput, "Apre un dispositivo di output per l'audio");
}