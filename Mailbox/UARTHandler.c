#include "UARTHandler.h"

#define INCREMENT_INDEX(X)  ((X) + 1 >= BUFFER_SIZE ? 0 : (X) + 1)

//Project includes

//Standard includes
#include <stdlib.h>
#include <string.h>

//Tivaware includes
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

//Type declarations
typedef struct Buffer_tag{
    char data[BUFFER_SIZE];
    uint32_t start;
    uint32_t current;
} Buffer;


//File global variables
static Buffer OutgoingBuffer;
static Buffer IncomingBuffer;


//Function Prototypes
bool AddByteToBuffer(Buffer* buffer, char byte);
void UART_ISR(void);
void TransmitBuffer(void);
bool BufferFull(Buffer *buffer);
bool BufferEmpty(Buffer *buffer);
char GetNextByte(Buffer* buffer);
bool Peak(Buffer* buffer, char c);

void InitializeUARTHandler(void){
    //Initialize buffers
    OutgoingBuffer.start = 0;
    OutgoingBuffer.current = 0;
    IncomingBuffer.start = 0;
    IncomingBuffer.current = 0;

    int i;
    for(i = 0; i < BUFFER_SIZE; i++){
        OutgoingBuffer.data[i] = 0;
        IncomingBuffer.data[i] = 0;
    }

    //
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    //
    // Enable UART0 so that we can configure the clock.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);


    //Configure UART for interrupts
    UARTFIFODisable(UART0_BASE);    //Generates an interrupt on every receive byte
    UARTIntRegister(UART0_BASE, UART_ISR);
    UARTIntEnable(UART0_BASE, UART_INT_TX | UART_INT_RX);
    UARTTxIntModeSet(UART0_BASE, UART_TXINT_MODE_EOT);  //Generate an interrupt after all bytes transmitted
}

bool BufferFull(Buffer *buffer){
    bool isFull = false;

    if(INCREMENT_INDEX(buffer->start) == buffer->current){
        isFull = true;
    }
    else{
        isFull = false;
    }

    return isFull;
}

bool BufferEmpty(Buffer *buffer){
    bool isFull = false;

    if(buffer->start == buffer->current){
        isFull = true;
    }
    else{
        isFull = false;
    }

    return isFull;
}

void UART_ISR(void){
    if(UARTIntStatus(UART0_BASE, true) & UART_INT_TX){
        UARTIntDisable(UART0_BASE, UART_INT_TX);
        TransmitBuffer();
        UARTIntClear(UART0_BASE, UART_INT_TX);
        UARTIntEnable(UART0_BASE, UART_INT_TX);
    }

    if(UARTIntStatus(UART0_BASE, true) & UART_INT_RX){
        UARTIntDisable(UART0_BASE, UART_INT_RX);

        //Collect all bytes available
        do{
            AddByteToBuffer(&IncomingBuffer, UARTCharGetNonBlocking(UART0_BASE));
        }
        while(UARTCharsAvail(UART0_BASE) && !BufferFull(&IncomingBuffer));

        UARTIntClear(UART0_BASE, UART_INT_RX);
        UARTRxErrorClear(UART0_BASE);
        UARTIntEnable(UART0_BASE, UART_INT_RX);
    }
}


bool AddByteToBuffer(Buffer* buffer, char byte){
    bool bufferHasRoom = false;

    if(INCREMENT_INDEX(buffer->current) != buffer->start){
        buffer->data[buffer->current] = byte;
        buffer->current = INCREMENT_INDEX(buffer->current);
        bufferHasRoom = true;
    }
    else{
        //Buffer is full
    }

    return bufferHasRoom;
}

uint32_t AddBytesToBuffer(char *bytes, uint32_t nBytes){
    uint32_t i = 0;
    bool bufferHasRoom = false;

    do{
        bufferHasRoom = AddByteToBuffer(&OutgoingBuffer, bytes[i]);

        if(bufferHasRoom){
            i++;
        }
    }
    while(i < nBytes && bufferHasRoom);

    TransmitBuffer();

    return i;
}

void TransmitBuffer(void){
    while(OutgoingBuffer.start != OutgoingBuffer.current &&
            UARTSpaceAvail(UART0_BASE))
    {
        if(UARTSpaceAvail(UART0_BASE)){
            UARTCharPutNonBlocking(
                    UART0_BASE,
                    OutgoingBuffer.data[OutgoingBuffer.start]
            );

            OutgoingBuffer.start = INCREMENT_INDEX(OutgoingBuffer.start);
        }

    }
}

char GetNextByte(Buffer* buffer){
    char byte = 0x00;

    if(buffer->start != buffer->current){
        byte = buffer->data[buffer->start];
        buffer->start = INCREMENT_INDEX(buffer->start);
    }

    return byte;
}

bool Peak(Buffer* buffer, char c){
    bool charFound = false;
    uint32_t start = buffer->start;

    while(start != buffer->current && !charFound){
        if(buffer->data[start] == c){
            charFound = true;
        }
        else{
            //Char not found, keep traversing
            start = INCREMENT_INDEX(start);
        }
    }

    return charFound;
}

bool MessageAvailable(void){
    bool isMessageAvailable = false;

    //Clear the buffer until a start byte is at the top (or nothing)
    while(!BufferEmpty(&IncomingBuffer) &&
            IncomingBuffer.data[IncomingBuffer.start] != MESSAGE_START_BYTE
            )
    {
        GetNextByte(&IncomingBuffer);  //Discard next byte
    }

    if(Peak(&IncomingBuffer, MESSAGE_START_BYTE)){
        if(Peak(&IncomingBuffer, MESSAGE_END_BYTE)){
            if(Peak(&IncomingBuffer, MESSAGE_ID_END_BYTE)){
                //Full message is present, parse it
                isMessageAvailable = true;
            }
            else{
                //Message is corrupt, discard it
                char c = 0x00;
                while(c != MESSAGE_END_BYTE){
                    c = GetNextByte(&IncomingBuffer);
                }
            }
        }
        else if(BufferFull(&IncomingBuffer)){
            //Buffer overflow, clear buffer
            IncomingBuffer.start = 0;
            IncomingBuffer.current = 0;
        }
        else{
            //Message still being transmitted
        }
    }
    else{
        //No message yet, do nothing
    }

    return isMessageAvailable;
}

Message ParseMessage(void){
    Message m;
    memset(m.data, 0, MAX_MESSAGE_DATA_LENGTH);
    m.ID = 0;  //Invalid message ID, used to tell if a message arrived

    if(MessageAvailable()){
        //Discard start byte from buffer
        GetNextByte(&IncomingBuffer);

        //Read the ID into a buffer and convert to an integer
        char IDBuffer[5];
        memset(IDBuffer, 0, 5);

        char nextByte = GetNextByte(&IncomingBuffer);
        uint8_t i = 0;
        do{
            IDBuffer[i] = nextByte;
            i++;
            nextByte = GetNextByte(&IncomingBuffer);
        }
        while(nextByte != MESSAGE_ID_END_BYTE);
        m.ID = strtol(IDBuffer, 0, 10);

        //Read the bytes until the end into the messages data

        i = 0;
        nextByte = GetNextByte(&IncomingBuffer);
        do{
            m.data[i] = nextByte;
            i++;
            nextByte = GetNextByte(&IncomingBuffer);
        }
        while(nextByte != MESSAGE_END_BYTE);
    }

    return m;
}
