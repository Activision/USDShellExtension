#pragma once

class CUSDPreviewThread
{
public:
	~CUSDPreviewThread();

	bool Init();
	void Term();

	HRESULT LaunchPreviewWindow(HWND hWndParent, LPCTSTR pPathToUsdStage);

	void WaitForExit();

	bool IsValid() const;

private:
	static DWORD WINAPI ThreadProc( _In_ LPVOID lpParameter );

	HANDLE m_hThread = nullptr;
	HWND m_hParent = 0;
	CString m_sPathToUsdStage;
};

inline bool CUSDPreviewThread::IsValid() const
{
	return m_hThread != nullptr;
}