// PbniHash.cpp : defines the entry point for the PBNI DLL.
//Global: 0
//Unicode: -1
#include "PbniHash.h"
#include "PbniHashStr.h"
#include "Main.h"


BOOL APIENTRY DllMain
(
   HANDLE hModule,
   DWORD ul_reason_for_all,
   LPVOID lpReserved
)
{	
		
	switch(ul_reason_for_all)
	{	
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:		
	case DLL_PROCESS_DETACH:
		break;
	}
			
	return TRUE;
}

// The description returned from PBX_GetDescription is used by
// the PBX2PBD tool to create pb groups for the PBNI class.
//
// + The description can contain more than one class definition.
// + A class definition can reference any class definition that
//   appears before it in the description.
// + The PBNI class must inherit from a class that inherits from
//   NonVisualObject, such as Transaction or Exception

PBXEXPORT LPCTSTR PBXCALL PBX_GetDescription()
{
   // combined class description
   static const TCHAR classDesc[] = {
      /* PbniHash */
      _T("class uo_hash from nonvisualobject\n") \
      _T("   function string of_getversion()\n") \
      _T("   function boolean of_add(string key, any value)\n") \
      _T("   function any of_get(string key)\n") \
      _T("   function boolean of_delete(string key)\n") \
      _T("   function ulong of_getcount()\n") \
      _T("   function boolean of_getkeys(ref string keys[])\n") \
      _T("   function long of_getlasterror()\n") \
      _T("   function string of_getlasterrmsg()\n") \
	  _T("   subroutine of_purge()\n") \
      _T("   function boolean of_set(string key, any value)\n") \
      _T("end class\n")
      //_T("   subroutine of_usestringkeycompare(boolean usestring)\n") 

	  /* PbniHashStr */
/*
      _T("class uo_hash_str from nonvisualobject\n") \
      _T("   function string of_hello()\n") \
      _T("   function boolean of_add(string key, string value)\n") \
      _T("   function string of_get(string key)\n") \
      _T("   function boolean of_delete(string key)\n") \
      _T("end class\n")
*/
	};

   return (LPCTSTR)classDesc;
}

// PBX_CreateNonVisualObject is called by PowerBuilder to create a C++ class
// that corresponds to the PBNI class created by PowerBuilder.
PBXEXPORT PBXRESULT PBXCALL PBX_CreateNonVisualObject
(
   IPB_Session * session,
   pbobject obj,
   LPCTSTR className,
   IPBX_NonVisualObject ** nvobj
)
{
   // The name must not contain upper case
   if (_tcscmp(className, _T("uo_hash")) == 0)
      *nvobj = new PbniHash(session);
/*
   else if (_tcscmp(className, _T("uo_hash_str")) == 0)
      *nvobj = new PbniHashStr(session);
*/
   return PBX_OK;
}

