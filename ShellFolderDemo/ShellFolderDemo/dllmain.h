// dllmain.h : Declaration of module class.

class CShellFolderDemoModule : public ATL::CAtlDllModuleT< CShellFolderDemoModule >
{
public :
	DECLARE_LIBID(LIBID_ShellFolderDemoLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_SHELLFOLDERDEMO, "{EBCE065E-82C7-48C5-A633-47F040175790}")
};

extern class CShellFolderDemoModule _AtlModule;
