namespace Factories {
	HRESULT GetDWriteFactory(LPVOID* ppFactory);
	HRESULT GetD2DFactory(LPVOID* ppFactory);
	HRESULT GetWICFactory(LPVOID* ppFactory);
	void Release();
}