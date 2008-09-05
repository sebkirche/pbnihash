// My.h : header file for PBNI class
#ifndef CPBNIHASH_H
#define CPBNIHASH_H

#include <pbext.h>
#include "libhashish.h"

#define TABLE_SIZE 100

class PbniHash : public IPBX_NonVisualObject
{
public:
	// construction/destruction
	PbniHash();
	PbniHash( IPB_Session * pSession );
	virtual ~PbniHash();

	// IPBX_UserObject methods
	PBXRESULT Invoke
	(
		IPB_Session * session,
		pbobject obj,
		pbmethodID mid,
		PBCallInfo * ci
	);

   void Destroy();

	// PowerBuilder method wrappers
	enum Function_Entrys
	{
		mid_Hello = 0,
		mid_Add,
		mid_Get,
		mid_Remove,
		mid_Count,
		mid_GetKeys,
		NO_MORE_METHODS
	};


protected:
 	// methods callable from PowerBuilder
	PBXRESULT Hello( PBCallInfo * ci );
	PBXRESULT Add(PBCallInfo * ci);
	PBXRESULT Get(PBCallInfo * ci);
	PBXRESULT Remove(PBCallInfo * ci);
	PBXRESULT Count(PBCallInfo * ci);
	PBXRESULT GetKeys(PBCallInfo * ci);

protected:
    // member variables
    IPB_Session * m_pSession;
	hi_handle_t * m_hi_handle;
	typedef struct PBDataRec {
		void *key;
		void *data;
	} PBDATAREC, *PPBDATAREC;
 };

#endif	// !defined(CPBNIHASH_H)