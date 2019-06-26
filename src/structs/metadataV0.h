

struct ArgItemV0
{
	char value[STRING_SIZE];
};

struct FuncCallArgV0
{
	char name[STRING_SIZE];
	char type[STRING_SIZE];
};

struct EventDocumentationV0
{
	char value[STRING_SIZE];
};

struct EventArgV0
{
	char name[STRING_SIZE];
	ArgItemV0 argItem[COLLECTION_SIZE];
	EventDocumentationV0 eventDoc[COLLECTION_SIZE];
};

struct OuterEventV0
{
	char name[STRING_SIZE];
	EventArgV0 eventArg[COLLECTION_SIZE];
};

struct FuncTypeV0
{
	// 0 - plain text, 1 - map type
	unsigned int typeName;
	char key[STRING_SIZE];
	char value[STRING_SIZE];
};

struct FunctionCallArgV0
{
	char name[STRING_SIZE];
	char type[STRING_SIZE];
};

struct FunctionV0
{
	char name[STRING_SIZE];
	char modifier[STRING_SIZE];
	FuncTypeV0 type[STRING_SIZE];
	char fallback[STRING_SIZE];
	char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct FunctionCallV0
{
	int id;
	char name[STRING_SIZE];
	FunctionCallArgV0 args[COLLECTION_SIZE];
	char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct CallV0
{
	char name[STRING_SIZE];
	FunctionCallV0 fn1[COLLECTION_SIZE];
};

struct ModuleContentV0
{
	char name[STRING_SIZE];
	CallV0 call;
};

struct StorageV0
{
	char prefix[STRING_SIZE];
	FunctionV0 function[COLLECTION_SIZE];
};

struct ModuleV0
{
	char prefix[STRING_SIZE];
	ModuleContentV0 module;	
	StorageV0 storage;
};

struct OuterEventWrapperV0 {
	char name[STRING_SIZE];
	unique_ptr<OuterEventV0> eventArg[COLLECTION_SIZE];
};

struct DispachCallV0 {
	char name[STRING_SIZE];
	char prefix[STRING_SIZE];
	int index;
};

struct OuterDispachV0 {
	char name[STRING_SIZE];
	DispachCallV0 call[COLLECTION_SIZE];
};

struct MDV0
{
	unique_ptr<OuterEventWrapperV0> oew;
	unique_ptr<ModuleV0> module[COLLECTION_SIZE];
	unique_ptr<OuterDispachV0> dispach;
};