/*
	pberializer v1.1 - Serialize and Unserialize Powerbuilder datatype (simple datatype)
	Copyright (c) 2012, Nicolas Sébastien Pierre-Olivier GEORGES

	Optimization axes:
		- using in memory buffer operations in 
			read(...) 
			write(...)
			get_as_blob(...)
			set_from_blob(...)

		- caching classname -> pbgroup + pbclass + mid for serializer/unserializer
		- use PROFILE_BEGIN .. PROFILE_END in "un/serialize switches"

	Limitations:
	------------
	1) powerobject can be serialized/unserialized under 2 conditions:
		it must implement the events:
			event blob on_serialize()
			event boolean on_unserialize(blob data)
	2) restricted to plateform because of direct datatype dump (think about little/big endians)
	3) structure are not serializable at this time (but could be done)
	4) array values are not implemented yet

*/

#include "pbserializer.h"

/*
use libhashish to manage cache entries ?

struct pbs_cache_entry{
	pbclass cls;
	pbgroup grp;
	pbmethodID serializer;
	pbmethodID unserialize;
};
*/

pbserializer::pbserializer(){
	PROFILE_FUNC();
	m_pSession = NULL;
	m_pserialized = new _data_stream();
}

pbserializer::~pbserializer(){
	PROFILE_FUNC();

	if(m_pserialized)
		delete m_pserialized;
}

pbserializer::pbserializer(IPB_Session* session) : m_pSession(session){
	PROFILE_FUNC();
	m_pserialized = new _data_stream();
}

pbserializer::pbserializer(IPB_Session* session, void* serialize_source){
	PROFILE_FUNC();

	//Allow to create a pbserializer object from an opened stream
	m_pSession = session;
	//TODO: determine if this is a FileName, Blob or a FileHandle !
	m_pserialized = (_data_stream*)serialize_source;
}

bool pbserializer::write(DWORD buffersize, void* bufferptr){
	PROFILE_FUNC();
	if(m_pserialized==NULL)
		m_pserialized = new _data_stream();
	if(m_pserialized==NULL)
		return false;
	return m_pserialized->write(bufferptr,buffersize) == buffersize;
}

void pbserializer::write_type(pbuint type){
	PROFILE_FUNC();

	write(sizeof(pbuint), &type);
}
void pbserializer::write_null(){
	PROFILE_FUNC();

	BYTE local_null = 0;
	write( sizeof(BYTE), &local_null);
}
void pbserializer::write_scalar(){
	PROFILE_FUNC();

	BYTE local_scalar = 1;
	write( sizeof(BYTE), &local_scalar);
}
void pbserializer::write_array(){
	PROFILE_FUNC();

	BYTE local_array = 2;
	write( sizeof(BYTE), &local_array);
}
void pbserializer::write_object(){
	PROFILE_FUNC();

	BYTE local_object = 3;
	write( sizeof(BYTE), &local_object);
}

void pbserializer::write_string(LPCTSTR str){
	PROFILE_FUNC();

	DWORD strlength = (wcslen(str)+1)*sizeof(str[0]);
	write( sizeof(strlength), &strlength);
	write( strlength, (void*)str);
}


bool pbserializer::read(DWORD buffersize, void* bufferptr){
	PROFILE_FUNC();
	return m_pserialized->read(bufferptr,buffersize) == buffersize;
}
pbuint pbserializer::read_type(){
	PROFILE_FUNC();

	pbuint type;
	read( sizeof(pbuint), &type);
	return type;
}
BYTE pbserializer::read_cardinality(){	//null, scalar or array
	PROFILE_FUNC();

	BYTE cardinality = 0;
	read( sizeof(BYTE), (void**)&cardinality);
	return cardinality;
}

//Must call free(str) when string is not used anymore (memory leaks other whise!)
LPCTSTR pbserializer::read_string(){
	PROFILE_FUNC();

	DWORD stringlength;
	read(sizeof(DWORD), (void**)&stringlength);
	void* buffer = malloc( (size_t)stringlength );
	read(stringlength, buffer);
	return (LPCTSTR)buffer;
}
//Serialize form:
//	byte	cardinality : 1 = scalar, 2 = unbounded array, 3 = bounded array; 0 for a null value
//	word	datatype
//	dword	sizeof_data
//	byte[]	data_buffer
bool pbserializer::serialize(IPB_Value* to_serialize){
	PROFILE_FUNC();

	if(to_serialize->IsNull()){
		write_null();
		return true;
	}
	//Special case for Enumerated datatype : make it act as a Int
	if(to_serialize->IsEnum()){
		write_scalar();
		write_type( pbvalue_int );
		pbint local_enum = to_serialize->GetInt();
		write( sizeof(pbint), &local_enum);
		return true;
	}
	if(to_serialize->IsArray()){
		write_array();
/*
		//store dimensions and boundaries		
		pbarray arr = to_serialize->GetArray();
		PBArrayInfo* arrinfo = m_pSession->GetArrayInfo(arr);
		pblong* dims = new pblong[arrinfo->numDimensions];
		pblong idx;
		write( sizeof(pblong), &arrinfo->numDimensions );
		for(idx=1;idx<=arrinfo->numDimensions;idx++){
			dims[idx] = arrinfo->bounds[idx - 1 ].lowerBound;	//really -1 ?
			write( sizeof(pblong), &dims[idx] );
		}
		pbboolean isnull;
		//this is not a cartesian product of dimensions, it doesn't work properly to dump the multidims arrays
		for(idx=arrinfo->numDimensions;idx>0;idx--){
			for(pblong jdx=arrinfo->bounds[idx - 1].lowerBound;jdx<=arrinfo->bounds[idx - 1].upperBound;jdx++){
				dims[idx] = jdx;
				serialize( m_pSession->GetPBAnyArrayItem(arr,dims,isnull) );
			}
			dims[idx] = arrinfo->bounds[idx - 1].lowerBound;	//prepare to restart from begining at this level
		}
		delete dims;
		m_pSession->ReleaseArrayInfo( arrinfo );
		//and store item count, a flattened boundared so a tip like 
		//	"int li_something[ 2 to 10, 5 to 42 ] = li_unbounded[]" 
		//can be applyed
		//Then simply store each item data
		//return true;
*/
		//Currentyle not handled,
		return false;
	}
	
	if(to_serialize->IsObject()){
		//Look if the object implement "ue_serilize() blob" and "ue_unserialize(blob data)",
		//if so, then just serialize it
		write_object();
		pbobject obj = to_serialize->GetObjectW();
		pbclass cls = m_pSession->GetClass( obj );
		pbmethodID mid_serialize = m_pSession->GetMethodID( cls, L"on_serialize", PBRT_EVENT, L"O");
		pbmethodID mid_unserialize = m_pSession->GetMethodID( cls, L"on_unserialize", PBRT_EVENT, L"BO");
		if(mid_serialize != kUndefinedMethodID && mid_unserialize != kUndefinedMethodID){
			//Store object classname
			LPCTSTR classname = m_pSession->GetClassNameW(cls);
			write_string( classname );
#ifdef HAS_RELEASE_STRING
			m_pSession->ReleaseString(classname);
#endif
			//invoke event ue_serialize on object and append it serialisation
			PBCallInfo ci;
			m_pSession->InitCallInfo(cls, mid_serialize, &ci);
			m_pSession->InvokeObjectFunction(obj, mid_serialize, &ci);
			//Serialize returned blob
			serialize( ci.returnValue );
			m_pSession->FreeCallInfo(&ci);
			return true;
		}
		return false;
	}
	write_scalar();
	write_type( to_serialize->GetType());
	switch(to_serialize->GetType()){
		case pbvalue_any:
			{
			//NOT Implemented : how can I know the real datatype behind if I am here ?
			//I may got an other datatype before !
				return false;
			}
			break;
		case pbvalue_blob:
			{
				pbblob blob = to_serialize->GetBlob();
				DWORD size = m_pSession->GetBlobLength(blob);
				write(sizeof(DWORD), &size);
				write( size, m_pSession->GetBlob(blob) );
			}
			break;
		case pbvalue_boolean:
			{	
				pbboolean boolean = to_serialize->GetBool();
				write( sizeof(pbboolean), &boolean);
			}
			break;
#ifdef HAS_PBBYTE
		case pbvalue_byte:
			{
				pbbyte byte = to_serialize->GetByte();
				write( sizeof(pbbyte), &byte);
			}
			break;
#endif
		case pbvalue_char:
			{
				pbchar charvalue = to_serialize->GetChar();
				write( sizeof(pbchar), &charvalue);
			}
			break;
		case pbvalue_date:
			{
				pbint local_year, local_month, local_day;
				m_pSession->SplitDate(to_serialize->GetDate(), &local_year, &local_month, &local_day);
				write( sizeof(pbint), &local_year);
				write( sizeof(pbint), &local_month);
				write( sizeof(pbint), &local_day);
			}
			break;
		case pbvalue_datetime:
			{
				pbint local_year, local_month, local_day, local_hour, local_minute;
				pbdouble local_second;
				m_pSession->SplitDateTime(to_serialize->GetDateTime(), &local_year, &local_month, &local_day, &local_hour, &local_minute, &local_second);
				write( sizeof(pbint), &local_year);
				write( sizeof(pbint), &local_month);
				write( sizeof(pbint), &local_day);
				write( sizeof(pbint), &local_hour);
				write( sizeof(pbint), &local_minute);
				write( sizeof(pbdouble), &local_second);
			}
			break;
		case pbvalue_dec:
			{
				LPCTSTR sec_str = m_pSession->GetDecimalString( to_serialize->GetDecimal() );
				write_string( sec_str );
				m_pSession->ReleaseDecimalString( sec_str );
			}
			break;
		case pbvalue_double:
			{
				pbdouble doublevalue = to_serialize->GetDouble();
				write( sizeof(pbdouble), &doublevalue );
			}
			break;
		case pbvalue_int:
			{
				pbint intvalue = to_serialize->GetInt();
				write( sizeof(pbint), &intvalue );
			}
			break;
		case pbvalue_long:
			{
				pblong longvalue = to_serialize->GetLong();
				write( sizeof(pblong), &longvalue );
			}
			break;
		case pbvalue_longlong:
			{
				pblonglong longlongvalue = to_serialize->GetLongLong();
				write( sizeof(pblonglong), &longlongvalue );
			}
			break;
		case pbvalue_real:
			{
				pbreal realvalue = to_serialize->GetReal();
				write( sizeof(pbreal), &realvalue );
			}
			break;
		case pbvalue_string:
			{
				pbstring pbstr = to_serialize->GetString();
				LPCTSTR str = m_pSession->GetString(pbstr);
				write_string(str);
#ifdef HAS_RELEASE_STRING
				m_pSession->ReleaseString(str);
#endif
			}
			break;
		case pbvalue_time:
			{
				pbint local_hour, local_minute;
				pbdouble local_second;
				m_pSession->SplitTime(to_serialize->GetTime(), &local_hour, &local_minute, &local_second);
				write( sizeof(pbint), &local_hour);
				write( sizeof(pbint), &local_minute);
				write( sizeof(pbdouble), &local_second);
			}
			break;
		case pbvalue_uint:
			{
				pbuint uintvalue = to_serialize->GetUint();
				write( sizeof(pbuint), &uintvalue );
			}
			break;
		case pbvalue_ulong:
			{
				pbulong ulongvalue = to_serialize->GetUlong();
				write( sizeof(pbulong), &ulongvalue );
			}
			break;
		default:
			{
				return false;
			}
			break;
	}
	return true;
}

IPB_Value* pbserializer::unserialize(){
	PROFILE_FUNC();

	IPB_Value* returnedValue = NULL;
	BYTE cardinality = read_cardinality();
	if(cardinality == 0){
		returnedValue = NULL;
	}
	else if(cardinality == 1){
		//scalar
		pbuint type = read_type();
		returnedValue = unserialize_type( type);
	}
	else if(cardinality == 2){
		//array
		//NOT Implemented yet
		returnedValue = NULL;
	}
	else if(cardinality == 3){
		//object
		returnedValue = unserialize_type( 65535);
	}
	return returnedValue;
}
IPB_Value* pbserializer::unserialize_type( pbuint type ){
	PROFILE_FUNC();
	PBXRESULT res;
	pblong ldim[1] = { 1 };
	pbarray larr = NULL;
	IPB_Value* to_unserialize;
	if(type!=65535){
		//create depending on datatype
		larr = m_pSession->NewUnboundedSimpleArray(type);
	}

	switch(type){
		case 65535:	//object hack
			{				
				LPCTSTR classname = read_string();
				pbgroup group = find_pbgroup(classname);
				pbclass cls = m_pSession->FindClass(group, classname);
				pbobject obj = m_pSession->NewObject(cls);
				free((void*)classname);
				if(obj==NULL){
					return NULL;
				}
				//then invoke ue_unserialize with the given blob (still to be read)
				BYTE data_cardinality = read_cardinality();
				if(data_cardinality!=1){
					return NULL;
				}
				pbuint data_type = read_type();
				if(data_type != pbvalue_blob){
					return NULL;
				}
				pbmethodID mid_unserialize = m_pSession->GetMethodID( cls, L"on_unserialize", PBRT_EVENT, L"BO");
				if(mid_unserialize == kUndefinedMethodID){
					return NULL;
				}
				PBCallInfo ci;
				m_pSession->InitCallInfo(cls, mid_unserialize, &ci);
				IPB_Value* value = unserialize_type(data_type);	//must be a blob
				ci.pArgs->GetAt(0)->SetBlob( value->GetBlob() );
				m_pSession->InvokeObjectFunction(obj, mid_unserialize, &ci);
				pbboolean res = ci.returnValue->GetBool();
				m_pSession->FreeCallInfo(&ci);
				m_pSession->ReleaseValue(value);
				//to_unserialize->SetObject(obj);
				larr = m_pSession->NewUnboundedObjectArray(cls);
				res = m_pSession->SetObjectArrayItem(larr, ldim, obj);
				to_unserialize = m_pSession->AcquireArrayItemValue(larr, ldim);
				return to_unserialize;
			}
			break;
		case pbvalue_any:
			{
				//why the hell we goes here ?
				//must never happend, at least in my mind and at this time-point :-p
				m_pSession->SetArrayItemToNull( larr, ldim);
			}
			break;
		case pbvalue_blob:
			{
				DWORD size;
				void* buffer = NULL;
				read(sizeof(DWORD), &size);
				buffer = malloc((size_t)size);
				read(size,buffer);
				pbblob value = m_pSession->NewBlob(buffer, size);
				m_pSession->SetBlobArrayItem( larr, ldim, value);	//to_unserialize->SetBlob( value );
				free(buffer);
			}
			break;
		case pbvalue_boolean:
			{
				pbboolean value;
				read(sizeof(pbboolean), &value);
				m_pSession->SetBoolArrayItem( larr, ldim, value);	//to_unserialize->SetBool( value );
			}
			break;
#ifdef HAS_PBBYTE
		case pbvalue_byte:
			{
				pbbyte value;
				read(sizeof(pbbyte), &value);
				m_pSession->SetByteArrayItem( larr, ldim, value);	//to_unserialize->SetByte( value );
			}
			break;
#endif
		case pbvalue_char:
			{
				pbchar value;
				read(sizeof(value), &value);
				m_pSession->SetCharArrayItem( larr, ldim, value);	//to_unserialize->SetChar( value );
			}
			break;
		case pbvalue_date:
			{
				pbdate value = m_pSession->NewDate();
				pbuint year, month, day;
				read(sizeof(pbuint), &year);
				read(sizeof(pbuint), &month);
				read(sizeof(pbuint), &day);
				m_pSession->SetDate(value, year, month, day);
				m_pSession->SetDateArrayItem( larr, ldim, value);	//to_unserialize->SetDate(value);
			}
			break;
		case pbvalue_datetime:
			{
				pbdatetime value = m_pSession->NewDateTime();
				pbuint year, month, day, hour, minute;
				pbdouble second;
				read(sizeof(pbuint), &year);
				read(sizeof(pbuint), &month);
				read(sizeof(pbuint), &day);
				read(sizeof(pbuint), &hour);
				read(sizeof(pbuint), &minute);
				read(sizeof(pbdouble), &second);
				m_pSession->SetDateTime(value, year, month, day, hour, minute, second);
				m_pSession->SetDateTimeArrayItem( larr, ldim, value);	//to_unserialize->SetDateTime(value);
			}
			break;
		case pbvalue_dec:
			{
				pbdec value = m_pSession->NewDecimal();
				LPCTSTR str = read_string();
				m_pSession->SetDecimal(value, str);
				free((void*)str);
				m_pSession->SetDecArrayItem( larr, ldim, value);	//to_unserialize->SetDecimal(value);
			}
			break;
		case pbvalue_double:
			{
				pbdouble value;
				read(sizeof(value), &value);
				res = m_pSession->SetDoubleArrayItem(larr, ldim, value);	//to_unserialize->SetDouble( value );
			}
			break;
		case pbvalue_int:
			{
				pbint value;
				read(sizeof(value), &value);
				m_pSession->SetIntArrayItem( larr, ldim, value);	//to_unserialize->SetInt( value );
			}
			break;
		case pbvalue_long:
			{
				pblong value;
				read(sizeof(value), &value);
				m_pSession->SetLongArrayItem( larr, ldim, value);	//to_unserialize->SetLong( value );
			}
			break;
		case pbvalue_longlong:
			{
				pblonglong value;
				read(sizeof(value), &value);
				m_pSession->SetLongLongArrayItem( larr, ldim, value);	//to_unserialize->SetLongLong( value );
			}
			break;
		case pbvalue_real:
			{
				pbreal value;
				read(sizeof(value), &value);
				m_pSession->SetRealArrayItem( larr, ldim, value);	//to_unserialize->SetReal( value );
			}
			break;
		case pbvalue_string:
			{
				LPCTSTR str = read_string();
				res = m_pSession->SetStringArrayItem(larr, ldim,str);
				//Does my array is automatically destroy by garbage collector ?
				//Hoping that is it the case !!!
				//Without array + dim it make CRASH pbvm !
				//to_unserialize->SetString(str);
				free((void*)str);
			}
			break;
		case pbvalue_time:
			{
				pbtime value = m_pSession->NewTime();
				pbuint hour, minute;
				pbdouble second;
				read(sizeof(pbuint), &hour);
				read(sizeof(pbuint), &minute);
				read(sizeof(pbdouble), &second);
				m_pSession->SetTime(value, hour, minute, second);
				m_pSession->SetTimeArrayItem( larr, ldim, value);	//to_unserialize->SetTime(value);
			}
			break;
		case pbvalue_uint:
			{
				pbuint value;
				read(sizeof(value), &value);
				m_pSession->SetUintArrayItem( larr, ldim, value);	//to_unserialize->SetUint( value );
			}
			break;
		case pbvalue_ulong:
			{
				pbulong value;
				read(sizeof(value), &value);
				m_pSession->SetUlongArrayItem( larr, ldim, value);	//to_unserialize->SetUlong( value );
			}
			break;
		default:
			{
				m_pSession->SetArrayItemToNull( larr, ldim);
			}
			break;
	}
	to_unserialize = m_pSession->AcquireArrayItemValue(larr, ldim);
	return to_unserialize;
}

pbblob pbserializer::get_as_blob(){
	PROFILE_FUNC();

	void* buffer = NULL;
	pblong length = 0;
	if(m_pserialized){
		buffer = malloc( length = (pblong)m_pserialized->length() );	//but we malloc it, so it is not a good idea!!!
		m_pserialized->read(buffer, length);
	}
	pbblob blob = m_pSession->NewBlob( buffer, length );
	if(buffer){
		free(buffer);
	}
	return blob;
}

bool pbserializer::set_from_blob(pbblob serialized_data){
	PROFILE_FUNC();

	if(m_pserialized==NULL)
		return false;
	m_pserialized->write(m_pSession->GetBlob(serialized_data),m_pSession->GetBlobLength(serialized_data));
	m_pserialized->move(0);
	//Now the object is ready to unserialize what you want !
	return true;
}

pbgroup pbserializer::find_pbgroup(LPCTSTR classname){
	PROFILE_FUNC();
	//ordered list of group
	pbgroup group;
	if(group = m_pSession->FindGroup(classname, pbgroup_userobject))
		return group;
	if(group = m_pSession->FindGroup(classname, pbgroup_window))
		return group;
	if(group = m_pSession->FindGroup(classname, pbgroup_structure))
		return group;
	if(group = m_pSession->FindGroup(classname, pbgroup_menu))
		return group;
	if(group = m_pSession->FindGroup(classname, pbgroup_application))
		return group;
/*
	if(group = m_pSession->FindGroup(classname, pbgroup_proxy))
		return group;
	if(group = m_pSession->FindGroup(classname, pbgroup_function))
		return group;
	if(group = m_pSession->FindGroup(classname, pbgroup_datawindow))
		return group;
*/
	return NULL;
}