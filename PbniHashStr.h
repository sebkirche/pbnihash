// My.h : header file for PBNI class
#ifndef CPBNIHASHSTR_H
#define CPBNIHASHSTR_H


#include <pbext.h>
#include "libhashish.h"

//#define _CRT_SECURE_NO_DEPRECATE	//disable the warnings concerning unsecure deprecated crt funcs
#define TABLE_STR_SIZE 23

class PbniHashStr : public IPBX_NonVisualObject
{
public:
	// construction/destruction
	PbniHashStr();
	PbniHashStr( IPB_Session * pSession );
	virtual ~PbniHashStr();

	

	// IPBX_UserObject methods
	PBXRESULT Invoke
	(
		IPB_Session * session,
		pbobject obj,
		pbmethodID mid,
		PBCallInfo * ci
	);

   void Destroy();
   // utility functions
   static bool ToAnsi(LPCWSTR inStr, LPSTR outStr);
   static bool ToUnicode(LPCSTR inStr, LPWSTR outStr);


	// PowerBuilder method wrappers
	enum Function_Entrys
	{
		mid_Hello = 0,
		mid_Add,
		mid_Get,
		mid_Remove,
		NO_MORE_METHODS
	};


protected:
 	// methods callable from PowerBuilder
	PBXRESULT Hello(PBCallInfo * ci);
	PBXRESULT Add(PBCallInfo * ci);
	PBXRESULT Get(PBCallInfo * ci);
	PBXRESULT Remove(PBCallInfo * ci);

protected:
    // member variables
    IPB_Session * m_pSession;
	hi_handle_t * m_hi_handle;
 };

#endif	// !defined(CPBNIHASHSTR_H)