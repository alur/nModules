
#ifndef FACTORIES_H
#define FACTORIES_H

// Safe way to release objects which require ->Release()
#define SAFERELEASE(x) if (*x != NULL) { (*x)->Release(); *x = NULL; }

namespace Factories {
	HRESULT GetDWriteFactory(LPVOID* ppFactory);
	HRESULT GetD2DFactory(LPVOID* ppFactory);
	HRESULT GetWICFactory(LPVOID* ppFactory);
	void Release();
}

#endif /* FACTORIES_H */
