#include <Arduino.h>
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

/*
TagAction tagActionList[] = {
    { .tag = { .uid = TAG_UID_1, .size = sizeof TAG_UID_1 }, .action = f },
    { .tag = { .uid = TAG_UID_2, .size = sizeof TAG_UID_1 }, .action = g },
    { .tag = { .uid = TAG_UID_3, .size = sizeof TAG_UID_3 }, .action = h },
};
*/


//bool PCD_PerformSelfTest();


class RFID {

    public:

    bool isRunning = true;

    MFRC522 mfrc522;

    TagAction* TagActionList = NULL;

    uint8_t TagActionListSize;

    void (*onNewCardHandler)(void) = NULL;

    uint8_t PIN_SDA;

    uint8_t PIN_RST;



    void setup (uint8_t _pin_sda, uint8_t _pin_rst) {

        PIN_SDA = _pin_sda;
        PIN_RST = _pin_rst;

        mfrc522.PCD_Init(PIN_SDA, PIN_RST);
        pre(); mfrc522.PCD_DumpVersionToSerial();
        //mfrc522.PCD_PerformSelfTest();
    };



    bool readTag ( Tag buffer ) {

        clearTagUid( buffer );
        long unsigned int timer = millis();

        // unlock when there is a Card or timeout
        while ( millis() - timer < 1000
                && (!isRunning || !mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial() ) ) {}

        //timeout
        if ( !( millis() - timer < 1000 ) ) {
            return false;
        }
        else {
            uint16_t size = min( buffer.size, mfrc522.uid.size );
            memcpy(buffer.uid, mfrc522.uid.uidByte, size);

            return true;
        }

    }


    void clearTagUid ( Tag buffer ) {

        for ( uint8_t i; i < buffer.size; i = i + 1 ) {
            buffer.uid[i] = 0x00;
        }

    }



    void run () {
        Tag tag;

        if ( isRunning && mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial() ) {

            getReadedTag( &tag );

            pre(); Serial.println("found new tag!");
            pre(); Serial.print("\tTag size: ");
                   Serial.println(tag.size);
            pre(); Serial.print("\tTag uid: ");
                   dump_byte_array(tag.uid, tag.size);
                   Serial.println();

            if (onNewCardHandler) {
                pre(); Serial.println("Call onNewCard()");

                Serial.println();
                onNewCardHandler();
                Serial.println();
            }

            if ( TagActionList ) {

                pre(); Serial.println("searching for matches...");

                for ( uint8_t i; i < TagActionListSize; i = i + 1 ) {

                    pre(); Serial.print( "\tCompare to: " );
                    dump_byte_array( TagActionList[i].tag.uid, TagActionList[i].tag.size );
                    Serial.println();
                    pre(); Serial.print("\tResult: ");
                    Serial.println(memcmp( tag.uid, TagActionList[i].tag.uid, TagActionList[i].tag.size ));

                    if ( memcmp( tag.uid, TagActionList[i].tag.uid, TagActionList[i].tag.size ) == 0 ) {

                        //Serial.println();
                        pre(); Serial.print("Match found! Call action ");
                        Serial.println(i);

                        Serial.println();
                        TagActionList[i].action();
                        Serial.println();

                    }

                }

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



    static void dump_byte_array(byte *buffer, byte bufferSize) {
        for (byte i = 0; i < bufferSize; i++) {
            Serial.print(buffer[i] < 0x10 ? " 0" : " ");
            Serial.print(buffer[i], HEX);
        }
    }



    void setTagActionList ( TagAction* t, uint8_t size ) {
        TagActionList = t;
        TagActionListSize = size;
    }


    void pre () {
        Serial.print(whoami());
        Serial.print(": ");
    }



    String whoami () {
        return "RFID on pin " + String(PIN_SDA);
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

        void setup(uint8_t _pins_sda[], uint8_t pin_rst) {

            PINS_SDA = _pins_sda;
            PIN_RST = pin_rst;

            pre(); Serial.print("Begin to setup ");
                   Serial.print( getRFIDsCount() );
                   Serial.println(" rfid(s)");
            for ( uint8_t i = 0; i < getRFIDsCount(); i = i + 1 ) {
                rfids[i].setup(PINS_SDA[i], PIN_RST);
                //rfids[i].switchAntennaOff();
            }

        }

        void run () {

            for (uint8_t i = 0; i < getRFIDsCount(); i = i + 1) {
                //Serial.print("Switch antenna ON of RFID ");
                //Serial.print(i);

                rfids[i].switchAntennaOn();
                rfids[i].run();
                rfids[i].switchAntennaOff();

                //Serial.print("Switch antenna OFF of RFID ");
                //Serial.println(i);
            }

        }



        uint8_t getRFIDsCount () {
            return rfids_count;
            //return sizeof PINS_SDA / sizeof (uint8_t);
        }



        RFID getRFID ( uint8_t i ) {
            return rfids[i];
        }


        RFID* getRFIDs () {
            return rfids;
        }

        //rfids.setTagActionLists( tagActionList, sizeof tagActionList / sizeof (TagAction) );
        void setTagActionLists ( TagAction* t, uint8_t size ) {

            for ( uint8_t i = 0; i < getRFIDsCount(); i = i + 1 ) {
                rfids[i].setTagActionList(t, size);
            }

        }

        void setOnNewCardHandlers ( void (*func)(void) ) {

            for ( uint8_t i = 0; i < getRFIDsCount(); i = i + 1 ) {
                rfids[i].onNewCardHandler = func;
            }

        }



        //rfids.setOnNewCardHandlers( setOnNewCardHandlers , sizeof setOnNewCardHandlers / sizeof (void(*)(void)) );
        void setOnNewCardHandlers ( void (*func[])(void), uint8_t size ) {

            pre(); Serial.println( "set " + String(size) + " onNewCardHandler(s)." );

            for ( uint8_t i = 0; i < getRFIDsCount(); i = i + 1 ) {
                rfids[i].onNewCardHandler = func[i];
            }

        }



        void pre () {
            Serial.print(whoami());
            Serial.print(": ");
        }



        String whoami () {
            return "RFIDs manager";
        }

};
