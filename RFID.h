#include <MFRC522.h>

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



typedef struct Tag {
    uint8_t* uid;
    uint8_t size;
} Tag;




typedef struct TagAction {
    Tag tag;
    void (*action)(void);
} TagAction;



//bool PCD_PerformSelfTest();


class RFID {

    public:

    bool isRunning = true;

    MFRC522 mfrc522;

    TagAction* TagActionList;

    uint8_t TagActionListSize;



    void setup (uint8_t SDA, uint8_t RST, TagAction* _TagActionList, uint8_t _TagActionListSize) {

        TagActionList = _TagActionList;
        TagActionListSize = _TagActionListSize;

        mfrc522.PCD_Init(SDA, RST);
        mfrc522.PCD_DumpVersionToSerial();
        //mfrc522.PCD_PerformSelfTest();
    };



    void run () {
        Tag tag;

        if ( isRunning && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ) {

            getReadedTag( &tag );

            Serial.println("Found new tag, searching for matches...");
            Serial.print("\tTag size: ");
            Serial.println(tag.size);
            Serial.print("\tTag uid: ");
            dump_byte_array(tag.uid, tag.size); Serial.println();
            Serial.println();

            for ( uint8_t i; i < TagActionListSize; i = i + 1 ) {

                Serial.print( "\tCompare to: " );
                dump_byte_array( TagActionList[i].tag.uid, TagActionList[i].tag.size ); Serial.println();
                Serial.print("\tResult: ");
                Serial.println(memcmp( tag.uid, TagActionList[i].tag.uid, TagActionList[i].tag.size ));

                if ( memcmp( tag.uid, TagActionList[i].tag.uid, TagActionList[i].tag.size ) == 0 ) {

                    Serial.println();
                    Serial.print("Match found! Call action ");
                    Serial.println(i);

                    TagActionList[i].action();

                }

                Serial.println();

            }

        }

    };



    void getReadedTag ( Tag* tag ) {
        //memcpy(tag->uid, mfrc522.uid.uidByte, 2);
        tag->uid = mfrc522.uid.uidByte;
        tag->size = mfrc522.uid.size;
    }



    void runningOff () {
        isRunning = false;
    };



    void runningOn () {
        isRunning = true;
    };



    void switchAntennaOff () {
        mfrc522.PCD_AntennaOff();
    };



    void switchAntennaOn () {
        mfrc522.PCD_AntennaOn();
    };



    void dump_byte_array(byte *buffer, byte bufferSize) {
        for (byte i = 0; i < bufferSize; i++) {
            Serial.print(buffer[i] < 0x10 ? " 0" : " ");
            Serial.print(buffer[i], HEX);
        }
    }


};


template < int RFIDS_COUNT >

class RFIDs_Manager {

    RFID rfids[RFIDS_COUNT];

    uint8_t rfids_count = RFIDS_COUNT;

    uint8_t* PINS_SDA;

    uint8_t PIN_RST;

    TagAction* TagActionList;

    uint8_t TagActionListSize;

    public:

        void setup(uint8_t _pins_sda[], uint8_t pin_rst, TagAction* _TagActionList, uint8_t _TagActionListSize) {

            PINS_SDA = _pins_sda;
            PIN_RST = pin_rst;
            TagActionList = _TagActionList;
            TagActionListSize = _TagActionListSize;

            Serial.print("Begin to setup ");
            Serial.print( getRFIDsCount() );
            Serial.println(" rfid(s)");
            for ( uint8_t i = 0; i < getRFIDsCount(); i = i + 1 ) {
                Serial.println(PIN_RST);
                Serial.println(PINS_SDA[i]);
                rfids[i].setup(PINS_SDA[i], PIN_RST, TagActionList, TagActionListSize);
                rfids[i].switchAntennaOff();
            }

        }

        void run () {

            for (uint8_t i = 0; i < getRFIDsCount(); i = i + 1) {
                rfids[i].switchAntennaOn();
                rfids[i].run();
                rfids[i].switchAntennaOff();
            }

        }

        uint8_t getRFIDsCount () {
            return rfids_count;
            //return sizeof PINS_SDA / sizeof (uint8_t);
        }
};
