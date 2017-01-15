#include <iostream>
#include <sys/time.h>
#include "bfd.h"
#define BFD_DEF_INTVL	3300
#define BFD_DEF_MULT	3
#define BFD_DEF_ACTV	1


using namespace std;

BfdEndpoint::BfdEndpoint(void)
{
//CONSTRUCTOR
	cout << "In constructor" << endl;
	//setting default values
	this->tx = BFD_DEF_INTVL;
	this->rx = BFD_DEF_INTVL;
	this->mult = BFD_DEF_MULT;
	this->active = BFD_DEF_ACTV;
	this->status = BFD_STAT_INIT; 
	this->sess_actv = true;

	this->Start();
}

BfdEndpoint::~BfdEndpoint(void)
{
//DESTRUCTOR
}

bool BfdEndpoint::SessionActiveGet(void)
{
return this->sess_actv;
}

int BfdEndpoint::Start(void)
{
	int rv = 0;
        cout << "Start Rx Thread!"<<endl;
	rv = pthread_create(&RxTh,NULL,&RxSM_wrp,this);
        cout << "STarted Rx"<<endl;
	rv = pthread_create(&TxTh,NULL,&TxSM_wrp,this);
        cout << "STarted Tx"<<endl;

}

void BfdEndpoint::TxSM(void)
{
 	while(sess_actv) {
  		if(TxTmstmpCheck()==OK &&
                  !TxQ.empty()) {
                this->TxDat(TxQ.front());
                TxQ.pop();
                }
 	}
return;	
}
int BfdEndpoint::TxQAdd(BfdDat *pDat)
{
//     TxQ.push(pDat);
    return OK;
}

void BfdEndpoint::RxSM(void)
{
 	BfdDat *Dat;
 	cout << "In RxSM" << endl;
 	cout.flush();
 	while(sess_actv) {
 		if(RxQ.empty() || !this->TxDat)
 			continue;
 		//inspect next BFD frame
 		if (RxTmstmpCheck()==OK)	
 			Dat = RxQ.front();
 		else
 			BfdStateSet(BFD_STAT_DOWN,Dat);
 		switch(BfdStateGet(Dat)) {
 			case BFD_STAT_ADMIN_DOWN:
 				break;
         		case BFD_STAT_DOWN:
                             TxQAdd(Dat);
 				break;
         		case BFD_STAT_INIT:
                             BfdStateSet(BFD_STAT_UP,Dat);
                             TxQAdd(Dat);
 				break;
         		case BFD_STAT_UP:
                             TxQAdd(Dat);
 				break;
 			default:
 				return;
 		}
 		RxQ.pop();
 	}
	return;
}

int BfdEndpoint::BfdStateSet(BfdState_e state,BfdDat* pDat)
{
  pDat->data[0] = BFD_STAT_DOWN<<20;
}

int BfdEndpoint::BfdStateGet(BfdDat *pDat)
{
 	char state = 0;
 	state = (pDat->data[0] & 0x00200000)<<2;
 	if (state < BFD_STAT_CNT)
 		return state;
 	else
	return ERROR;
}

int BfdEndpoint::RxTmstmpCheck(void)
{
	unsigned long lastTmstmp = this->lastRx_us;
	RxTmstmpUpdate();
	if (this->lastRx_us - lastTmstmp > rx)
		return ERROR;
	else
		return OK;
}

int BfdEndpoint::RxTmstmpUpdate(void)
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	this->lastRx_us = 1000000 * tv.tv_sec + tv.tv_usec;
	return OK;
}

int BfdEndpoint::TxTmstmpCheck(void)
{
	unsigned long lastTmstmp = this->lastTx_us;
	RxTmstmpUpdate();
	if (this->lastTx_us - lastTmstmp > (tx-tx*0.01)) 
		return ERROR;
	else
		return OK;
}

int BfdEndpoint::TxTmstmpUpdate(void)
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	this->lastTx_us = 1000000 * tv.tv_sec + tv.tv_usec;
	return OK;
}

void BfdEndpoint::BFDDatTxReg(void(*f)(BfdDat*))
{
this->TxDat = (*f);
}

void BfdEndpoint::BFDDatRxReg(BfdDat *pDat)
{
    if(!pDat)
        return
    RxQ.push(pDat);
}

void *RxSM_wrp(void *context)
{
	if (!context)
		return NULL;
        cout<<"invoking RxSM"<<endl;

	((BfdEndpoint*)context)->RxSM();

	return context;

}

void *TxSM_wrp(void *context)
{
	if (!context)
		return NULL;
        cout<<"invoking TxSM"<<endl;

	((BfdEndpoint*)context)->TxSM();

	return context;

}
