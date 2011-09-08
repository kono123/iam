#ifndef	_LMX9820_
#define _LMX9820_
//====================================================================================================================
// Interface with Bluetooth module LMX9820 connected to USART
// (C) Skvortsov Artyom, 2005 mega103@online.ru 8-926-528-9157
// Header file, version 1.1
//====================================================================================================================
// Select UART channel connected to Bluetooth module
#define	BT_iRTS		PORTD.4
#define	BT_iCTS		PIND.5
#define	BT_iRES		PORTB.5
//------------------------------------------
#define	BT_UART		1		// 0 or 1
//------------------------------------------
#if BT_UART == 0
	#define bt_getchar		getchar
	#define bt_putchar		putchar
	#define bt_rxcounter		rx_counter0
	#define bt_txcounter		tx_counter0
	#define bt_rxbufferoverflow	rx_buffer_overflow0
	#define bt_rxwrindex		rx_wr_index0
	#define bt_rxrdindex		rx_rd_index0
#else
	#define bt_getchar		getchar1
	#define bt_putchar		putchar1
	#define bt_rxcounter		rx_counter1
	#define bt_txcounter		tx_counter1
	#define bt_rxbufferoverflow	rx_buffer_overflow1
	#define bt_rxwrindex		rx_wr_index1
	#define bt_rxrdindex		rx_rd_index1
#endif
//------------------------------------------
#define	BT_MAXLOCALPORTS	31	// smaller by 1, 0 number is not used
#define	BT_DATAOFFSET		3
#define	BT_MAXPAYLOAD		330
#define BT_BUFSIZE		333
#if (BT_BUFSIZE < (BT_MAXPAYLOAD + BT_DATAOFFSET))
	#warning "Warning! Bluetooth buffer size smaller than maximum payload in incoming data!";
#endif
//------------------------------------------
// Packet type identification
#define	BTPAK_REQ				0x52	// REQUEST, Request send to Bluetooth module
#define	BTPAK_CFM				0x43	// CONFIRM, The Bluetooth module confirms to a request
#define	BTPAK_IND				0x69	// INDICATION, Information sent from the Bluetooth module
#define	BTPAK_RES				0x72	// RESPONSE, An optional response to an indication
//------------------------------------------
// Start & End delimiters
#define	BTDLM_STX				0x02
#define	BTDLM_ETX				0x03
//------------------------------------------
// Fields in packet descriptions
#define	BTFLD_STX			0
#define	BTFLD_TYPE			1
#define	BTFLD_OPCODE			2
#define	BTFLD_DLEN			3
#define	BTFLD_CHK			5
#define	BTFLD_DATA			6
//------------------------------------------
// Opcode values
#define GAP_INQUIRY			0x00
#define GAP_DEVICE_FOUND		0x01
#define GAP_REMOTE_DEVICE_NAME		0x02
#define GAP_READ_LOCAL_NAME		0x03
#define GAP_WRITE_LOCAL_NAME		0x04
#define GAP_READ_LOCAL_BDA		0x05
#define GAP_SET_SCANMODE		0x06
#define GAP_GET_FIXED_PIN		0x16
#define GAP_SET_FIXED_PIN		0x17
#define GAP_GET_SECURITY_MODE		0x18
#define GAP_SET_SECURITY_MODE		0x19
#define GAP_REMOVE_PAIRING		0x1B
#define GAP_LIST_PAIRED_DEVICES		0x1C
#define GAP_ENTER_SNIFF_MODE		0x21
#define GAP_EXIT_SNIFF_MODE		0x37
#define GAP_ENTER_PARK_MODE		0x38
#define GAP_EXIT_PARK_MODE		0x39
#define GAP_ENTER_HOLD_MODE		0x3A
#define GAP_SET_LINK_POLICY		0x3B
#define GAP_GET_LINK_POLICY		0x3C
#define GAP_POWER_SAVE_MODE_CHANGED	0x3D
#define GAP_ACL_ESTABLISHED		0x50
#define GAP_ACL_TERMINATED		0x51
//------------------------------------------
#define SPP_SET_PORT_CONFIG		0x07
#define SPP_GET_PORT_CONFIG		0x08
#define SPP_PORT_CONFIG_CHANGED		0x09
#define SPP_ESTABLISH_LINK		0x0A
#define SPP_LINK_ESTABLISHED		0x0B
#define SPP_INCOMING_LINK_ESTABLISHED  	0x0C
#define SPP_RELEASE_LINK		0x0D
#define SPP_LINK_RELEASED		0x0E
#define SPP_SEND_DATA			0x0F
#define SPP_INCOMING_DATA		0x10
#define SPP_TRANSPARENT_MODE		0x11
#define SPP_CONNECT_DEFAULT_CON		0x12
#define SPP_STORE_DEFAULT_CON		0x13
#define SPP_GET_LIST_DEFAULT_CON        0x14
#define SPP_DELETE_DEFAULT_CON          0x15
#define SPP_PORT_STATUS_CHANGED		0x3E
#define SPP_GET_PORT_STATUS		0x40
#define SPP_PORT_SET_DTR		0x41
#define SPP_PORT_SET_RTS		0x42
#define SPP_PORT_BREAK			0x43
#define SPP_PORT_OVERRUN_ERROR		0x44
#define SPP_PORT_PARITY_ERROR		0x45
#define SPP_PORT_FRAMING_ERROR          0x46
//------------------------------------------
#define SDAP_CONNECT			0x32
#define SDAP_DISCONNECT			0x33
#define SDAP_CONNECTION_LOST		0x34
#define SDAP_SERVICE_BROWSE		0x35
#define SDAP_SERVICE_SEARCH             0x36
#define SDAP_SERVICE_REQUEST            0x1E
#define SDAP_ATTRIBUTE_REQUEST          0x3F
//------------------------------------------
#define CHANGE_LOCAL_BDADDRESS		0x27
#define CHANGE_NVS_UART_SPEED		0x23
#define CHANGE_UART_SETTINGS            0x48
#define SET_PORTS_TO_OPEN		0x22
#define GET_PORTS_TO_OPEN               0x1F
#define RESTORE_FACTORY_SETTINGS        0x1A
#define STORE_CLASS_OF_DEVICE           0x28
#define FORCE_MASTER_ROLE               0x1D
#define READ_OPERATION_MODE             0x49
#define WRITE_OPERATION_MODE            0x4A
#define SET_DEFAULT_LINK_POLICY         0x4C
#define GET_DEFAULT_LINK_POLICY         0x4D
#define SET_EVENT_FILTER                0x4E
#define GET_EVENT_FILTER                0x4F
//------------------------------------------
#define ENABLE_SDP_RECORD               0x29
#define DELETE_SDP_RECORDS              0x2A
#define STORE_SPP_RECORD                0x2B
#define STORE_DUN_RECORD                0x2C
#define STORE_FAX_RECORD                0x2D
#define STORE_OPP_RECORD                0x2E
#define STORE_FTP_RECORD                0x2F
#define STORE_SYNC_RECORD               0x30
//------------------------------------------
#define LMX9820_RESET                   0x26
#define LMX9820_READY                   0x25
#define LMX9820_TEST_MODE               0x24
#define LMX9820_FIRMWARE_UPGRADE        0x47
#define LMX9820_READ_RSSI               0x20
#define LMX9820_RF_TEST_MODE            0x4B
//------------------------------------------
// RFCOMM Release Reasons
#define RFCR_DLC_DISC_LOCAL_DEVICE	0x00		// The local device has disconnected the DLC.
#define RFCR_DLC_DISC_REMOTE_DEVICE	0x01		// The remote device has disconnected the DLC.
#define RFCR_DLC_DISC_ACL_FAILURE	0x02		// ACL link failure/ link supervision timeout.
#define RFCR_DLC_DISC_LOWER_LAYER	0x03		// Lower layer (e.g. L2CAP) has disconnected the DLC.
//------------------------------------------
// RFCOMM Error Codes
#define RFCS_NO_ERROR			0x00		// No error
#define RFCS_INVALID_DLC		0x01		// The DLC does not exist
#define RFCS_INVALID_PORT		0x02		// The port does not exist
#define RFCS_DLC_ESTABLISH_FAILED	0x03		// The DLC establishment failed
#define RFCS_ACCESS_REJECTED		0x04		// SECM did not authorize access to the requested ser-vice (DLC)
#define RFCS_INVALID_CONNECTION		0x05		// There does not exist a DLC/L2CAP connection to the device
#define RFCS_UNSPECIFIED_ERROR		0xff		// Not used
//----------------------------------------------------------
// Generic Error Codes
#define BT_ER_OK 					0x00	// No error.
#define BT_ER_INVALID_NO_OF_PARAMETERS			0x01	// The number of bytes in the request does not corre-spond to the protocol specification	
#define BT_ER_DURATION_OUT_OF_RANGE			0x02	// The given duration value is not valid according to the specification.
#define BT_ER_INVALID_MODE				0x03	// The selected mode is not valid according to the spec-ification
#define BT_ER_TIMEOUT					0x04	// A timeout occurred.
#define BT_ER_UNKNOWN_ERROR				0x05	// An unknown error occurred.
#define BT_ER_NAME_TOO_LONG				0x06	// The number of bytes in the name string is longer than the maximum specified value.
#define BT_ER_INVALID_DISCOVER_PARAM			0x07	// The given discoverability parameter does not contain a valid value according to the specification.
#define BT_ER_INVALID_CONNECT_PARAM	     		0x08	// The given connectability parameter does not contain a valid value according to the specification.
#define BT_ER_INVALID_SECURITY_MODE			0x09	// The given security mode is not a valid Bluetooth security mode.
#define BT_ER_LINKKEY_DOES_NOT_EXISTS			0x0a	// No link key exists for the given Bluetooth address
#define BT_ER_CONNECTION_FAILED				0x0b	// The connection setup failed due to unknown reasons.
#define BT_ER_TRUNCATED_ANSWER				0x0c	// The returned number of services is too large to be handled by the LMX9820. The answer is truncated
#define BT_ER_RESULT_TOO_LARGE				0x0d	// The SDP result from the remote device is too large to be handled by the LMX9820 due to ram limitations
#define BT_ER_ENTER_TESTMODE	     			0x0e	// It is currently not possible to enter the selected test mode.
#define BT_ER_ILLEGAL_TESTMODE				0x0f	// The given test mode is not a valid test mode accord-ing to the specification
#define BT_ER_RESET_TO_NSC_BDADDRESS			0x10	// The LMX9820 will change the Bluetooth address to the NSC address.
#define BT_ER_UART_SPEED_OUT_OF_RANGE			0x11	// The selected UART speed value is not valid accord-ing to the specification.
#define BT_ER_INVALID_PORT				0x12	// The given port value is larger than the maximum specified value.
#define BT_ER_ILLEGAL_STATE_VALUE			0x13	// The given state value is not a valid state according to the specification
#define BT_ER_IDENTIFIER_OUT_OF_RANGE			0x14	// The given identifier is larger than the maximum speci-fied value.
#define BT_ER_REC_ALREADY_IN_SEL_STATE	  		0x15	// The service record is already enabled/disabled.
#define BT_ER_INVALID_AUTH_VALUE	  		0x16	// The given authentication value is not a valid value according to the specification.
#define BT_ER_INVALID_ENCRYPT_VALUE			0x17	// The given encryption value is not a valid value according to the specification.
#define BT_ER_MAX_NO_OF_SERVICE_RECORDS			0x18	// The maximum number of service records, which the LMX9820 is able to store, is reached.
#define BT_ER_WRITING_TO_NVS				0x19	// An error occurred while writing to flash. The service record may not be stored.
#define BT_ER_INVALID_ROLE				0x1a	// The given role value is not a valid value according to the specification.
#define BT_ER_LIMIT					0x1b	// Limits exceeded (Parameter(s) violates limits).
#define BT_ER_UNEXPECTED				0x1c	// Unexpected at this moment.
#define BT_ER_UNABLE_TO_SEND				0x1d	// Could not send at this moment, no reason specified.
#define BT_ER_CURRENTLY_NO_BUFFER			0x1e	// Currently no room in buffer, try again later.
#define BT_ER_NO_CONNECTION				0x1f	// Trying to use an inexistent connection.
#define BT_ER_SPP_INVALID_PORT				0x20	// Port number out of range.
#define BT_ER_SPP_PORT_NOT_OPEN				0x21	// Port is closed.
#define BT_ER_SPP_PORT_BUSY				0x22	// Connection establishment on a PORT that has a con-nection.
#define BT_ER_SPP_MULTIPLE_CONNECTIONS			0x23	// Transparent mode attempted while more than 1 con-nection active.
#define BT_ER_SPP_MULTIPLE_TRANSPARENT			0x24	// Trying to store a default connection when a transpar-ent default connection is already stored, or trying to store a transparent default connection when another connection is already stored.
#define BT_ER_SPP_DEF_CNCT_NOT_STORED	  		0x25	// Trying to connection to a default connection, which is not stored.
#define BT_ER_SPP_AUTO_CNCT_PROGRESS   			0x26	// Trying to start connecting to default connections when default connection establishment is already progress-ing.
#define BT_ER_UNSPECIFIED_ERROR				0x27	// Other error.
#define BT_ER_IDENTIFIER_NOT_IN_USE			0x28	// Trying to enable a SDP record which is not stored. Wrong identifier.
#define BT_ER_INVALID_SUPPORTED_FAX			0x29	// Faxclass parameter must be 0 or 1.
#define BT_ER_TOO_MANY_FORMATS	      			0x2a	// The given number of supported formats excesses the specified maximum number of supported formats.
#define BT_ER_TOO_MANY_DATASTORES			0x2b	// The given number of data stores excess the specified maximum number of data stores.
#define BT_ER_ATTEMPT_FAILED				0x2c	// Attempt to change low power mode failed
#define BT_ER_ILLEGAL_LINK_POLICY			0x2d	// The given link policy value is out of range
#define BT_ER_PINCODE_TOO_LONG				0x2e	// The pin code length field is too large.
#define BT_ER_PARITY_BIT_OUT_OF_RANGE			0x2f	// The given parity check is out of range
#define BT_ER_STOP_BITS_OUT_OF_RANGE			0x30	// The given number of stop bits is out of range
//----------------------------------------------------------
#define BT_ER_CONNECT_LOST				0xff	// Error code of this library. When LMX9820 does not acknowledge
//----------------------------------------------------------
// Error codes of internal functions of this library
#define	BT_LIB_OK					0x00	// No new pakets from bluetooth
#define	BT_LIB_NODATA					0x01	// No new pakets from bluetooth
#define	BT_LIB_BUFOVF					0x02	// Bluetooth buffer overflow
#define	BT_LIB_TMOUT					0x03	// Timeout of receive char from bluetooth occured
#define	BT_LIB_CHKERR					0x04	// Checksum error in received paket found
#define	BT_LIB_ENDERR					0x05	// End delimiter in received paket not found
//----------------------------------------------------------
// Codes of high-level functions
#define	BT_ANSWER_NO					0x00
#define	BT_ANSWER_OK			       		0x01
#define	BT_ANSWER_ERR			      		0xff
//----------------------------------------------------------
#define	BT_TMOUT1US_GETCH				2000
#define	BT_TMOUT1MS_GETNAME				20000
#define	BT_TMOUT1MS_SDAPCONNECT				20000
#define	BT_TMOUT1MS_SDAPDISCONNECT			20000
#define	BT_TMOUT1MS_SDAPBROWSE				20000
#define	BT_TMOUT1MS_SPPCREATE				40000
#define	BT_TMOUT1MS_SPPSENDDATA				10000
#define	BT_TMOUT1MS_SPPRELEASE				3000
#define	BT_TMOUT1MS_TRANSPARENTREQ			30000
#define	BT_TMOUT1MS_STOREDEF				30000
#define	BT_TMOUT1MS_CONNECTDEF				30000
#define	BT_TMOUT1MS_DELDEF				30000
#define	BT_TMOUT1MS_RDNAME				20000
#define	BT_TMOUT1MS_WRNAME				20000
#define	BT_TMOUT1MS_WRCLASS				20000
#define	BT_TMOUT1MS_SETSCAN				20000
#define	BT_TMOUT1MS_RDOPMD				20000
#define	BT_TMOUT1MS_WROPMD				20000
#define	BT_TMOUT1MS_RDRSSI				20000
#define	BT_TMOUT1MS_UARTSP				20000
#define	BT_TMOUT1MS_FACSET				20000
#define	BT_TMOUT1MS_GETPIN				20000
#define	BT_TMOUT1MS_SETPIN				20000
#define	BT_TMOUT1MS_SOFTRES				4000
#define	BT_TMOUT1MS_RES					2000
#define	BT_TMOUT1MS_RDSEMD				20000
#define	BT_TMOUT1MS_WRSEMD				20000
#define	BT_TMOUT1MS_WRADDR				20000
#define	BT_TMOUT1MS_RDADDR				20000
#define	BT_TMOUT1MS_REQPAIR				5000
#define	BT_TMOUT1MS_RMPAIR				5000
//----------------------------------------------------------
// ACL Error Codes not listed here
//----------------------------------------------------------
// UART speed constants for bt_change_uart_speed() function
#define	BT_UART_2400	0x00
#define	BT_UART_4800	0x01
#define	BT_UART_7200	0x02
#define	BT_UART_9600	0x03
#define	BT_UART_19200	0x04
#define	BT_UART_38400	0x05
#define	BT_UART_57600	0x06
#define	BT_UART_115200	0x07
#define	BT_UART_230400	0x08
#define	BT_UART_460800	0x09
#define	BT_UART_921600	0x0a
//====================================================================================================================
extern char bt_buf[BT_BUFSIZE];					// buffer for data exchange
extern char bt_error;						// general termination code for all functions
extern bit bt_sdap_active;					// flag of SDAP connection active
extern char bt_connection_active[BT_MAXLOCALPORTS];		// flag of (SPP) connection active for each port
extern char bt_transparent_mode;				// transparent mode indication
extern bit bt_incoming_link;					// flag of incoming link
extern bit bt_incoming_data;					// flag of incoming data received to bt_buf
//------------------------------------------------------------
#define	BTNAMEMAXLEN	30

struct btdevice			// bluetooth device parameters
{
char ena;			// flag of enable this device for use
char addr[6];			// bluetooth device address
char class[3];			// bluetooth device class
char name[BTNAMEMAXLEN];	// bluetooth device user friendly name
};

eeprom struct btdevicee		// eeprom copy
{
char ena;
char addr[6];
char class[3];
char name[BTNAMEMAXLEN];
};
//------------------------------------------------------------
struct btpakheader		// command interface packet header
{
char paktype;
char opcode;
unsigned short datalen;
char checksum;
};
//------------------------------------------------------------
#define	BTSERVICEMAXLEN	40

struct btservice		// bluetooth service parameters
{
unsigned short group;
unsigned short uuid;
char port;
char name[BTSERVICEMAXLEN];
};

eeprom struct btservicee		// eeprom copy
{
unsigned short group;
unsigned short uuid;
char port;
char name[BTSERVICEMAXLEN];
};
//====================================================================================================================
// External bluetooth library functions
// Return value for most functions:
// BT_ANSWER_OK 	= function terminated ok
// BT_ANSWER_ERR 	= error (see code in bt_error)
// BT_ANSWER_NO 	= no new data arrived
//------------------------------------------------------------
// Hardware reset
// !! WAIT 1 SEC AFTER ANY NV RAM FUNCTION!!
char bt_reset(void);
//------------------------------------------------------------
// Software reset
// !! WAIT 1 SEC AFTER ANY NV RAM FUNCTION!!
char bt_resetsoft(void);
//------------------------------------------------------------
// Inquiry command, for discovering devices
// dur 		= duration of inquiry. 1..48. (1 = 1.28 c)
// numresp 	= maximum number of responses. 0x00-0xff (0x00 - unlimited)
// limited	= 0x00 - general inquiry, 0x01 - limited inquiry
void bt_inquiry(char dur, char numresp, char limited);
//------------------------------------------------------------
// Inquiry response
// dev 		= pointer to btdevice struct, where device address and class will be stored
// inquiryend	= pointer to return variable. == 1 - no any more devices, inquiry finished
char bt_inquiry_respose(struct btdevice *dev, char *inquiryend);
//------------------------------------------------------------
// Get device user-friendly name
// dev		= pointer to btdevice struct, with device address filled
char bt_get_name(struct btdevice *dev);
//------------------------------------------------------------
// SDAP service connect
// dev		= pointer to btdevice struct, with device address filled
char bt_sdap_connect(struct btdevice *dev);
//------------------------------------------------------------
// SDAP service disconnect
char bt_sdap_disconnect(void);
//------------------------------------------------------------
// Browse services for selected service class
// uuid		= service class (0x1101 - serial port service, 0x1002 - all services)
// services	= returns amount of services found
char bt_sdap_service_browse(unsigned short uuid, char *services);
//------------------------------------------------------------
// Read service, call after bt_sdap_service_browse
// servno	= number of service to read (< services)
// serv		= returns service parameters
char bt_sdap_service_read(struct btservice *serv, char servno);
//------------------------------------------------------------
// Create serial port connection
// dev		= device to connect, with address filled
// localport	= number of local port (1..30)
// localport	= number of port on remote device (discovered by bt_service_read() function)
char bt_spp_create(struct btdevice *dev, char localport, char remoteport);
//------------------------------------------------------------
// Send data in packet mode
// DATA must be saved to bt_buf beginning from index BT_DATAOFFSET
// max data size is 330 bytes
// localport	= number of local port using which connection was established
char bt_spp_senddata(char localport, unsigned short len);
//------------------------------------------------------------
// Send data in packet mode
// DATA must be saved to array
// max data size is 330 bytes
// localport	= number of local port using which connection was established
char bt_spp_senddata_array(char localport, unsigned short len, char *array);
//------------------------------------------------------------
// Receive data in packet mode
// DATA must be read from bt_buf beginning from index BT_DATAOFFSET
// max data size is 330 bytes
// localport	= number of local port from which data received
char bt_spp_receivedata(char *localport, unsigned short *len);
//------------------------------------------------------------
// Terminate serial port connection
// localport	= number of local port
char bt_spp_release_link(char localport);
//------------------------------------------------------------
// Discover address and local port of device who is established connection
// Call after bt_listen() returns SPP_INCOMING_LINK_ESTABLISHED
char bt_spp_incoming_parameters(struct btdevice *dev, char *localport);
//------------------------------------------------------------
// After SPP connection established, enable to select to transparent mode
// For terminate transparent mode host must send UART BREAK signal acknowledged by UART BREAK too
// Also if UART BREAK received, host must select to packet mode and receive SPP_TRANSPARENT_MODE indication via bt_listen with Transparent mode = off
char bt_transparent_mode_request(char localport);
//------------------------------------------------------------
// Store one of 3 default connections in NV memory
// idx	= 0..2 - number of connection
char bt_store_default_connection(char idx, char localport, char remoteport, char *remoteaddr, char transparent);
//------------------------------------------------------------
// Connect via selected default connection
// idx	= 0..2 - number of connection (0xff - all connections)
char bt_connect_default_connection(char idx);
//------------------------------------------------------------
// Delete default connection
// idx	= 0..2 - number of connection
char bt_delete_default_connection(char idx);
//------------------------------------------------------------
// Read local device name to dev->name
char bt_read_local_name(struct btdevice *dev);
//------------------------------------------------------------
// Write local device name from dev->name
char bt_write_local_name(struct btdevice *dev);
//------------------------------------------------------------
// Read local bluetooth device address, dev->addr
char bt_read_local_address(struct btdevice *dev);
//------------------------------------------------------------
// Write local bluetooth device address, dev->addr
// Writing 0xffffffffffff forces to show default factory programmed device address
char bt_write_local_address(struct btdevice *dev);
//------------------------------------------------------------
// Write local bluetooth device class, dev->class
char bt_write_local_class(struct btdevice *dev);
//------------------------------------------------------------
// Select scan mode for bluetooth device
// connectability 	= 0 - not connectable, 1 - connectable
// discoverability	= 0 - not discoverable, 1 - general, 2 - limited, 3 - automatic limited discoverable modes
char bt_set_scan_mode(char connectability, char discoverability);
//------------------------------------------------------------
// Read current operation mode
// mode			= 0 - command mode, 1 - automatic mode
char bt_read_opmode(char *mode);
//------------------------------------------------------------
// Write current operation mode
// mode			= 0 - command mode, 1 - automatic mode
char bt_write_opmode(char mode);
//------------------------------------------------------------
// Read device signal strength (need dev-addr) in dB
char bt_read_rssi(struct btdevice *dev, char* level);
//------------------------------------------------------------
// Select uart interface speed (use defines BT_UART_xxxxx)
// Get active after reset
// Works if ISEL configuration pins set to Check NV Settings
char bt_change_uart_speed(char speed);
//------------------------------------------------------------
// Restore factory defaults (uart speed to 9600, etc.)
char bt_restore_factory_settings(void);
//------------------------------------------------------------
// Get fixed security pin
// len		= returns size of pin (1..16 bytes)
// pin[]	= returns pin code
char bt_get_pin(char *pin, char *len);
//------------------------------------------------------------
// Set fixed security pin
// len		= size of pin (1..16 bytes)
// pin[]	= pin code
char bt_set_pin(char *pin, char len);
//------------------------------------------------------------
// Get current security mode
// mode		= 0x01 - no security, 0x02 - level determined in service database, 0x03 - necessary, 0x83 - necessary with link layer encrypt
char bt_read_security_mode(char *mode);
//------------------------------------------------------------
// Get current security mode
// mode		= 0x01 - no security, 0x02 - level determined in service database, 0x03 - necessary, 0x83 - necessary with link layer encrypt
char bt_write_security_mode(char mode);
//------------------------------------------------------------
// Decode indication if received
// Special return value:
// 0 - no any indications reseived, else - code of indication
char bt_listen(void);
//------------------------------------------------------------
// List paired devices. Do discover adresses invoke bt_read_paired() after this
// devices	= amount of paired devices (0..7)
char bt_request_paired_list(char *devices);
//------------------------------------------------------------
// Read paired device address
// dev		= write here address of device
// devno	= number of device in paired list to read (0..7)
char bt_read_paired(struct btdevice *dev, char devno);
//------------------------------------------------------------
// Remove pairing device dev->addr from paired list
char bt_remove_pairing(struct btdevice *dev);
//------------------------------------------------------------
// Show size of incoming data in len
// Return numer of port where data arrived
char bt_incoming_data_size(unsigned short *len);
//------------------------------------------------------------
// While there is no function for low power modes
//====================================================================================================================
#endif