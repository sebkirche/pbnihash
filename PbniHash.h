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
		// TODO: add enum entries for each callable method
		NO_MORE_METHODS
	};


protected:
 	// methods callable from PowerBuilder
	PBXRESULT Hello( PBCallInfo * ci );

protected:
    // member variables
    IPB_Session * m_pSession;
	hi_handle_t * m_hi_handle;
 };

#endif	// !defined(CPBNIHASH_H)