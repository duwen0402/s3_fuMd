// testTraderApi.cpp : 定义控制台应用程序的入口点。
//


#include ".\ThostTraderApi\ThostFtdcMdApi.h"
#include "MdSpi.h"

#include <cstdio>
#include <iomanip>
#include <iostream>

using namespace std;

// UserApi对象
CThostFtdcMdApi* pUserApi;

// 配置参数
//char FRONT_ADDR[] = "tcp://58.246.40.131:41213";	 	// 前置地址-华西
//TThostFtdcBrokerIDType	BROKER_ID = "8090";				// 经纪公司代码-华西
//TThostFtdcInvestorIDType INVESTOR_ID = "113693";	// 投资者代码
//TThostFtdcPasswordType  PASSWORD = "208508";			// 用户密码

//char FRONT_ADDR[] = "tcp://119.188.44.66:8803";		// 前置地址-徽商期货
//TThostFtdcBrokerIDType	BROKER_ID = "90001";             // 经纪公司代码-徽商期货
//TThostFtdcInvestorIDType INVESTOR_ID = "071966";	// 投资者代码-徽商期货
//TThostFtdcPasswordType  PASSWORD = "114163";			// 用户密码-徽商期货

char FRONT_ADDR[] = "tcp://180.168.146.187:10010";		// 前置地址-模拟
TThostFtdcBrokerIDType	BROKER_ID = "9999";             // 经纪公司代码-模拟
TThostFtdcInvestorIDType INVESTOR_ID = "071966";	// 投资者代码-模拟
TThostFtdcPasswordType  PASSWORD = "114163";			// 用户密码-模拟


char *ppInstrumentID[] = {"zn1806","","","","","","","","","","","","","","","","","","",""};			// 行情订阅列表		
int iInstrumentID =1;									// 行情订阅数量
string dataTable = "";

// 请求编号
int iRequestID = 0;

void main(void)
{
	struct CThostFtdcReqUserLoginField lf; 

		// 初始化UserApi
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi
	
	CThostFtdcMdSpi* pUserSpi = new CMdSpi();
	pUserApi->RegisterSpi(pUserSpi);						// 注册事件类
	pUserApi->RegisterFront(FRONT_ADDR);					// connect
	pUserApi->Init();
    pUserSpi->OnFrontConnected();
	pUserApi->Join();
//	pUserApi->Release();
}