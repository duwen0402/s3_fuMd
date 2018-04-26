

#include "MdSpi.h"
#include <iostream>
#include <cstdio>
#include <iomanip>
#include <string>
#include <sstream> 
#include <fstream>
#include <vector>
#include <Windows.h>  
#include <algorithm>  
#include <time.h>  
#include <thread>  
#include <mutex>  
#include <deque>   
#include <condition_variable>  
#include <stdio.h> 
#include <conio.h> 
#include <tchar.h> 

using namespace std;

DWORD WINAPI Thread1(LPVOID lpParmeter);
void   CALLBACK   TimerProc(HWND   hWnd, UINT   nMsg, UINT   nTimerid, DWORD   dwTime);
void CalKline();
string doubleToString(double num);
#pragma warning(disable : 4996)
#define NUM_THREADS 5 //线程数  
// USER_API参数
extern CThostFtdcMdApi* pUserApi;

// 配置参数
extern char FRONT_ADDR[];		
extern TThostFtdcBrokerIDType	BROKER_ID;
extern TThostFtdcInvestorIDType INVESTOR_ID;
extern TThostFtdcPasswordType	PASSWORD;
extern char* ppInstrumentID[];	
extern int iInstrumentID;
extern string dataTable; 


bool startBar;
int upsecond1=0;
int upsecond2=0;
int upsecond3=0;
int upsecond4=0;

std::deque<int> m_data;
#define BUF_SIZE 10000000 
TCHAR szName[] = TEXT("Local\\MarketDatas");
TCHAR szMsg[] = TEXT("Duwen from first process.");
char tickDatas[BUF_SIZE];
string lasttickdatas;
// 请求编号
extern int iRequestID;
double lastHolds[4];
int totalMemNum = 0;
void sharedata()
{
	HANDLE hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // 物理文件句柄  
		NULL,                    // 默认安全级别  
		PAGE_READWRITE,          // 可读可写  
		0,                       // 高位文件大小  
		BUF_SIZE,                // 地位文件大小  
		szName                   // 共享内存名称  
		);
	char *pBuf = (char *)MapViewOfFile(
		hMapFile,            // 共享内存的句柄  
		FILE_MAP_ALL_ACCESS, // 可读写许可  
		0,
		0,
		BUF_SIZE
		);
	while (1)
	{
		strncpy(pBuf, tickDatas, BUF_SIZE - 1);
		pBuf[BUF_SIZE - 1] = '\0';
	}
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}


void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
}
		
void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	
	cerr << "--->>> " << "OnFrontConnected" << endl;
	///用户登录请求
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(req));
	strcpy(req.BrokerID, BROKER_ID);
	strcpy(req.UserID, INVESTOR_ID);
	strcpy(req.Password, PASSWORD);
	int iResult = pUserApi->ReqUserLogin(&req, ++iRequestID);

	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pUserApi->GetTradingDay() << endl;
	
		// 请求订阅行情
		SubscribeMarketData();	
	}
}

void CMdSpi::SubscribeMarketData()
{	
	HANDLE hThread1 = INVALID_HANDLE_VALUE;
	hThread1 = CreateThread(NULL, 0, Thread1, NULL, 0, NULL);
	int iResult = pUserApi->SubscribeMarketData(ppInstrumentID, iInstrumentID);
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

DWORD WINAPI Thread1(LPVOID lpParmeter)
{
	std::cout << "new thread" << std::endl;
	sharedata();
	return 0;
}


string doubleToString(double num)
{
	char str[256];
	sprintf(str, "%lf", num);
	string result = str;
	return result;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspSubMarketData" << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspUnSubMarketData" << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
	string instrumentid = pDepthMarketData->InstrumentID;
	string UpdateTime = "";
	string tickdata = "";
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	double tickholds[4];
	double lastvolume;
	double lastopeninterest;
	double tickvolume;
	double interestdiff;
	double opennum;
	double closenum;
	string status;

	GetLocalTime(&wtm);

	tickholds[0] = pDepthMarketData->Volume;
	tickholds[1] = pDepthMarketData->OpenInterest;
	tickholds[2] = pDepthMarketData->Turnover;

	lastvolume = lastHolds[0];
	lastopeninterest = lastHolds[1];
	tickvolume = tickholds[0] - lastvolume;
	interestdiff = tickholds[1] - lastopeninterest;
	opennum = (tickvolume + interestdiff) / 2;
	closenum = (tickvolume - interestdiff) / 2;

	printf("%s,%d,%d,%d,%d,%d\n", pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec, wtm.wHour, wtm.wMinute, wtm.wSecond, wtm.wMilliseconds);

	UpdateTime = pDepthMarketData->UpdateTime;
	//if (totalMemNum > 10)
	//{
	//	lasttickdatas = "";
	//	totalMemNum = 0;
	//}
	tickdata = lasttickdatas +"|0,0,0," 
		+ doubleToString(pDepthMarketData->LastPrice) + ','  + doubleToString(pDepthMarketData->OpenInterest) + ',' + doubleToString(interestdiff) + ','  
		+ doubleToString(pDepthMarketData->Turnover) + ',' + doubleToString(pDepthMarketData->Volume) + ',' + doubleToString(opennum) + ',' + doubleToString(closenum)+ ",0,0,"
		+ doubleToString(pDepthMarketData->BidPrice1) + ',' + doubleToString(pDepthMarketData->AskPrice1) + ',' + doubleToString(pDepthMarketData->BidVolume1) + ',' + doubleToString(pDepthMarketData->AskVolume1) ;
	lasttickdatas = tickdata;
	strncpy(tickDatas, tickdata.c_str(), BUF_SIZE - 1);
	for (int i = 0; i < 4; i++)
	{
		lastHolds[i] = tickholds[i];
	}
	totalMemNum = totalMemNum + 1;

}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}


