// PbniHash.cpp : PBNI class
#include "PbniHash.h"


// default constructor
PbniHash::PbniHash()
{
}

PbniHash::PbniHash( IPB_Session * pSession )
:m_pSession( pSession )
{
	struct hi_init_set hi_set;

	hi_set_zero(&hi_set);
	hi_set_bucket_size(&hi_set, TABLE_SIZE);
	hi_set_hash_alg(&hi_set, HI_HASH_DEFAULT);
	hi_set_coll_eng(&hi_set, COLL_ENG_LIST);
	hi_set_key_cmp_func(&hi_set, hi_cmp_int32);

	hi_create(&m_hi_handle, &hi_set);	//todo: check error
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
		
		case mid_Add:
			pbxr = this->Add(ci);
			break;
		case mid_Get:
			pbxr = this->Get(ci);
			break;
		case mid_Remove:
			pbxr = this->Remove(ci);
			break;

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


PBXRESULT PbniHash::Add( PBCallInfo * ci )
{
	PBXRESULT	pbxr = PBX_OK;
	int hi_res;

	// check arguments
	if ( ci->pArgs->GetAt(0)->IsNull() ||
		ci->pArgs->GetAt(1)->IsNull() )
	{
		// if any of the passed arguments is null, return the null value
		ci->returnValue->SetToNull();
	}
	else
	{
		pbstring key = ci->pArgs->GetAt(0)->GetString();
		//pbstring data = ci->pArgs->GetAt(1)->GetString();
		LPCTSTR tszKey = m_pSession->GetString(key);
		//LPCTSTR tszData = m_pSession->GetString(data);

		//convert the key into ansi
		int iKeyLen = wcstombs(NULL, (LPWSTR)tszKey, 0) + 1;
		LPSTR ansiKey = (LPSTR)malloc(iKeyLen);				//ne pas oublier le free à la fermeture
		wcstombs(ansiKey, (LPWSTR)(LPWSTR)tszKey, iKeyLen);


		////alloc a copy for the string 
		//int iDataLen = (wcslen(tszData) + 1) * sizeof(WCHAR);
		//void * memdata = malloc(iDataLen);				//ne pas oublier le free à la fermeture
		//memcpy(memdata, tszData, iDataLen);
		IPB_Value* data = m_pSession->AcquireValue(ci->pArgs->GetAt(1));

		if (HI_ERR_SUCCESS == hi_insert(m_hi_handle, ansiKey, strlen(ansiKey), data /*tszData*/))
			ci->returnValue->SetBool(true);
		else
			ci->returnValue->SetBool(false);
	}

	return pbxr;
}

PBXRESULT PbniHash::Get(PBCallInfo *ci)
{
	PBXRESULT	pbxr = PBX_OK;
	int iRet;
	void *data_ptr;

	if ( ci->pArgs->GetAt(0)->IsNull())
	{
		// if any of the passed arguments is null, return the null value
		ci->returnValue->SetToNull();
	}
	else
	{
		pbstring key = ci->pArgs->GetAt(0)->GetString();
		LPCTSTR tszKey = m_pSession->GetString(key);

		//convert the key into ansi
		int iKeyLen = wcstombs(NULL, (LPWSTR)tszKey, 0) + 1;
		LPSTR ansiKey = (LPSTR)malloc(iKeyLen);
		wcstombs(ansiKey, (LPWSTR)(LPWSTR)tszKey, iKeyLen);		
		
		//search the key
		iRet = hi_get_str(m_hi_handle, ansiKey, (void**)&data_ptr);
		if (HI_ERR_SUCCESS == iRet)
			//ci->returnValue->SetString((LPCWSTR)data_ptr);
			m_pSession->SetValue(ci->returnValue, (IPB_Value*)data_ptr);
		else
			ci->returnValue->SetToNull();
	}
	return pbxr;
}

PBXRESULT PbniHash::Remove(PBCallInfo *ci)
{
	PBXRESULT	pbxr = PBX_OK;
	int iRet;
	void *data_ptr;

	if ( ci->pArgs->GetAt(0)->IsNull())
	{
		// if any of the passed arguments is null, return the null value
		ci->returnValue->SetToNull();
	}
	else
	{
		pbstring key = ci->pArgs->GetAt(0)->GetString();
		LPCTSTR tszKey = m_pSession->GetString(key);

		//convert the key into ansi
		int iKeyLen = wcstombs(NULL, (LPWSTR)tszKey, 0) + 1;
		LPSTR ansiKey = (LPSTR)malloc(iKeyLen);
		wcstombs(ansiKey, (LPWSTR)(LPWSTR)tszKey, iKeyLen);		
		
		//search the key
		iRet = hi_remove(m_hi_handle, ansiKey, strlen(ansiKey),(void**)&data_ptr);
		m_pSession->ReleaseValue((IPB_Value*)data_ptr);
		if (HI_ERR_SUCCESS == iRet)
			//ci->returnValue->SetString((LPCWSTR)data_ptr);
			ci->returnValue->SetBool(true);
		else
			ci->returnValue->SetBool(false);
	}
	return pbxr;
}

