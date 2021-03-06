#include <iostream>
#include <iomanip>
#include "ModuleV2718.h"

ModuleV2718::ModuleV2718(){
  fBaseAddr = -1;
  _vmeLink = 0;   // link 0 goes to ADC2 (BaseAddr 0x22220000)
  // link 1 goes to ADC3 (BaseAddr 0x33330000)
  _boardId = 0;
//  fNRead = 0;
}

ModuleV2718::~ModuleV2718(){

}

void ModuleV2718::Start(){
  fBaseAddr = _iniFile->Int( this->fName, "BaseAddress", -1 );
  _vmeLink = _iniFile->Int( this->fName, "VmeLink", -1 );
  _boardId = _iniFile->Int( this->fName, "BoardId", -1 );
  if( ( _boardId > -1 ) && ( _vmeLink > -1 ) ){
    CVErrorCodes ret = cvSuccess;
    std::cout << "Starting the (pulser) Module " << fName << std::endl 
        << "Type: V2718" << std::endl 
        << "Base Address: 0x" << std::setw( 8 ) << std::setfill( '0' )<< std::hex <<  this->fBaseAddr << std::dec << std::endl 
        << "VME Link: " << this->_vmeLink << std::endl
        << "Board Id: " << this->_boardId << std::endl << std::endl;
    if( ( ret = CAENVME_Init( cvV2718, this->_vmeLink, this->_boardId, &this->_handle ) ) != cvSuccess ){
      std::cerr <<" [ERR:V2718] No Caen VME controller found!\n " << std::endl << CAENVME_DecodeError( ret ) << std::endl;
      exit( -1 );
    } else { std::cout << "VME Interface initialized." << std::endl; }
  }
  fName = _iniFile->String( this->fName, "Name", this->fName );
//  this->SetPulserParameters( 0, 2, 1, 0, 0);
  this->SetPulserParameters( 1, 20, 5, 2, 0);
std::cout << "-------------------- " << std::endl;
std::cout << "Restarting BUNCH ID... " << std::endl;
  //this->StartPulser( 0 );
sleep(2);
  this->StartPulser( 1 );
  sleep(10);
  this->StopPulser( 1 );
std::cout << " Stop BUNCH ID... " << std::endl;
std::cout << "-------------------- " << std::endl;
}
	
int ModuleV2718::Initialize(){
  fBaseAddr = _iniFile->Int( this->fName, "BaseAddress", -1 );
  _vmeLink = _iniFile->Int( this->fName, "VmeLink", -1 );
  _boardId = _iniFile->Int( this->fName, "BoardId", -1 );
  if( ( _boardId > -1 ) && ( _vmeLink > -1 ) ){
    CVErrorCodes ret = cvSuccess;
    std::cout << "Initializing Module " << fName << std::endl 
        << "Type: V2718" << std::endl 
        << "Base Address: 0x" << std::setw( 8 ) << std::setfill( '0' )<< std::hex <<  this->fBaseAddr << std::dec << std::endl 
        << "VME Link: " << this->_vmeLink << std::endl
        << "Board Id: " << this->_boardId << std::endl << std::endl;
    if( ( ret = CAENVME_Init( cvV2718, this->_vmeLink, this->_boardId, &this->_handle ) ) != cvSuccess ){
      std::cerr <<" [ERR:V2718] No Caen VME controller found!\n " << std::endl << CAENVME_DecodeError( ret ) << std::endl;
      exit( -1 );
    } else { std::cout << "VME Interface initialized." << std::endl; }
    /*
    cout << "\n---------------------------------------------------------------\n"<< endl;
    cout << " -> Initialization of " << ADC_Name << endl;
    cout << " -> Looking for " << ADC_Name << " at 0x" << hex << BaseAddr << dec << " ..." << endl;
    
    fADC = new ADC_V1721_CAEN( this->fBaseAddr, this->fName, this->fVmeLink, this->fBoardId );
    //cout << "Software reset..." << endl;
    //fADC->SW_Reset(); //Perform Software Reset
   
    while( !fADC->Is_Board_Ready_For_Acquisition() ){
      sleep( 1 ); //sec
      cout << " Polling for ACQ ready  0x" << setw(2) << setfill( '0' ) << hex 
          << fADC->Get_ACQ_Status() << decCAENVME_WriteCycle( this->_handle, this->fBaseAddr + V1190_Module_Reset, (void*)&Data, cvA32_U_DATA, cvD16 ); << endl;
      ++counter;	
      if( counter > 1000 ){
        cout << "Could not reach ACQ ready in module " << this->fName << endl 
            << "ABORT!" << endl;
        exit( -1 );
      }
    }
    */
  }
  fName = _iniFile->String( this->fName, "Name", this->fName );
  this->Stop();
  this->SetPulserParameters( 0, 2, 1, 0, 0);
//this->SetPulserParameters( 1, 10, 5, 2, 1);
  this->StartPulser( 0 );
//this->StartPulser( 1 );
  return true;
}

BSCModule * ModuleV2718::Clone(){ 
  std::cout << "V2718" << std::endl; 
  return new ModuleV2718();
}

bool ModuleV2718::Read(){
  return true;
}

bool ModuleV2718::Write(){
  return true;
}

void ModuleV2718::Stop(){
  this->StopPulser( 0 );
  this->StopPulser( 1 );
}

void ModuleV2718::SetPulserParameters( unsigned char aPulser, unsigned char aPeriod, unsigned char aWidth, unsigned char aOutput, unsigned char NumberOfPulses ){
  //1.) Set pulser
  //unsigned char NumberOfPulses = 0; //Infinite
  this->_status = CAENVME_SetPulserConf( this->_handle, 
                                   CVPulserSelect( aPulser ), // 0 or 1 
                                   aPeriod,
                                   aWidth, 
                                   cvUnit25ns, // CVTimeUnits Unit, 
                                   NumberOfPulses,
                                   cvManualSW, // CVIOSources Start, 
                                   cvManualSW  // CVIOSources Reset
                                 );
  if( this->_status != cvSuccess ){
    std::cerr <<" [ERR:V2718] Cannot initialize the pulser.\n " << std::endl << CAENVME_DecodeError( this->_status ) << std::endl;
    //exit( -1 );
  }
  //2.) Set output
  this->_status = CAENVME_SetOutputConf( 
                                  this->_handle, 
                                  CVOutputSelect( aOutput ),//CVOutputSelect OutSel, 
                                  cvDirect,// Normal polarity //CVIOPolarity OutPol,
                                  cvActiveHigh,//CVLEDPolarity LEDPol, 
                                  cvMiscSignals 
                                  );
  if( _status != cvSuccess ){
    std::cerr <<" [ERR:V2718] Cannot initialize the output to use pulser.\n " << std::endl << CAENVME_DecodeError( this->_status ) << std::endl;
    //exit( -1 );
  }
}

void ModuleV2718::StartPulser( unsigned char aPulser ){
  this->_status = CAENVME_StartPulser( this->_handle, CVPulserSelect( aPulser ) );
}

void ModuleV2718::StopPulser( unsigned char aPulser ){
  this->_status = CAENVME_StopPulser( this->_handle, CVPulserSelect( aPulser ) );
}



unsigned char ModuleV2718::ReadC( unsigned long aAddress ){
  unsigned char Data;
  this->_status = CAENVME_ReadCycle( this->_handle, aAddress, (void*)&Data, cvA32_U_DATA, cvD8 );
	return Data;
}

unsigned short ModuleV2718::ReadW( unsigned long aAddress ){
	unsigned short Data;
	_status = CAENVME_ReadCycle( this->_handle, aAddress, (void*)&Data, cvA32_U_DATA, cvD16 );
	return Data;
}

unsigned long ModuleV2718::ReadL( unsigned long aAddress ){
	unsigned long Data;
	_status = CAENVME_ReadCycle( this->_handle, aAddress, (void*)&Data, cvA32_U_DATA, cvD32 );
	return Data;
}

void ModuleV2718::WriteC( unsigned long aAddress, unsigned char aData ){
	_status = CAENVME_WriteCycle( this->_handle, aAddress, (void*)&aData, cvA32_U_DATA, cvD8 );
}

void ModuleV2718::WriteW( unsigned long aAddress, unsigned short aData ){
	_status = CAENVME_WriteCycle( this->_handle, aAddress, (void*)&aData, cvA32_U_DATA, cvD16 );
}

void ModuleV2718::WriteL( unsigned long aAddress, unsigned long aData ){
	_status = CAENVME_WriteCycle( this->_handle, aAddress, (void*)&aData, cvA32_U_DATA, cvD32 );
}
