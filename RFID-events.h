/*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */



#include <EventManager.h>
#include <MFRC522.h>



class RFID {



    static int NEW_TAG_EVENT = 522;
    bool isListening = false;



    MFRC522 mfrc522;
    

    EventManager& eventManager;



    void init (int SDA, int RST, EventManager& _eventManager) {
        eventManager = _eventManager;
        mfrc522.PCD_Init(SDA, RST);
        mfrc522.PCD_DumpVersionToSerial();
    };



    void run () {
        if ( isListening && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ) {
            eventManager.queueEvent(RFID::NEW_TAG_EVENT, TAG_RFID(RFID); //this
        }

    };



    void listenOff () {
        isListening = false;
    };



    void listenOn () {
        isListening = true;
    };



    void switchAntennaOff () {
        mfrc522.PCD_AntennaOff();
    };



    void switchAntennaOn () {
        mfrc522.PCD_AntennaOn();
    };



}



class TAG_RFID {

    MFRC522& mfrc522;



    TAG_RFID (MFRC522& _mfrc522) {
        mfrc522 = *_mfrc522;
    }



    byte* getId () {
        return mfrc522.uid.uidByte;
    };



}



