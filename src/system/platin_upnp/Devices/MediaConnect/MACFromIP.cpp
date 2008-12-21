/*****************************************************************
|
|      Platinum - AV Media Connect Device
|
|      (c) 2004 Sylvain Rebaud
|      Author: Sylvain Rebaud (c0diq@yahoo.com)
|
****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <malloc.h>
#include <windows.h>
#include <memory.h>
#include <time.h>
#include <iprtrmib.h>
#include "MACFromIP.h"  

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.

//
// IP_ADDRESS_STRING - store an IP address as a dotted decimal string
//

typedef struct {
  char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

//
// IP_ADDR_STRING - store an IP address with its corresponding subnet mask,
// both as dotted decimal strings
//

typedef struct _IP_ADDR_STRING {
  struct _IP_ADDR_STRING* Next;
  IP_ADDRESS_STRING IpAddress;
  IP_MASK_STRING IpMask;
  DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;

//
// ADAPTER_INFO - per-adapter information. All IP addresses are stored as
// strings
//

typedef struct _IP_ADAPTER_INFO {
  struct _IP_ADAPTER_INFO* Next;
  DWORD ComboIndex;
  char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
  char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
  UINT AddressLength;
  BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
  DWORD Index;
  UINT Type;
  UINT DhcpEnabled;
  PIP_ADDR_STRING CurrentIpAddress;
  IP_ADDR_STRING IpAddressList;
  IP_ADDR_STRING GatewayList;
  IP_ADDR_STRING DhcpServer;
  BOOL HaveWins;
  IP_ADDR_STRING PrimaryWinsServer;
  IP_ADDR_STRING SecondaryWinsServer;
  time_t LeaseObtained;
  time_t LeaseExpires;
} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;


typedef DWORD (__stdcall *_GetIpNetTable)(PMIB_IPNETTABLE pIpNetTable, PULONG pdwSize,BOOL bOrder);
typedef DWORD (__stdcall *_GetAdaptersInfo)(PIP_ADAPTER_INFO pAdapterInfo,PULONG pOutBufLen);

int GetMACFromIP(char* szIP, NPT_String& MAC)
{
  // condition: WSAStartup must have been called before
  SOCKET udp_s; 
  SOCKADDR_IN udp_sin; 
  DWORD _ip;
  HMODULE hDLL; 

  hDLL = LoadLibrary("iphlpapi.dll");

  // first, compare the IP with the local addresses of the computer (because those IPs aren't in the ARP table)
  if (hDLL && GetProcAddress(hDLL, "GetAdaptersInfo")) {
    PIP_ADAPTER_INFO pAdapterInfo;
    ULONG AdapterSize;

    DWORD dwRet = ((_GetAdaptersInfo)GetProcAddress(hDLL, "GetAdaptersInfo"))(NULL, &AdapterSize);
    if (dwRet == ERROR_BUFFER_OVERFLOW) {
      pAdapterInfo = (PIP_ADAPTER_INFO)malloc(AdapterSize);
      memset(pAdapterInfo, 0, AdapterSize);
      dwRet = ((_GetAdaptersInfo)GetProcAddress(hDLL, "GetAdaptersInfo"))(pAdapterInfo, &AdapterSize);

      if (dwRet == NOERROR) {
        // ok went fine, check all the adapters
        PIP_ADAPTER_INFO pCurrent = pAdapterInfo;
        while (pCurrent) {
          // compare the IP
          if (strcmp(pCurrent->IpAddressList.IpAddress.String, szIP) == 0) {
            // it's the right adapter, so create the MAC and return it
            char* szMAC = (char*)malloc(19);
            sprintf(szMAC, "%02X-%02X-%02X-%02X-%02X-%02X", 
              pCurrent->Address[0],
              pCurrent->Address[1],
              pCurrent->Address[2],
              pCurrent->Address[3],
              pCurrent->Address[4],
              pCurrent->Address[5]);
            MAC = szMAC;
            free(szMAC);
            free(pAdapterInfo);
            FreeLibrary(hDLL);
            return 1;
          }
          // go to the next adapter
          pCurrent = pCurrent->Next;
        }
      }
      free(pAdapterInfo);
    }
  }

  // we didn't find it locally, so use the ARP table now

  /*  if (WSAStartup(0x0202, &WsaData)==NULL)    
  printf("WSA Startup OK!\n");*/

  _ip=inet_addr(szIP); 

  // initiate a socket connection so the ARP table is updated
  udp_s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
  if(udp_s!=SOCKET_ERROR) 
  { 
    udp_sin.sin_family = AF_INET; 
    udp_sin.sin_port = htons(5232); // why 5232... who knows
    udp_sin.sin_addr.s_addr = _ip; 
    if(sendto(udp_s, "TEST", 5, NULL, (SOCKADDR*)&udp_sin, sizeof(udp_sin))>0) 
    { 
      // if the Send worked, then the table is updated, and we can get it
      ULONG cbIpNetTable = 0; 
      MIB_IPNETTABLE * pIpNetTable;
      if (hDLL && GetProcAddress(hDLL, "GetIpNetTable")) {
        DWORD dwRet = ((_GetIpNetTable)GetProcAddress(hDLL, "GetIpNetTable"))(NULL, &cbIpNetTable, FALSE);

        // now that we have the size we need, create the table
        pIpNetTable = (MIB_IPNETTABLE *) malloc(cbIpNetTable); 
        memset(pIpNetTable, 0, cbIpNetTable);
        dwRet = ((_GetIpNetTable)GetProcAddress(hDLL, "GetIpNetTable")) (pIpNetTable, &cbIpNetTable, FALSE);
        if (dwRet == NO_ERROR) 
        { 
          DWORD i;
          for (i = 0; i < pIpNetTable->dwNumEntries; i++)
          { 
            // compare to the provided IP
            if(pIpNetTable->table[i].dwAddr==_ip&&pIpNetTable->table[i].dwType!=2) 
            { 
              char* szMAC = (char*)malloc(19);
              sprintf(szMAC, "%02X-%02X-%02X-%02X-%02X-%02X", 
                pIpNetTable->table[i].bPhysAddr[0], 
                pIpNetTable->table[i].bPhysAddr[1], 
                pIpNetTable->table[i].bPhysAddr[2], 
                pIpNetTable->table[i].bPhysAddr[3], 
                pIpNetTable->table[i].bPhysAddr[4], 
                pIpNetTable->table[i].bPhysAddr[5]); 
              MAC = szMAC;
              free(szMAC);
              free(pIpNetTable); 
              closesocket(udp_s); 
              return 1;
            } 
          } 
          free(pIpNetTable); 
          // "MAC-address not found"; 
        } 
        else {
          // "ERROR Open IPMAC table"; 
        }

      }
    } 
    else {
      // "Send data ERROR!"; 
    }

    closesocket(udp_s); 
  } 
  else {
    // "ERROR open socket"; 
  }
  FreeLibrary(hDLL);
  return 0;
}
