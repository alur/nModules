#pragma once

class ExplorerService {
public:
    ExplorerService();
    ~ExplorerService();

    void Start();
    void Stop();

private:
    DWORD m_dwThread;
};
