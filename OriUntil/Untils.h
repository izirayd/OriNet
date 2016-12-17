#ifndef UntilsH
#define UntilsH

#include "../OriKernel/KernelSocket.h"
#include "../OriLng/OriLng.h"

#ifndef CLIENT_REALESE
#include "../OriServer/OriServerClient.h"
#endif


#pragma warning(disable : 4996)
#pragma warning(disable : 4800)
#pragma warning(disable : 4244)

#include "../OriFunctionTable/TableFunction.h"
#include "../OriFunctionTable/TableFiles.h"

#define OriFunc(NameFunction)  \
     void NameFunction(OriServerClient &Client, OriLng &Lng)

#define OriAdd(NameFunction) OriFunc(NameFunction)

#define OriNewConnect(NameFunction) \
     void NameFunction(OriServerClient &Client, NewConnection _null)

#define OriEndConnect(NameFunction) \
     void NameFunction(OriServerClient &Client, EndConnection _null)

typedef bool EndConnection;
typedef char NewConnection;

typedef void(*FuncEndConnection)(OriServerClient &Client, EndConnection _null);
typedef void(*FuncNewConnection)(OriServerClient &Client, NewConnection _null);

#define SIZE_SHA256 65

#define KEY_SESSION "daPOqQNwQ"


typedef void(*DefinedEndType)(OriServerClient &Client);


#endif