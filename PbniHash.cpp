// PbniHash.cpp : PBNI class
#include "PbniHash.h"


// default constructor
PbniHash::PbniHash()
{
	struct hi_init_set hi_set;

	hi_set_zero(&hi_set);
	hi_set_bucket_size(&hi_set, TABLE_SIZE);
	hi_set_hash_alg(&hi_set, HI_HASH_DEFAULT);
	hi_set_coll_eng(&hi_set, COLL_ENG_LIST);
	hi_set_key_cmp_func(&hi_set, hi_cmp_int32);

	hi_create(&m_hi_handle, &hi_set);	//todo: check error
}

PbniHash::PbniHash( IPB_Session * pSession )
:m_pSession( pSession )
{
}

// destructor
PbniHash::~PbniHash()
{
	hi_fini(m_hi_handle);
}

// method called by PowerBuilder to invoke PBNI class methods
PBXRESULT PbniHash::Invoke
(
	IPB_Session * session,
	pbobject obj,
	pbmethodID mid,
	PBCallInfo * ci
)
{
   PBXRESULT pbxr = PBX_OK;

	switch ( mid )
	{
		case mid_Hello:
			pbxr = this->Hello( ci );
         break;
		
		// TODO: add handlers for other callable methods

		default:
			pbxr = PBX_E_INVOKE_METHOD_AMBIGUOUS;
	}

	return pbxr;
}


void PbniHash::Destroy() 
{
   delete this;
}

// Method callable from PowerBuilder
PBXRESULT PbniHash::Hello( PBCallInfo * ci )
{
	PBXRESULT	pbxr = PBX_OK;

	// return value
	ci->returnValue->SetString( _T("Hello from PbniHash") );

	return pbxr;
}


