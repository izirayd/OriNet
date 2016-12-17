#ifndef HSTRING_FUNCTION
#define HSTRING_FUNCTION

#include <stdint.h>
#include <stdlib.h>
#include <typeinfo>
#pragma warning(disable : 4996)

#define TSTR   template <typename TypeVarStr>
#define TDSTR  template <typename TStr1, typename TStr2>

void WCharToChar(char* str1, const wchar_t* wstr2, size_t max);
void CharToWChar(wchar_t* wstr1, const char* str2, size_t max);

TSTR int32_t StrCmp(const TypeVarStr *str1, const TypeVarStr *str2)	{

	while ((*str1 != '\0') && (*str1 == *str2)) {
		str1++;
		str2++;
	}

	return (TypeVarStr)*str1 - (TypeVarStr)*str2;
}

TSTR int32_t StrCmpCopy(const TypeVarStr *str1, const TypeVarStr *str2)	{

	while ((*str1 != '\0') && (*str1 == *str2)) {
		str1++;
		str2++;
	}

	return (TypeVarStr)*str1 - (TypeVarStr)*str2;
}

TSTR uint32_t StrLen(TypeVarStr *str1) {
	uint32_t len = 0;
	while (*str1 != 0x00) {
		str1++;
		len++;
	}
	return len;
}

TSTR void StrCpy(TypeVarStr *str1, const TypeVarStr *str2)	{
	TypeVarStr *TmpStr = str1;
	while ((*TmpStr++ = *str2++) != 0x00);
}

TSTR void StrCat(TypeVarStr *str1, const TypeVarStr *str2)
{
	StrCpy(&str1[StrLen(str1)], str2);
}

TSTR void StrClr(TypeVarStr *Str1, uint32_t Size) {
	for (uint32_t i = 0; i < Size; i++)
		Str1[i] = '\0';
}

TDSTR void ConvertStr(TStr1* str1, const TStr2* str2) {

	if ((StrCmp(typeid(str1).name(), "char *") == 0) && (StrCmp(typeid(str2).name(), "const char *") == 0)) return;
	if ((StrCmp(typeid(str1).name(), "wchar_t *") == 0) && (StrCmp(typeid(str2).name(), "const wchar_t *") == 0)) return;
	if ((StrCmp(typeid(str1).name(), "char * __ptr64") == 0) && (StrCmp(typeid(str2).name(), "char * __ptr64") == 0)) return;
	if ((StrCmp(typeid(str1).name(), "wchar_t * __ptr64") == 0) && (StrCmp(typeid(str2).name(), "const wchar_t * __ptr64") == 0)) return;

	if ((StrCmp(typeid(str1).name(), "char *") == 0) || (StrCmp(typeid(str1).name(), "char * __ptr64") == 0))
		WCharToChar((char *)str1, (wchar_t*)str2, StrLen(str2));
	else
		CharToWChar((wchar_t*)str1, (char *)str2, StrLen(str2));
}

TDSTR void StrCpy(TStr1 *str1, const TStr2 *str2)	{

	uint32_t Len = StrLen(str2);
	if (Len == 0) return;
	TStr1 *tmp = new TStr1[Len + 1];
	ConvertStr(tmp, str2);
	tmp[Len] = NULL;
	StrCpy<TStr1>(str1, tmp);
	str1[Len] = NULL;
	delete[] tmp;
}

TDSTR void StrCat(TStr1 *str1, const TStr2 *str2)	{
	uint32_t Len = StrLen(str2);
	if (Len == 0) return;
	TStr1 *tmp = new TStr1[Len + 1];
	ConvertStr(tmp, str2);
	tmp[Len] = NULL;
	StrCat<TStr1>(str1, tmp);
	delete[] tmp;
}

TDSTR int32_t StrCmp(TStr1 *str1, const TStr2 *str2)  {
	uint32_t Len = StrLen(str2);
	if (Len == 0) return -1;
	TStr1 *tmp = new TStr1[Len + 1];
	tmp[Len] = NULL;
	StrCpy(tmp, str2);
	//ConvertStr(tmp, str2);
	int32_t Result = StrCmpCopy<TStr1>(str1, tmp);
	delete[] tmp;
	return Result;
}


typedef wchar_t ucode_t;
typedef char    acode_t;

TSTR class BaseString
{


  public:
	  void CreateBuffer(uint32_t Size) { 
		  Buffer = new TypeVarStr[Size]; 
		  TypeVarBuffer = GetType(Buffer);

	  }
	  void DeleteBuffer()              { delete[] Buffer; }

	  TypeVarStr *GetBuffer() { return Buffer; }

	  void SetBuffer(acode_t *PointBuffer)
	  {

	  }

  private:

	  enum var_t
	  {
		  unknown = 0,
		  acode = 1,
		  ucode = 2
	  };

	  var_t GetType(TypeVarStr *PointBuffer)
	  {
		  if ((StrCmp(typeid(PointBuffer).name(), "acode_t *") == 0)) return var_t::acode;
		  if ((StrCmp(typeid(PointBuffer).name(), "ucode_t *") == 0)) return var_t::ucode;
		  if ((StrCmp(typeid(PointBuffer).name(), "char * __ptr64") == 0)) return var_t::acode;
		  if ((StrCmp(typeid(PointBuffer).name(), "ucode_t * __ptr64") == 0)) return var_t::ucode;

		  return var_t::unknown;
	  }



	  TypeVarStr *Buffer;
	  var_t       TypeVarBuffer;
};

#endif
