#ifndef C_PBSERIALIZER_H
#define C_PBSERIALIZER_H

#include <pbext.h>
//#define SHINY_PROFILER FALSE
#if SHINY_PROFILER==TRUE
#pragma message( "**** PROFILING ENABLED ****" )
#include "Shiny.h"
#else
#include <stdio.h>
#define PROFILE_FUNC()	/* nothing */ 
#endif



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

//#define USE_C_FILE
#define USE_MEM

class _data_stream{
public:
	_data_stream(){
#ifdef USE_MEM
		m_buffer = NULL;
		m_cursor = m_size = 0;
#else
	#ifdef USE_C_FILE
		FILE* init_datastream();
	#else
		HANDLE init_datastream();
	#endif
#endif
	}
	~_data_stream(){
		if(m_buffer){
			free(m_buffer);
		}
	}
	DWORD length(){ return m_size; }
	DWORD pos(){ return m_cursor; }
	bool move(DWORD pos){ //Do not enlarge memory allocation
		if(pos <= m_size){
			m_cursor = pos ;
			return true;
		}
		return false;
	}
	DWORD read(void* trgBuffer, DWORD bytesToRead){
		if(trgBuffer && m_cursor + bytesToRead <= m_size ){
			//memcpy(trgBuffer,(void*)(((DWORD)m_buffer) + m_cursor), bytesToRead);
			RtlMoveMemory(trgBuffer,(void*)(((DWORD)m_buffer) + m_cursor), bytesToRead);
			m_cursor+=bytesToRead;
			return bytesToRead;
		}
		return 0;
	}
	DWORD write(void* srcBuffer, DWORD bytesToWrite){
		if(m_buffer==NULL){
			if(m_buffer = malloc(bytesToWrite)){
				m_size = bytesToWrite;
			}
			if(m_size<bytesToWrite){
				return 0;
			}
		}
		if(m_size<bytesToWrite){
			m_buffer = realloc(m_buffer,m_size+bytesToWrite);
			return 0;
		}
		RtlMoveMemory((void*)(((DWORD)m_buffer) + m_cursor),srcBuffer,bytesToWrite);
		m_cursor+=bytesToWrite;
		return bytesToWrite;
	}
private:
#ifdef USE_MEM
	void* m_buffer;
	DWORD m_cursor;
	DWORD m_size;
#else
	#ifdef USE_C_FILE
		FILE*  m_pserialized;
	#else
		HANDLE m_pserialized;
		LPCWSTR m_ptempFileName;
		bool m_pis_temp;
	#endif
#endif
};

class pbserializer{
public:
	pbserializer();
	~pbserializer();
	pbserializer(IPB_Session* session);
	pbserializer(IPB_Session* session, void* serialize_source);
	bool serialize(IPB_Value* to_serialize);
	IPB_Value* unserialize();
	IPB_Value* unserialize_type( pbuint type );
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

	pbgroup find_pbgroup(LPCTSTR classname);
	pbblob get_as_blob();
	bool set_from_blob(pbblob serialized_data);

private:
	IPB_Session* m_pSession;
	_data_stream* m_pserialized;
};

#endif