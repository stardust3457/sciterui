#pragma once
#include <memory>
#include <vector>

typedef const void * SCITER_ELEMENT;
typedef const void * HWINDOW;

typedef struct
{
    int32_t x;
    int32_t y;
} SCITER_POINT;

__interface IWindowDestroySink
{
    void OnWindowDestroy(HWINDOW hWnd);
};

__interface IWindowCloseSink
{
    bool OnWindowCloseRequest(HWINDOW hWnd);
};

__interface ISciterWindow
{
    void Show() = 0;
    void CenterWindow() = 0;
    void FixMinSize() = 0;
    HWINDOW GetHandle() const = 0;
    uint32_t GetMinWidth() const = 0;
    uint32_t GetMinHeight(uint32_t width) const = 0;
    SCITER_ELEMENT GetRootElement() const = 0;
    void OnDestroySinkAdd(IWindowDestroySink * Sink) = 0;
    void OnDestroySinkRemove(IWindowDestroySink * Sink) = 0;
    void OnCloseSinkAdd(IWindowCloseSink * Sink) = 0;
    void OnCloseSinkRemove(IWindowCloseSink * Sink) = 0;
    bool Destroy() = 0;
    void RunModal() = 0;
    bool IsClosed() const = 0;
};

__interface IBaseElement
{
    std::shared_ptr<void> GetInterface(const char * riid) = 0;
};

__interface IWidget
{
    void Attached(SCITER_ELEMENT element, IBaseElement * baseElement) = 0;
    void Detached(SCITER_ELEMENT element) = 0;
    std::shared_ptr<void> GetInterface(const char * riid) = 0;
};

__interface ISciterUI;
typedef IWidget * (__stdcall * tyCreateWidget)(ISciterUI & SciterUI);
typedef void(__stdcall * tyReleaseWidget)(IWidget * widget);

enum SCITERUI_WINDOW_CREATE_FLAGS {
    SUIW_CHILD = (1 << 0), // child window only, if this flag is set all other flags ignored
    SUIW_ALPHA = (1 << 6), // transparent window ( e.g. WS_EX_LAYERED on Windows )
    SUIW_MAIN = (1 << 7), // main window of the app, will terminate the app on close
    SUIW_POPUP = (1 << 8), // the window is created as topmost window.
    SUIW_ENABLE_DEBUG = (1 << 9), // make this window inspector ready
    SUIW_HIDDEN = (1 << 10),  // Create window hidden, caller must show it explicitly
};

__interface ISciterUI
{
    bool AttachHandler(SCITER_ELEMENT elemHandle, const char * riid, void * pinterface) = 0;
    bool DetachHandler(SCITER_ELEMENT elemHandle, const char * riid, void * pinterface) = 0;
    std::shared_ptr<void> GetElementInterface(SCITER_ELEMENT elemHandle, const char * riid) = 0;
    bool SetElementHtmlFromResource(SCITER_ELEMENT elemHandle, const char * uri) = 0;
    bool LoadResource(const char * uri, std::vector<uint8_t> & data) = 0;
    bool WindowCreate(HWINDOW parent, const char * baseHtml, int x, int y, int width, int height, unsigned int flags, ISciterWindow *& window) = 0;
    void PopupShow(SCITER_ELEMENT hePopup, SCITER_ELEMENT heAnchor, uint32_t placement) = 0;
    void PopupShowAt(SCITER_ELEMENT hePopup, SCITER_POINT pos, uint32_t placement) = 0;
    void PopupHide(SCITER_ELEMENT he) = 0;
    bool RegisterWidgetType(const char * name, tyCreateWidget createWidget, tyReleaseWidget releaseWidget,  const char * widgetCss) = 0;
    void UpdateWindow(HWINDOW hwnd) = 0;
    void Run() = 0;
    void Stop() = 0;
    void Shutdown() = 0;
};

bool SciterUIInit(const char * languageDir, const char * baseLanguage, const char * currentLanguage, bool Console, ISciterUI *& sciterUI);
