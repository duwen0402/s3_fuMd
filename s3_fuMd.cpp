// testTraderApi.cpp : �������̨Ӧ�ó������ڵ㡣
//


#include ".\ThostTraderApi\ThostFtdcMdApi.h"
#include "MdSpi.h"

#include <cstdio>
#include <iomanip>
#include <iostream>

using namespace std;

// UserApi����
CThostFtdcMdApi* pUserApi;

// ���ò���
//char FRONT_ADDR[] = "tcp://58.246.40.131:41213";	 	// ǰ�õ�ַ-����
//TThostFtdcBrokerIDType	BROKER_ID = "8090";				// ���͹�˾����-����
//TThostFtdcInvestorIDType INVESTOR_ID = "113693";	// Ͷ���ߴ���
//TThostFtdcPasswordType  PASSWORD = "208508";			// �û�����

//char FRONT_ADDR[] = "tcp://119.188.44.66:8803";		// ǰ�õ�ַ-�����ڻ�
//TThostFtdcBrokerIDType	BROKER_ID = "90001";             // ���͹�˾����-�����ڻ�
//TThostFtdcInvestorIDType INVESTOR_ID = "071966";	// Ͷ���ߴ���-�����ڻ�
//TThostFtdcPasswordType  PASSWORD = "114163";			// �û�����-�����ڻ�

char FRONT_ADDR[] = "tcp://180.168.146.187:10010";		// ǰ�õ�ַ-ģ��
TThostFtdcBrokerIDType	BROKER_ID = "9999";             // ���͹�˾����-ģ��
TThostFtdcInvestorIDType INVESTOR_ID = "071966";	// Ͷ���ߴ���-ģ��
TThostFtdcPasswordType  PASSWORD = "114163";			// �û�����-ģ��


char *ppInstrumentID[] = {"zn1806","","","","","","","","","","","","","","","","","","",""};			// ���鶩���б�		
int iInstrumentID =1;									// ���鶩������
string dataTable = "";

// ������
int iRequestID = 0;

void main(void)
{
	struct CThostFtdcReqUserLoginField lf; 

		// ��ʼ��UserApi
	pUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// ����UserApi
	
	CThostFtdcMdSpi* pUserSpi = new CMdSpi();
	pUserApi->RegisterSpi(pUserSpi);						// ע���¼���
	pUserApi->RegisterFront(FRONT_ADDR);					// connect
	pUserApi->Init();
    pUserSpi->OnFrontConnected();
	pUserApi->Join();
//	pUserApi->Release();
}