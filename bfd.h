#ifndef BFD_ENDPOINT_H
#define BFD_ENDPOINT_H
#include <pthread.h>
#include <queue>

#define BFD_PL_SZ	6
#define ERROR		-1
#define OK		0

using namespace std;

enum BfdState_e {
	BFD_STAT_ADMIN_DOWN = 0,
	BFD_STAT_DOWN,
	BFD_STAT_INIT,
	BFD_STAT_UP,
	BFD_STAT_CNT };

typedef struct  {
	int data[BFD_PL_SZ];
}BfdDat;


class BfdEndpoint {
   public:
	int RxIntvlSet(int);
	int TxIntvlSet(int);
	int MultSet(int);
	int ActvSet(bool);
	enum BfdState_e StatusGet();
	void SessionActiveSet(bool);
	bool SessionActiveGet(void);
	BfdEndpoint();	
	~BfdEndpoint();	
	void BFDDatRxReg(BfdDat *pDat);		// function to provide BFD Rx data to the algorithm
	void BFDDatTxReg(void(*f)(BfdDat*)); // function to register the Tx func that is invoked to send BFD out
	void RxSM(void);
	void TxSM(void);
    private:
	//configuration params
	int rx;		// Rx interval [us]
	int tx; 	// Tx interval [us]
	int mult;	// multiplier
	bool active;	// active (1) or passive (0) endpoint
	bool sess_actv; // signals if session is active
	//internal vars
	enum BfdState_e status;
	pthread_t RxTh;
	pthread_t TxTh;
	queue<BfdDat*> RxQ;
	queue<BfdDat*> TxQ;
	unsigned long lastRx_us;
	unsigned long lastTx_us;
	void(*TxDat)(BfdDat*);
	//internal methods
	int Start();
	int BfdStateGet(BfdDat *pDat);
	int BfdStateSet(BfdState_e state,BfdDat* pDat);
	int RxTmstmpCheck(void);    
	int RxTmstmpUpdate(void);
        int TxTmstmpCheck(void);    
	int TxTmstmpUpdate(void);
        int TxQAdd(BfdDat *pDat);
};

void *RxSM_wrp(void *context);
void *TxSM_wrp(void *context); 
	

#endif
