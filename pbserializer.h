#ifndef C_PBSERIALIZER_H
#define C_PBSERIALIZER_H

#include <pbext.h>
//#define SHINY_PROFILER FALSE
#if SHINY_PROFILER==TRUE
#pragma message( "**** PROFILING ENABLED ****" )
#endif
#include "Shiny.h"


/*
	Serialize and Unserialize Powerbuilder datatype (simple datatype)
	Copyright (c) 2012, Nicolas Sébastien Pierre-Olivier GEORGES
*/
#if PBVM_VERSION >= 105 
#define HAS_PBBYTE
#endif

#if PBVM_VERSION >= 110 
#define HAS_RELEASE_STRING
#endif

#define USE_C_FILE

class pbserializer{
public:
	pbserializer();
	~pbserializer();
	pbserializer(IPB_Session* session);
	pbserializer(IPB_Session* session, void* serialize_source);
	bool serialize(IPB_Value* to_serialize);
	bool unserialize(IPB_Value* to_unserialize);
	bool unserialize_type( pbuint type, IPB_Value* to_unserialize);
	bool write(DWORD buffersize, void* bufferptr);
	bool read(DWORD buffersize, void* bufferptr);
	pbuint read_type();
	BYTE read_cardinality();	//null, scalar or array
	LPCTSTR read_string();

	void write_type(pbuint type);
	void write_null();
	void write_scalar();
	void write_array();
	void write_object();
	void write_string(LPCTSTR str);

#ifdef USE_C_FILE
	FILE* create_temp_file();
#else
	HANDLE create_temp_file();
#endif

	pbgroup find_pbgroup(LPCTSTR classname);
	pbblob get_as_blob();
	void set_from_blob(pbblob serialized_data);

private:
	IPB_Session* m_pSession;
#ifdef USE_C_FILE
	FILE*  m_pserialized;
#else
	HANDLE m_pserialized;
	LPCWSTR m_ptempFileName;
#endif
	bool m_pis_temp;
	
};

#endif