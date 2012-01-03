// My.h : header file for PBNI class
#ifndef CPBNIHASH_H
#define CPBNIHASH_H

#include <pbext.h>
#include "libhashish.h"

#define STRING2(x) #x
#define STRING(x) STRING2(x)
#pragma message( "**** Using PBVM " STRING(PBVM_VERSION) " ****" )

#define TABLE_SIZE 100

class PbniHash : public IPBX_NonVisualObject
{
public:
	// construction/destruction
	PbniHash();
	PbniHash( IPB_Session * pSession );
	virtual ~PbniHash();
	void ReleaseSessionString(LPCTSTR str){
#if PBVM_VERSION >=105
	if(m_pSession && str)
		m_pSession->ReleaseString(str);
#endif
	}

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
		mid_GetVersion = 0,
		mid_Add,
		mid_Get,
		mid_Remove,
		mid_Count,
		mid_GetKeys,
		mid_GetLastError,
		mid_GetLastErrMsg,
		//mid_UseStrCompare,
		mid_Purge,
		mid_Set,
		mid_GetValues,
		mid_GetEach,
		mid_Serialize,
		mid_UnSerialize,
		mid_OnSerialize,
		mid_OnUnSerialize,
		NO_MORE_METHODS
	};


protected:
 	// methods callable from PowerBuilder
	PBXRESULT GetVersion( PBCallInfo * ci );
	PBXRESULT Add(PBCallInfo * ci);
	PBXRESULT Get(PBCallInfo * ci);
	PBXRESULT Remove(PBCallInfo * ci);
	PBXRESULT Count(PBCallInfo * ci);
	PBXRESULT GetKeys(PBCallInfo * ci);
	PBXRESULT GetLastErr(PBCallInfo * ci);
	PBXRESULT GetLastErrMsg(PBCallInfo * ci);
	PBXRESULT UseStringCompare(PBCallInfo * ci);
	PBXRESULT Purge(PBCallInfo * ci);
	PBXRESULT Set(PBCallInfo * ci);
	PBXRESULT GetValues(PBCallInfo * ci);
	PBXRESULT GetEach(PBCallInfo * ci);
	PBXRESULT Serialize(PBCallInfo * ci);
	PBXRESULT UnSerialize(PBCallInfo * ci);
	PBXRESULT OnSerialize(PBCallInfo * ci);
	PBXRESULT OnUnSerialize(PBCallInfo * ci);
	void DoPurge();

protected:
    // member variables
    IPB_Session * m_pSession;
	hi_handle_t * m_hi_handle;
	typedef struct PBDataRec {
		void *key;
		void *data;
	} PBDATAREC, *PPBDATAREC;
	int m_lastError;
 };

#endif	// !defined(CPBNIHASH_H)