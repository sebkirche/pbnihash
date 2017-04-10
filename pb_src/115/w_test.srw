$PBExportHeader$w_test.srw
forward
global type w_test from window
end type
type cb_2 from commandbutton within w_test
end type
type cbx_arrays from checkbox within w_test
end type
type cb_displayhash from commandbutton within w_test
end type
type cb_populate2 from commandbutton within w_test
end type
type cb_testserialize from commandbutton within w_test
end type
type cb_testgetters from commandbutton within w_test
end type
type cb_populate from commandbutton within w_test
end type
type cb_getvalues from commandbutton within w_test
end type
type lb_values from listbox within w_test
end type
type cb_set from commandbutton within w_test
end type
type lb_keys from listbox within w_test
end type
type cb_purge from commandbutton within w_test
end type
type cb_enum from commandbutton within w_test
end type
type st_count from statictext within w_test
end type
type cb_1 from commandbutton within w_test
end type
type cb_obj from commandbutton within w_test
end type
type cb_delete from commandbutton within w_test
end type
type cb_get from commandbutton within w_test
end type
type cb_add from commandbutton within w_test
end type
type st_2 from statictext within w_test
end type
type st_1 from statictext within w_test
end type
type sle_key from singlelineedit within w_test
end type
type sle_data from singlelineedit within w_test
end type
type gb_1 from groupbox within w_test
end type
end forward

global type w_test from window
integer width = 3205
integer height = 1864
boolean titlebar = true
string title = "PbniHash test application"
boolean controlmenu = true
boolean minbox = true
boolean maxbox = true
long backcolor = 67108864
string icon = "C:\dev\powerbuilder\pbni\pbnihash\hash.ico"
boolean center = true
cb_2 cb_2
cbx_arrays cbx_arrays
cb_displayhash cb_displayhash
cb_populate2 cb_populate2
cb_testserialize cb_testserialize
cb_testgetters cb_testgetters
cb_populate cb_populate
cb_getvalues cb_getvalues
lb_values lb_values
cb_set cb_set
lb_keys lb_keys
cb_purge cb_purge
cb_enum cb_enum
st_count st_count
cb_1 cb_1
cb_obj cb_obj
cb_delete cb_delete
cb_get cb_get
cb_add cb_add
st_2 st_2
st_1 st_1
sle_key sle_key
sle_data sle_data
gb_1 gb_1
end type
global w_test w_test

type prototypes
Subroutine OutputDebugString (String lpszOutputString)  LIBRARY "kernel32.dll" ALIAS FOR "OutputDebugStringW";

end prototypes

type variables


uo_hash hash_str
end variables

forward prototypes
public subroutine list_data ()
public subroutine list_data2 ()
public function string stringify (any aa_value)
public subroutine show_count ()
public subroutine show_data (any aa_val)
end prototypes

public subroutine list_data ();string keys[]
ulong i, count
/*
//Method using OF_GETKEYS
if hash_str.of_getkeys(ref keys) then
	count = upperbound(keys)
	lb_keys.reset()
	lb_values.reset()
	for i = 1 to count
		lb_keys.additem(keys[i])
		lb_values.additem(string( hash_str.of_get(keys[i]) ) )
	next
end if
*/

//Method using OF_GETEACH
any data[]
count = hash_str.of_geteach(ref keys[], ref data[])
lb_keys.reset()
lb_values.reset()
for i = 1 to count
	lb_keys.additem(keys[i])
	any la_val
	la_val = data[i]
	string ls_val
	ls_val = stringify( la_val)
	lb_values.additem(ls_val )
next

show_count( )

end subroutine

public subroutine list_data2 ();string keys[]
ulong i, count

//Method using OF_GETKEYS
if hash_str.of_getkeys(ref keys) then
	count = upperbound(keys)
	lb_keys.reset()
	lb_values.reset()
	for i = 1 to count
		lb_keys.additem(keys[i])
		lb_values.additem(stringify( hash_str.of_get(keys[i]) ) )
	next
end if
end subroutine

public function string stringify (any aa_value);if isnull(aa_value) then
	return "{null}"
end if

//It is okay to say 
//	boolean is_array = NOT (lowerbound( la_val ) = -1 AND upperbound( la_val ) = -1)
//	but NOT when you declare your array as : int an_array[ -1 to -1 ]	// which may be really stupid ...
//	int that case, we may do a try / catch to get exception catched!
boolean ib_is_array
long ll_lower,ll_upper
ll_lower = lowerbound( aa_value ) 
ll_upper = upperbound( aa_value ) 
ib_is_array = NOT (ll_lower = -1 AND ll_upper = -1)
if ib_is_array then
	any la_array[]
	string ls_stringified = "["
	long i
	la_array[] = aa_value
	if NOT ( upperbound( aa_value, 2 ) = -1 AND lowerbound( aa_value, 2 ) = -1) then
		//adjust lowerbound / upperbound for multi dim arrays
		long dim = 1, ll_lower_tmp, ll_upper_tmp
		ll_lower = 1
		ll_upper = 1
		do 
			ll_lower_tmp = lowerbound( aa_value, dim )
			ll_upper_tmp = upperbound( aa_value, dim )
			dim ++
			if ll_lower_tmp = -1 AND ll_upper_tmp = -1 then
				exit
			else
				ll_upper *=  ( ll_upper_tmp - ll_lower_tmp + 1 )
			end if
		loop while true
	end if
	for i = ll_lower to ll_upper
		ls_stringified += stringify( la_array[i] )
		if i<>ll_upper then ls_stringified+=", "
	next
	ls_stringified += "]"
	return ls_stringified
end if

string ls_classname
ls_classname = string(classname(aa_value),"[general]")
choose case ls_classname
	case "byte", "blob", "boolean", "char", "string", "dec", "decimal", "double", "real", "integer", "unsignedinteger", "long", &
		"longlong", "unsignedlong", "date", "time", "datetime", &
		"" //an enumerated datatype return "" as classname
		return string( aa_value, "[general]" )
	case else
		//object or structure
		return "{"+upper(ls_classname)+"}"
end choose

return "{?}"
end function

public subroutine show_count ();
st_count.text = string(hash_str.of_getcount())

end subroutine

public subroutine show_data (any aa_val);
if isnull(aa_val) then 
	sle_data.text = 'null : ' + string(hash_str.of_getlasterror( )) + " = " + hash_str.of_getlasterrmsg( )
else
	sle_data.text = string(aa_val)
end if

end subroutine

event open;
hash_str = create uo_hash

//gb_1.text = hash_str.of_hello()

end event

event close;
if isvalid(hash_str) then destroy (hash_str)


end event

on w_test.create
this.cb_2=create cb_2
this.cbx_arrays=create cbx_arrays
this.cb_displayhash=create cb_displayhash
this.cb_populate2=create cb_populate2
this.cb_testserialize=create cb_testserialize
this.cb_testgetters=create cb_testgetters
this.cb_populate=create cb_populate
this.cb_getvalues=create cb_getvalues
this.lb_values=create lb_values
this.cb_set=create cb_set
this.lb_keys=create lb_keys
this.cb_purge=create cb_purge
this.cb_enum=create cb_enum
this.st_count=create st_count
this.cb_1=create cb_1
this.cb_obj=create cb_obj
this.cb_delete=create cb_delete
this.cb_get=create cb_get
this.cb_add=create cb_add
this.st_2=create st_2
this.st_1=create st_1
this.sle_key=create sle_key
this.sle_data=create sle_data
this.gb_1=create gb_1
this.Control[]={this.cb_2,&
this.cbx_arrays,&
this.cb_displayhash,&
this.cb_populate2,&
this.cb_testserialize,&
this.cb_testgetters,&
this.cb_populate,&
this.cb_getvalues,&
this.lb_values,&
this.cb_set,&
this.lb_keys,&
this.cb_purge,&
this.cb_enum,&
this.st_count,&
this.cb_1,&
this.cb_obj,&
this.cb_delete,&
this.cb_get,&
this.cb_add,&
this.st_2,&
this.st_1,&
this.sle_key,&
this.sle_data,&
this.gb_1}
end on

on w_test.destroy
destroy(this.cb_2)
destroy(this.cbx_arrays)
destroy(this.cb_displayhash)
destroy(this.cb_populate2)
destroy(this.cb_testserialize)
destroy(this.cb_testgetters)
destroy(this.cb_populate)
destroy(this.cb_getvalues)
destroy(this.lb_values)
destroy(this.cb_set)
destroy(this.lb_keys)
destroy(this.cb_purge)
destroy(this.cb_enum)
destroy(this.st_count)
destroy(this.cb_1)
destroy(this.cb_obj)
destroy(this.cb_delete)
destroy(this.cb_get)
destroy(this.cb_add)
destroy(this.st_2)
destroy(this.st_1)
destroy(this.sle_key)
destroy(this.sle_data)
destroy(this.gb_1)
end on

type cb_2 from commandbutton within w_test
integer x = 69
integer y = 1124
integer width = 402
integer height = 112
integer taborder = 80
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "Stringify test"
end type

event clicked;string ls_scalar = 'I`m a scalar'
string ls_array[] = { 'first item', 'second item' }
string ls_hack_arrary[ -1 to -1 ] = { 'hiden array' }
string ls_multidim_array[ -1 to 1, 3 to 4] = { '1:1', '1:2', '1:3', '2:1', '2:2', '2:3' }

any la_vars[]
la_vars[ 1 ] = ls_scalar
la_vars[ 2 ] = ls_array[]
la_vars[ 3 ] = ls_hack_arrary[]
la_vars[ 4 ] = ls_multidim_array[]
string ls_res
int i
for i = 1 to upperbound( la_vars[] )
	ls_res += 'la_vars['+string( i ) + '] = ' + stringify( la_vars[i] ) + "~r~n"
next

messagebox( "Stringify arrays test", ls_res )
end event

type cbx_arrays from checkbox within w_test
integer x = 1216
integer y = 72
integer width = 402
integer height = 68
integer textsize = -8
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
long textcolor = 33554432
long backcolor = 67108864
string text = "with arrays"
end type

type cb_displayhash from commandbutton within w_test
integer x = 69
integer y = 996
integer width = 402
integer height = 112
integer taborder = 70
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "Display hash"
end type

event clicked;Display_hash( hash_str, 0 )
end event

type cb_populate2 from commandbutton within w_test
integer x = 1211
integer y = 148
integer width = 402
integer height = 112
integer taborder = 20
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "populate²"
end type

event clicked;hash_str.of_add( "a string", "string value")

int intval = - 42
hash_str.of_add( "an int", intval)

long longval = - 42000
hash_str.of_add( "an long", longval)

double doubleval= 654564.0212
hash_str.of_add( "a double", doubleval)
// * date, datetime, time, real, uint, ulong, longlong, char, byte, blob, decimal
date dateval = today() //compile day !
hash_str.of_add( "a date", dateval)
time timeval = now() //compile time!
hash_str.of_add( "a time", timeval)
datetime datetimeval = datetime(today(),now()) //compile day/time !
hash_str.of_add( "a datetime", datetimeval)
real realval = 42.321
hash_str.of_add( "a real", realval)
uint uintval = 42
hash_str.of_add( "an uint", uintval)
ulong ulongval = 42000
hash_str.of_add( "an ulong", ulongval)
dec decval= 123456.789
hash_str.of_add( "a decimal", decval)
longlong longlongval = 140000000002
hash_str.of_add( "an longlong", longlongval)
char charval = '!'
hash_str.of_add( "a char", charval )
byte byteval = 127
hash_str.of_add( "a byte", byteval )
blob blobval
blobval = blob( "<TestBlob>")
hash_str.of_add( "a blob", blobval )

uo_hash lh_current	
lh_current = create uo_hash
lh_current.of_add( "k1", 1)
lh_current.of_add( "k2", 2)
lh_current.of_add( "k3str", "3")
//TODO: must avoid/handle recursive inclusion ! (to not enter in an infinite loop)
//lh_current.of_add( "k4:uo_hash=parent", hash_str)
hash_str.of_add( "an uo_hash", lh_current)
//enumeration : buggy
keycode keyval = KeyEnter!
hash_str.of_add( "a keycode", keyval)

// * null value
string ls_null
setnull(ls_null)
boolean lb_added
lb_added = hash_str.of_add( "a null string", ls_null)
// * other objects than uo_hash but implementing "event blob on_serialize( )" and "event boolean on_unserialise( blob )"

// * structure : must not work (yet)

// * arrays ( string array, int array, any array { containg other arrays and so on ! } )
if cbx_arrays.checked then
	string ls_str_array[] = { "str1", "str2" }
	hash_str.of_add( "string array", ls_str_array[] )
	any ls_str_array2[] = { "str1", "str2" }
	ls_str_array2[upperbound(ls_str_array2[])+1] = {"child1","child2"}
	hash_str.of_add( "string array²", ls_str_array2[] )
	string ls_str_array3[2,3] = {"1.1","1.2","2.1","2.2","3.1","3.2"}
	hash_str.of_add( "string array[2,3]", ls_str_array3[] )	
//Test Flattened access for multi dimensionnal arrays (but requiere a copy)
//	string tmp, ls_accessor[]
//	int i
//	ls_accessor[] = ls_str_array3[]
//	for i = 1 to 6
//		tmp = ls_accessor[i]
//	next	
end if

list_data( )
end event

type cb_testserialize from commandbutton within w_test
integer x = 969
integer y = 1000
integer width = 443
integer height = 112
integer taborder = 60
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "test serialize"
end type

event clicked;blob lbl_serialized

lbl_serialized = hash_str.of_serialize( )
//Messagebox( "serialize", "Size: " + string( len( lbl_serialized ) ) )
display_blob( lbl_serialized )

if Messagebox( "Test next ?", "Do you want to test unserialize ?", Question!, YesNo!, 2) = 1 then
	hash_str.of_purge( )
	list_data( )
	boolean lb_ok
	lb_ok = hash_str.of_unserialize( lbl_serialized )
	messagebox( "Unzerialize", "Returns: " + string( lb_ok ) )
	list_data( )
end if

end event

type cb_testgetters from commandbutton within w_test
integer x = 969
integer y = 868
integer width = 443
integer height = 112
integer taborder = 50
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "test getters"
end type

event clicked;long ll_start, ll_end_1, ll_end_2
lb_keys.setredraw(false)
lb_values.setredraw(false)
ll_start = cpu()
list_data( )	//Method of_geteach
ll_end_1 = cpu()
list_data2( )	//Method of_getkeys + of_get
ll_end_2 = cpu()
lb_keys.setredraw(true)
lb_values.setredraw(true)
Messagebox( "benchmark", &
	"list_data( )	//Method of_geteach~r~n~t"+string( ll_end_1 - ll_start )+" ms~r~n" +&
	"list_data2( )	//Method of_getkeys + of_get~r~n~t"+string( ll_end_2 - ll_end_1 )+" ms~r~n" )
end event

type cb_populate from commandbutton within w_test
integer x = 791
integer y = 148
integer width = 402
integer height = 112
integer taborder = 10
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "populate"
end type

event clicked;any la_value
string ls_key

long i
for i = 1 to 255
	ls_key = "#"+string(rand(32767))
	la_value = i + rand(32767) * i
	if mod( i, 2 ) = 0 then
		la_value = "str:"+string( la_value )
	end if
	hash_str.of_add( ls_key, la_value )
next

list_data( )
end event

type cb_getvalues from commandbutton within w_test
integer x = 969
integer y = 736
integer width = 443
integer height = 112
integer taborder = 60
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "get values"
end type

event clicked;any la_values[]
ulong count
count = hash_str.of_getvalues( la_values[] )
if count > 0 then
	messagebox("getvalues", "retrieve " + string(count) + " <=> " + string( upperbound( la_values[] ) ) )
end if
end event

type lb_values from listbox within w_test
integer x = 2226
integer y = 68
integer width = 951
integer height = 1672
integer taborder = 50
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
long textcolor = 33554432
boolean hscrollbar = true
boolean vscrollbar = true
boolean sorted = false
borderstyle borderstyle = stylelowered!
end type

type cb_set from commandbutton within w_test
integer x = 498
integer y = 612
integer width = 443
integer height = 112
integer taborder = 40
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "set"
end type

event clicked;if hash_str.of_set(sle_key.text, sle_data.text) then 
	sle_data.text ="+ok"
else
	sle_data.text ="+pas ok :" + string(hash_str.of_getlasterror( )) + " = " + hash_str.of_getlasterrmsg( )
end if

list_data( )

end event

type lb_keys from listbox within w_test
event clicked pbm_lbuttonclk
integer x = 1682
integer y = 68
integer width = 517
integer height = 1676
integer taborder = 20
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
long textcolor = 33554432
boolean vscrollbar = true
boolean sorted = false
borderstyle borderstyle = stylelowered!
end type

event doubleclicked;
string ls_key

ls_key = text(index)
sle_key.text = ls_key
any t
t = hash_str.of_get(ls_key)
show_data(t)

end event

type cb_purge from commandbutton within w_test
integer x = 69
integer y = 612
integer width = 402
integer height = 112
integer taborder = 40
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "purge"
end type

event clicked;
hash_str.of_purge()
list_data( )

end event

type cb_enum from commandbutton within w_test
integer x = 498
integer y = 740
integer width = 443
integer height = 112
integer taborder = 50
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "getKeys/values"
end type

event clicked;
string keys[]
int i

//messagebox("of_getKeys()", string(hash_str.of_getkeys(ref keys)))
if hash_str.of_getkeys(ref keys) then
	sle_data.text ="+ok"
else
	sle_data.text ="+pas ok :" + string(hash_str.of_getlasterror( )) + " = " + hash_str.of_getlasterrmsg( )
end if

list_data( )

end event

type st_count from statictext within w_test
integer x = 1426
integer y = 492
integer width = 242
integer height = 92
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
long textcolor = 33554432
long backcolor = 67108864
string text = "none"
boolean focusrectangle = false
end type

type cb_1 from commandbutton within w_test
integer x = 69
integer y = 740
integer width = 402
integer height = 112
integer taborder = 40
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "debugstring"
end type

event clicked;outputdebugstring( "foo...bar")

end event

type cb_obj from commandbutton within w_test
integer x = 69
integer y = 868
integer width = 402
integer height = 112
integer taborder = 40
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "test add/get"
end type

event clicked;
OutputDebugString("0")
uo_hash hash

hash = create uo_hash
boolean res

str_test val



//OutputDebugString("1")
//OutputDebugString(" ajout 1" + string(res ))
val.l_val = 123
val.s_val = "valeur no 1"
res = hash.of_add( "1111111", val)
messagebox("ajout 1", string(res))

val.l_val = 42
val.s_val = "valeur en deuxieme"
res = hash.of_add( "2222222", val)
messagebox("ajout 2", string(res))

val.l_val = 99
val.s_val = "valeur en 3"
res = hash.of_add( "1111110", val)
messagebox("ajout 3", string(res))

//====================================

any ret_val
str_test val2
ret_val = hash.of_get("1111111")
if isnull(ret_val) then
	messagebox("get 1", "null")
else
	val2 = ret_val
	messagebox("get 1", string(val2.l_val) + " / " + val2.s_val)
end if

ret_val = hash.of_get("1111110")
if isnull(ret_val) then
	messagebox("get 3", "null")
else
	val2 = ret_val
	messagebox("get 3", string(val2.l_val) + " / " + val2.s_val)
end if

ret_val = hash.of_get("123456")
if isnull(ret_val) then
	messagebox("get 4", "null")
else
	val2 = ret_val
	messagebox("get 4", string(val2.l_val) + " / " + val2.s_val)
end if


ret_val = hash.of_get("2222222")
if isnull(ret_val) then
	messagebox("get 2", "null")
else
	val2 = ret_val
	messagebox("get 2", string(val2.l_val) + " / " + val2.s_val)
end if

ret_val = hash.of_get("1111110")
if isnull(ret_val) then
	messagebox("get 3", "null")
else
	val2 = ret_val
	messagebox("get 3", string(val2.l_val) + " / " + val2.s_val)
end if


if isvalid(hash) then destroy hash

end event

type cb_delete from commandbutton within w_test
integer x = 969
integer y = 484
integer width = 443
integer height = 112
integer taborder = 30
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "delete"
end type

event clicked;
if hash_str.of_delete(sle_key.text) then 
	sle_data.text ="+ok"
else
	sle_data.text ="+pas ok :" + string(hash_str.of_getlasterror( )) + " = " + hash_str.of_getlasterrmsg( )
end if

list_data( )

end event

type cb_get from commandbutton within w_test
integer x = 498
integer y = 484
integer width = 443
integer height = 112
integer taborder = 30
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "get"
end type

event clicked;any t
t = hash_str.of_get(sle_key.text)

show_data(t)

end event

type cb_add from commandbutton within w_test
integer x = 69
integer y = 484
integer width = 402
integer height = 112
integer taborder = 20
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
string text = "add"
end type

event clicked;
if hash_str.of_add(sle_key.text, sle_data.text) then 
	sle_data.text ="+ok"
else
	sle_data.text ="+pas ok :" + string(hash_str.of_getlasterror( )) + " = " + hash_str.of_getlasterrmsg( )
end if

list_data( )

end event

type st_2 from statictext within w_test
integer x = 119
integer y = 272
integer width = 210
integer height = 84
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
long textcolor = 33554432
long backcolor = 67108864
string text = "data"
boolean focusrectangle = false
end type

type st_1 from statictext within w_test
integer x = 114
integer y = 168
integer width = 210
integer height = 84
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
long textcolor = 33554432
long backcolor = 67108864
string text = "key"
boolean focusrectangle = false
end type

type sle_key from singlelineedit within w_test
integer x = 370
integer y = 164
integer width = 402
integer height = 84
integer taborder = 10
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
long textcolor = 33554432
string text = "none"
borderstyle borderstyle = stylelowered!
boolean hideselection = false
end type

type sle_data from singlelineedit within w_test
integer x = 370
integer y = 272
integer width = 1179
integer height = 84
integer taborder = 10
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
long textcolor = 33554432
string text = "none"
borderstyle borderstyle = stylelowered!
boolean hideselection = false
end type

type gb_1 from groupbox within w_test
integer x = 59
integer y = 28
integer width = 1605
integer height = 376
integer taborder = 10
integer textsize = -10
integer weight = 400
fontcharset fontcharset = ansi!
fontpitch fontpitch = variable!
fontfamily fontfamily = swiss!
string facename = "Tahoma"
long textcolor = 33554432
long backcolor = 67108864
end type

