// PbniHash.cpp : PBNI class
#define _CRT_SECURE_NO_DEPRECATE

#include "PbniHash.h"

#ifdef _DEBUG
#define	VERSION_STR	_T(" (Debug version - ") _T(__DATE__) _T(" ") _T(__TIME__) _T(")")
#else
#define	VERSION_STR	_T(" (Release version - ") _T(__DATE__) _T(" ") _T(__TIME__) _T(")")
#endif

// default constructor
PbniHash::PbniHash()
{
}

PbniHash::PbniHash( IPB_Session * pSession )
:m_pSession( pSession )
{
	struct hi_init_set hi_set;

	m_lastError = HI_ERR_SUCCESS;
	hi_set_zero(&hi_set);
	hi_set_bucket_size(&hi_set, TABLE_SIZE);
	hi_set_hash_alg(&hi_set, HI_HASH_DEFAULT);
	hi_set_coll_eng(&hi_set, COLL_ENG_LIST);
	//hi_set_key_cmp_func(&hi_set, hi_cmp_int32);
	hi_set_key_cmp_func(&hi_set, hi_cmp_str);

	hi_create(&m_hi_handle, &hi_set);	//todo: check error
}

// destructor
PbniHash::~PbniHash()
{
	DoPurge();
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
		case mid_Count:
			pbxr = this->Count(ci);
			break;
		case mid_GetKeys:
			pbxr = this ->GetKeys(ci);
			break;
		case mid_GetLastError:
			pbxr = this ->GetLastErr(ci);
			break;
		case mid_GetLastErrMsg:
			pbxr = this ->GetLastErrMsg(ci);
			break;
		/*case mid_UseStrCompare:
			pbxr = this ->GetKeys(ci);//!!!!
			break;*/
		case mid_Purge:
			pbxr = this->Purge(ci);
			break;
		case mid_Set:
			pbxr = this->Set(ci);
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
	ci->returnValue->SetString( _T("Hello from PbniHash") VERSION_STR );

	return pbxr;
}


PBXRESULT PbniHash::Add( PBCallInfo * ci )
{
	PBXRESULT	pbxr = PBX_OK;
	int hi_res;
	int keyLen;

	// check arguments
	if ( ci->pArgs->GetAt(0)->IsNull() || ci->pArgs->GetAt(1)->IsNull() )
	{
		// if any of the passed arguments is null, return the null value
		ci->returnValue->SetToNull();
	}
	else
	{
		pbstring key = ci->pArgs->GetAt(0)->GetString();
		LPCTSTR tszKey = m_pSession->GetString(key);

		//ce qui suit est identique a faire un AcquireValue sur la key...
		//wchar_t *localKey = (wchar_t *)malloc((wcslen(tszKey) + 1) * sizeof(wchar_t));
		//wcscpy(localKey, tszKey);
		keyLen = wcstombs(NULL, (LPWSTR)tszKey, 0) + 2;
		char *localKey = (char*)malloc(keyLen);
		wcstombs(localKey, (LPWSTR)tszKey, keyLen);

		IPB_Value *data = m_pSession->AcquireValue(ci->pArgs->GetAt(1));

		PPBDATAREC dataRecord = (PPBDATAREC)malloc(sizeof(PBDATAREC));
		dataRecord->key = localKey;
		dataRecord->data = data;
		
		hi_res = hi_insert(m_hi_handle, (void*)dataRecord->key, strlen(localKey), dataRecord /*tszData*/);
		if (HI_SUCCESS == hi_res)
			ci->returnValue->SetBool(true);
		else
			ci->returnValue->SetBool(false);
		//TODO: need to tell if HI_ERR_DUPKEY
		m_lastError = hi_res;
	}
	return pbxr;
}

PBXRESULT PbniHash::Get(PBCallInfo *ci)
{
	PBXRESULT	pbxr = PBX_OK;
	int iRet;
	PPBDATAREC data_ptr;

	if ( ci->pArgs->GetAt(0)->IsNull())
	{
		// if any of the passed arguments is null, return the null value
		ci->returnValue->SetToNull();
	}
	else
	{
		pbstring key = ci->pArgs->GetAt(0)->GetString();
		LPCTSTR tszKey = m_pSession->GetString(key);
		int keyLen = wcstombs(NULL, (LPWSTR)tszKey, 0) + 2;
		char *localKey = (char*)malloc(keyLen);
		wcstombs(localKey, (LPWSTR)tszKey, keyLen);

		//search the key
		iRet = hi_get(m_hi_handle, (void*)localKey, strlen(localKey), (void**)&data_ptr);
		if (HI_SUCCESS == iRet)
			m_pSession->SetValue(ci->returnValue, (IPB_Value*)data_ptr->data);
		else
			ci->returnValue->SetToNull();
		free(localKey);
		m_lastError = iRet;
	}
	return pbxr;
}

PBXRESULT PbniHash::Set(PBCallInfo *ci)
{
	PBXRESULT	pbxr = PBX_OK;
	int iRet;
	PPBDATAREC data_ptr;

	if ( ci->pArgs->GetAt(0)->IsNull() || ci->pArgs->GetAt(1)->IsNull() )
	{
		// if any of the passed arguments is null, return the null value
		ci->returnValue->SetToNull();
	}
	else
	{
		pbstring key = ci->pArgs->GetAt(0)->GetString();
		LPCTSTR tszKey = m_pSession->GetString(key);
		int keyLen = wcstombs(NULL, (LPWSTR)tszKey, 0) + 2;
		char *localKey = (char*)malloc(keyLen);
		wcstombs(localKey, (LPWSTR)tszKey, keyLen);

		//search the key
		iRet = hi_get(m_hi_handle, (void*)localKey, strlen(localKey), (void**)&data_ptr);
		if (HI_SUCCESS == iRet)
		{
			//exists, so we can free the existing data and replace by our new one
			m_pSession->ReleaseValue((IPB_Value*)data_ptr->data);
			IPB_Value *data = m_pSession->AcquireValue(ci->pArgs->GetAt(1));
			data_ptr->data = data;
			ci->returnValue->SetBool(true);
			free(localKey);
		}
		else
		{
			IPB_Value *data = m_pSession->AcquireValue(ci->pArgs->GetAt(1));
			PPBDATAREC dataRecord = (PPBDATAREC)malloc(sizeof(PBDATAREC));
			dataRecord->key = localKey;
			dataRecord->data = data;
			iRet = hi_insert(m_hi_handle, (void*)dataRecord->key, strlen(localKey), dataRecord /*tszData*/);
			if (HI_SUCCESS == iRet)
				ci->returnValue->SetBool(true);
			else
				ci->returnValue->SetBool(false);
		}
		m_lastError = iRet;
	}
	return pbxr;
}

PBXRESULT PbniHash::Remove(PBCallInfo *ci)
{
	PBXRESULT	pbxr = PBX_OK;
	int iRet;
	PPBDATAREC data_ptr;

	if ( ci->pArgs->GetAt(0)->IsNull())
	{
		// if any of the passed arguments is null, return the null value
		ci->returnValue->SetToNull();
	}
	else
	{
		pbstring key = ci->pArgs->GetAt(0)->GetString();
		LPCTSTR tszKey = m_pSession->GetString(key);
		int keyLen = wcstombs(NULL, (LPWSTR)tszKey, 0) + 2;
		char *localKey = (char*)malloc(keyLen);
		wcstombs(localKey, (LPWSTR)tszKey, keyLen);
		
		//search the key
		iRet = hi_remove(m_hi_handle, (void*)localKey, strlen(localKey),(void**)&data_ptr);
		if (HI_SUCCESS == iRet)
		{
			free(data_ptr->key);
			m_pSession->ReleaseValue((IPB_Value*)data_ptr->data);
			free(data_ptr);
			ci->returnValue->SetBool(true);
		}
		else
			ci->returnValue->SetBool(false);
		free(localKey);
		m_lastError = iRet;
	}
	return pbxr;
}

PBXRESULT PbniHash::Count(PBCallInfo *ci)
{
	PBXRESULT	pbxr = PBX_OK;
	pbulong ulRet = m_hi_handle->no_objects;
	ci->returnValue->SetUlong(ulRet);

	return pbxr;
}

PBXRESULT PbniHash::GetLastErr(PBCallInfo *ci)
{
	PBXRESULT	pbxr = PBX_OK;
	//pblong lRet = m_lastError;
	ci->returnValue->SetLong(m_lastError);

	return pbxr;
}

PBXRESULT PbniHash::GetLastErrMsg(PBCallInfo *ci)
{
	PBXRESULT	pbxr = PBX_OK;
	const char *msg;

	msg = hi_strerror(m_lastError);
	int msgLen = mbstowcs(NULL, msg, strlen(msg)+1);
	LPWSTR wmsg = (LPWSTR)malloc((msgLen+1) * sizeof(wchar_t));
	mbstowcs(wmsg, msg, strlen(msg)+1);
	ci->returnValue->SetString(wmsg);
	free(wmsg);
	return pbxr;
}


PBXRESULT PbniHash::GetKeys(PBCallInfo *ci)
{
	PBXRESULT pbxr = PBX_OK;

	if(ci->pArgs->GetAt(0)->IsNull() || !ci->pArgs->GetAt(0)->IsArray() || !ci->pArgs->GetAt(0)->IsByRef())
	{
		//there must be one reference to an array
		ci->returnValue->SetBool(false);
	}
	else
	{
		pblong dim[1] = {1};						//on peut avoir des tableaux avec plusieurs dimensions
													//ici on utilise un tableau à 1 dimension, et on commence à 1
		pbarray keys;
		void *key, *data;
		unsigned long keylen;

		hi_iterator_t *iter;


		if(HI_SUCCESS == hi_iterator_create(m_hi_handle, &iter))
		{
			keys = m_pSession->NewUnboundedSimpleArray(pbvalue_string);
			while(HI_SUCCESS == hi_iterator_getnext(iter, &data, &key, &keylen))
			{
				int keyLen = mbstowcs(NULL, (const char*)key, 0);
				LPWSTR wstr = (LPWSTR)malloc((keyLen+1) * sizeof(wchar_t));
				mbstowcs(wstr, (const char*)key, keyLen + 1);
#ifdef _DEBUG
				OutputDebugString(wstr);
#endif
				m_pSession->SetStringArrayItem(keys, dim, wstr);
				dim[0]++;		//prochain index
				free(wstr);
			}
			hi_iterator_fini(iter);
			ci->pArgs->GetAt(0)->SetArray(keys);
			ci->returnValue->SetBool(true);
		}
	}

	return pbxr;
}


PBXRESULT PbniHash::Purge(PBCallInfo *ci)
{
	PBXRESULT pbxr = PBX_OK;

	DoPurge();
	return pbxr;
}
 
void PbniHash::DoPurge()
{
	void *key, *data;
	unsigned long keylen;
	int iRet;
	PPBDATAREC data_ptr;
	hi_iterator_t *iter;


	if(HI_SUCCESS == hi_iterator_create(m_hi_handle, &iter))
	{
		while(HI_SUCCESS == hi_iterator_getnext(iter, &data, &key, &keylen))
		{
#ifdef _DEBUG
			OutputDebugStringA((char*)key);
#endif
			iRet = hi_remove(m_hi_handle, key, keylen,(void**)&data_ptr);
			if (HI_SUCCESS == iRet)
			{
				free(data_ptr->key);
				m_pSession->ReleaseValue((IPB_Value*)(data_ptr->data));
				free(data_ptr);
			}
		}
		hi_iterator_fini(iter);
	}
}
