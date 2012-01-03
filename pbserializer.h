#ifndef C_PBSERIALIZER_H
#define C_PBSERIALIZER_H

#include <pbext.h>

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
	HANDLE create_temp_file();

	pbgroup find_pbgroup(LPCTSTR classname);
	pbblob get_as_blob();
	void set_from_blob(pbblob serialized_data);

private:
	IPB_Session* m_pSession;
	HANDLE m_pserialized;
	bool m_pis_temp;
	LPCWSTR m_ptempFileName;
};

#endif