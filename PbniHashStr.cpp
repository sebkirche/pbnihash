// PbniHashStr.cpp : PBNI class
#include "PbniHashStr.h"


// default constructor
PbniHashStr::PbniHashStr()
{
}

PbniHashStr::PbniHashStr( IPB_Session * pSession )
:m_pSession( pSession )
{
	int iRet = hi_init_str(&m_hi_handle, TABLE_STR_SIZE);
	if (HI_ERR_SUCCESS != iRet)
		exit(-1);
}

// destructor
PbniHashStr::~PbniHashStr()
{
	hi_fini(m_hi_handle);
}

// method called by PowerBuilder to invoke PBNI class methods
PBXRESULT PbniHashStr::Invoke
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
		default:
			pbxr = PBX_E_INVOKE_METHOD_AMBIGUOUS;
	}
	return pbxr;
}


void PbniHashStr::Destroy() 
{
   delete this;
}

// Method callable from PowerBuilder
PBXRESULT PbniHashStr::Hello( PBCallInfo * ci )
{
	PBXRESULT	pbxr = PBX_OK;

	// return value
	ci->returnValue->SetString( _T("Hello from PbniHashStr") );

	return pbxr;
}

PBXRESULT PbniHashStr::Add( PBCallInfo * ci )
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
		pbstring data = ci->pArgs->GetAt(1)->GetString();
		LPCTSTR tszKey = m_pSession->GetString(key);
		LPCTSTR tszData = m_pSession->GetString(data);

		//convert the key into ansi
		int iKeyLen = wcstombs(NULL, (LPWSTR)tszKey, 0) + 1;
		LPSTR ansiKey = (LPSTR)malloc(iKeyLen);				//ne pas oublier le free à la fermeture
		wcstombs(ansiKey, (LPWSTR)(LPWSTR)tszKey, iKeyLen);

		//alloc a copy for the string 
		int iDataLen = (wcslen(tszData) + 1) * sizeof(WCHAR);
		void * memdata = malloc(iDataLen);				//ne pas oublier le free à la fermeture
		memcpy(memdata, tszData, iDataLen);

		if (HI_ERR_SUCCESS != hi_insert_str(m_hi_handle, ansiKey, memdata))
			ci->returnValue->SetBool(false);
	}

	return pbxr;
}

bool PbniHashStr::ToAnsi(LPCWSTR inStr, LPSTR outStr)
{
   int		rc, len;
   TCHAR		*szw = L"null";

   if (inStr != NULL)
   {
      szw = (TCHAR*)inStr;
      len = (int)_tcslen(szw);
      rc = WideCharToMultiByte(
         CP_ACP, 
         0, 
         (LPCWSTR)szw, 
         len,
         outStr, 
         len,
         NULL,
         NULL);
      outStr[len] = NULL;
   }
   return (rc > 0);
}

bool PbniHashStr::ToUnicode(LPCSTR inStr, LPWSTR outStr)
{
   int	rc = 0, len;
   char	*sz = "null";

   if (inStr != NULL)
   {
      sz = (char*)inStr;
      len = (int)((strlen(sz) + 1) * 2);
      MultiByteToWideChar(
         CP_ACP,
         0,
         sz,
         len,
         outStr,   
         len/sizeof(outStr[0]));
   }

   return (rc > 0);
}