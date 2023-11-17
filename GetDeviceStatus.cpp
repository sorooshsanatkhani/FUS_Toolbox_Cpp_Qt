// Author: Soroosh Sanatkhani
// Columbia University
// Created: 1 August, 2023
// Last Modified : 26 October, 2023

#include "stdafx.h"
#include "WaveformGenerator.h"
#include "visa.h"
#pragma comment(lib,"visa64.lib")
#include <stdio.h>
#include <tchar.h>
#include <windows.h>


	std::pair<std::wstring, int> WaveformGenerator::GetDeviceStatus()
	{
		ViSession defaultRM;
		ViSession instr;
		ViUInt32 numInstrs;
		ViUInt32 retCount;
		ViFindList findList;
		ViStatus status;
		int status_int = 0;
		char instrResourceString[VI_FIND_BUFLEN];
		unsigned char buffer[100];
		std::wstring device_status;
		/** First we must call viOpenDefaultRM to get the manager
		* handle. We will store this handle in defaultRM.*/
		status = viOpenDefaultRM(&defaultRM);
		if (status < VI_SUCCESS)
		{
			status_int = 0;
			device_status = L"Resource Manages Failed!";
			return std::make_pair(device_status, status_int);
		}
		/* Find all the USB TMC VISA resources in our system and store the number of resources in the system in
		numInstrs. */
		status = viFindRsrc(defaultRM, "USB?*INSTR", &findList, &numInstrs, instrResourceString);
		if (status < VI_SUCCESS)
		{
			//printf("An error occurred while finding resources.\nPress 'Enter' to continue.");
			fflush(stdin);
			getchar();
			viClose(defaultRM);
			status_int = 0;
			device_status = L"An error occurred while finding resources.\nPress 'Okay' to continue.";
			return std::make_pair(device_status, status_int);
			//return status;
		}
		status = viOpen(defaultRM, instrResourceString, VI_NULL, VI_NULL, &instr);
		if (status < VI_SUCCESS)
		{
			//printf("Cannot open a session to the device.\n");
			status_int = 0;
			device_status = L"Cannot open a session to the device.";
			return std::make_pair(device_status, status_int);
		}
		/* * At this point we now have a session open to the USB TMC instrument.
		* We will now use the viPrintf function to send the device the string "*IDN?\n",
		* asking for the device's identification. */
		const char* cmmand = "*IDN?\n";
		status = viPrintf(instr, cmmand);
		if (status < VI_SUCCESS)
		{
			//printf("Error writing to the device.\n");
			device_status = L"Error writing to the device.";
			status = viClose(instr);
			status_int = 0;
			return std::make_pair(device_status, status_int);
		}
		/** Now we will attempt to read back a response from the device to
		* the identification query that was sent. We will use the viScanf
		* function to acquire the data.
		* After the data has been read the response is displayed.*/
		status = viScanf(instr, "%t%n", buffer, &retCount);
		if (status < VI_SUCCESS)
		{
			viClose(defaultRM);
			status_int = 0;
			device_status = L"Error reading a response from the device.";
			return std::make_pair(device_status, status_int);
		}
		else
		{
			viClose(defaultRM);
			int wlen = MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<char*>(buffer), retCount, nullptr, 0);
			device_status.resize(wlen, L'\0');
			MultiByteToWideChar(CP_ACP, 0, reinterpret_cast<char*>(buffer), retCount, &device_status[0], wlen);
			status_int = 1;
			return std::make_pair(device_status, status_int);
		}


		//std::wstring device_status(reinterpret_cast<wchar_t*>(buffer), retCount / sizeof(wchar_t));
		

		//device_status = FUS::removeEnd(device_status); // Call the removeEnd() function with the FUS:: namespace qualifier
	}