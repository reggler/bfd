#include <iostream>
#include "bfd.h"

using namespace std;
void TxBfdData(BfdDat *pDat);
void RxBfdData(BfdDat *pDat);

int main() {
    BfdDat Data;
	BfdEndpoint BfdEndpoint;
        BfdEndpoint.BFDDatTxReg(TxBfdData);
	cout << "Hello World!" << endl;
	cin.get();
        //When BFD frame is received
        BfdEndpoint.BFDDatRxReg(&Data);
	return 0;
}

void TxBfdData(BfdDat *pDat)
{
    cout << "in TxBfdData" << endl;
    return;
}
