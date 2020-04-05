/**
 * 
 * schemadef.h
 * 
 * Schema system definitions
 * 
 * 
 */
#pragma once

#include <stdint.h>

class CSchemaInstallCallback
{

};

class CSchemaSystem
{

};

class CSchemaSystemTypeScope 
{

};

class CSchemaType
{
public:
	virtual int32_t virtual0() = 0;
	virtual int32_t virtual1() = 0;
	virtual uint64_t virtual2(uint64_t arg0) = 0;
	virtual int32_t virtual3() = 0;
	virtual int32_t virtual4() = 0;
	virtual int64_t virtual5() = 0;
	// unknown
	virtual uint32_t virtual7() = 0;
	virtual int64_t virtual8(int64_t arg0, int64_t arg1) = 0;
	virtual int64_t virtual9() = 0;
	virtual int64_t virtual10(int64_t arg0, int64_t arg1, int64_t arg2, int64_t arg3) = 0;
	// unknown

};

class CSchemaType_Atomic : public CSchemaType 
{

};

class CSchemaType_Atomic_I : public CSchemaType, CSchemaType_Atomic
{

};

class CSchemaType_Atomic_T : public CSchemaType, CSchemaType_Atomic
{

};

class CSchemaType_Atomic_TT : public CSchemaType, CSchemaType_Atomic, CSchemaType_Atomic_T
{

};

class CSchemaType_Atomic_CollectionOfT : public CSchemaType, CSchemaType_Atomic, CSchemaType_Atomic_T
{

};

class CSchemaType_Bitfield : public CSchemaType
{

};

class CSchemaType_Builtin : public CSchemaType
{

};

class CSchemaType_DeclaredClass : public CSchemaType
{

};

class CSchemaType_DeclaredEnum : public CSchemaType
{

};

class CSchemaType_FixedArray : public CSchemaType 
{

};

class CSchemaType_NoschemaType : public CSchemaType
{

};

class CSchemaType_Ptr : public CSchemaType
{

};

