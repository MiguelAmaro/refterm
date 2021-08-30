/* date = August 28th 2021 10:25 pm */

#ifndef REFTERM_SERIAL_PORT_H
#define REFTERM_SERIAL_PORT_H

// NOTE(MIGUEL): Should This code go in the windows platfom layer?? and i just keep abastractions and generics here
//#include "RingBuffer.h"
//#include "LAL.h"

typedef struct
{
    HANDLE comm;
    //RingBuffer transmit_queue;
    //RingBuffer recieve_queue;
    //u16 padding;
} Device_Comm;

static Device_Comm g_SerialPort_device = {0}; 
static uint8_t          g_SerialPort_buffer[256];

uint32_t win32_SerialPort_device_init(void);

// NOTE(MIGUEL): this funtion was copied over to refterm_example.c under EstablishSerialPortConnection(void)
// NOTE(MIGUEL): should take params to decide a device
uint32_t
win32_SerialPort_init(void)
{
    // Created An IO Stream to talk with micro controller
    
    //HANDLE Comm_Handle;
    
    // NOTE(MIGUEL): How do I know which com it is??
    // TODO(MIGUEL): Find out why comm get initialize with garbage
    g_SerialPort_device.comm = CreateFile("\\\\.\\COM12",
                                          GENERIC_READ | GENERIC_WRITE,
                                          0,
                                          NULL,
                                          OPEN_EXISTING,
                                          0,
                                          NULL);
    
    
    if(g_SerialPort_device.comm == INVALID_HANDLE_VALUE)
    {
        //printf("Error in opening serial port \n");
        return 0;
    }
    else
    {
        //printf("Opening serial port successful \n");
    }
    
    DCB DeviceSerialParams       = { 0 }; // Initializing DCB structure
    DeviceSerialParams.DCBlength = sizeof(DeviceSerialParams);
    DeviceSerialParams.BaudRate  = 9600      ;
    DeviceSerialParams.ByteSize  = 8         ;         
    DeviceSerialParams.StopBits  = ONESTOPBIT;
    DeviceSerialParams.Parity    = NOPARITY  ;
    
    SetCommState(g_SerialPort_device.comm, &DeviceSerialParams);
    
    
    COMMTIMEOUTS TimeoutsComm                = { 0 };
    TimeoutsComm.ReadIntervalTimeout         = 50; // in milliseconds
    TimeoutsComm.ReadTotalTimeoutConstant    = 50; // in milliseconds
    TimeoutsComm.ReadTotalTimeoutMultiplier  = 10; // in milliseconds
    TimeoutsComm.WriteTotalTimeoutConstant   = 50; // in milliseconds
    TimeoutsComm.WriteTotalTimeoutMultiplier = 10; // in milliseconds
    
    SetCommTimeouts(g_SerialPort_device.comm, &TimeoutsComm);
    
    DWORD dwEventMask;
    uint32_t status = 0;
    
    if(!SetCommMask(g_SerialPort_device.comm, EV_RXCHAR))
    { return 0; }
    
    if(!WaitCommEvent(g_SerialPort_device.comm, &dwEventMask, NULL))
    { return 0; }
    
    return 1;
}

void
win32_SerialPort_send_data(uint8_t *bytes, uint32_t size)
{
    unsigned long num_bytes_written = 0;
    uint32_t tx_status         = 0; // Transmission
    
    tx_status = WriteFile(g_SerialPort_device.comm, // Handle to the Serial port
                          bytes                   , // Data to be written to the port
                          size                    , // Number of bytes to write
                          &num_bytes_written      , // Bytes written
                          0)                ;
    Assert(tx_status);
    
    return;
}

void
win32_SerialPort_recieve_data(void)
{
    // NOTE(MIGUEL): This seems to be very similar to 
    //               static int UpdateTerminalBuffer(example_terminal *Terminal, HANDLE FromPipe)
    //               Maybe not how do I integrate this?
    
    // Reading
    //RoundBuffer* SerialBuffer = RoundBuffer_Create(256);
    
    DWORD bytes_to_read; //ReadCount
    uint8_t temp_byte;
    int i = 0;
    
    do
    {
        ReadFile( g_SerialPort_device.comm, //Handle of the Serial port
                 &temp_byte         ,
                 sizeof(temp_byte)  ,
                 &bytes_to_read     ,
                 NULL              );
        
        g_SerialPort_buffer[i] = temp_byte;
        i++;
    }
    while (bytes_to_read > 0 && i < 256);
    
    return;
}

void
win32_SerialPort_close_device()
{
    CloseHandle(g_SerialPort_device.comm);//Closing the Serial Port
    
    return;
}

#endif //REFTERM_SERIAL_PORT_H
