$PBExportHeader$hashdemo.sra
$PBExportComments$Generated Application Object
forward
global type hashdemo from application
end type
global transaction sqlca
global dynamicdescriptionarea sqlda
global dynamicstagingarea sqlsa
global error error
global message message
end forward

global variables
string gs_var = "Test hack"
end variables

global type hashdemo from application
string appname = "hashdemo"
end type
global hashdemo hashdemo

on hashdemo.create
appname="hashdemo"
message=create message
sqlca=create transaction
sqlda=create dynamicdescriptionarea
sqlsa=create dynamicstagingarea
error=create error
end on

on hashdemo.destroy
destroy(sqlca)
destroy(sqlda)
destroy(sqlsa)
destroy(error)
destroy(message)
end on

event open;
open(w_test)
end event

