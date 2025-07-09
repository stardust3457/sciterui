#include <widgets/page_nav.h>
#include <sciter_element.h>
#include <sciter_handler.h>
#include "std_string.h"
#include <map>
#include <memory>
#include <set>

namespace SciterUI
{

class WidgetPageNav :
    public std::enable_shared_from_this<WidgetPageNav>,
    public IWidget,
    public IPageNav,
    public IClickSink,
    public ISciterElementCallback
{
    struct InsensitiveCompare
    {
        bool operator()(const std::string & a, const std::string & b) const
        {
            return _stricmp(a.c_str(), b.c_str()) < 0;
        }
    };

    struct CPage
    {
    public:
        CPage(SCITER_ELEMENT Elem_) :
            Elem(Elem_),
            PageExternal(false)
        {
        }

        SciterElement Elem;
        SciterElement Content;
        std::string PageName;
        std::string PageContents;
        std::string PageLabel;
        bool PageExternal;

    private:
        CPage() = delete;
        CPage(const CPage &) = delete;
        CPage& operator=(const CPage &) = delete;
    };
    typedef std::map<std::string, std::unique_ptr<CPage>, InsensitiveCompare> NavPages;
    typedef std::map<SCITER_ELEMENT, CPage *> ElemPageMap;
    typedef std::map<WidgetPageNav *, std::shared_ptr<WidgetPageNav>> PageNavs;
    typedef std::set<IPagesSink *> IPagesSinkSet;

public:
    static bool Register(ISciterUI & SciterUI);

private:
    WidgetPageNav() = delete;
    WidgetPageNav(const WidgetPageNav &) = delete;
    WidgetPageNav& operator=(const WidgetPageNav &) = delete;

    WidgetPageNav(ISciterUI & SciterUI);
    void HideCurrentPage(CPage * page);
    bool ShouldChangePage(CPage * newPage, CPage * currentPage);
    void TryToShowPage(SCITER_ELEMENT Element);
    void ShowPage(CPage * newPage);

    //IWidget
    void Attached(SCITER_ELEMENT element, IBaseElement * baseElement);
    void Detached(SCITER_ELEMENT element);
    std::shared_ptr<void> GetInterface(const char * riid);

    // ISciterElementCallback
    bool OnSciterElement(SCITER_ELEMENT he);

    // IClickSink
    bool OnClick(SCITER_ELEMENT Element, SCITER_ELEMENT SourceElem, uint32_t EventReason);

    // IPageNav
    std::string GetCurrentPage();
    void AddSink(IPagesSink * sink);
    void RemoveSink(IPagesSink * sink);

    static IWidget* __stdcall CreateWidget(ISciterUI& SciterUI);

    static PageNavs m_instances;

    ISciterUI & m_sciterUI;
    IBaseElement * m_baseElement;
    SciterElement m_pageNavElem;
    IPagesSinkSet m_sinks;
    SciterElement m_targetFrame;
    NavPages m_pages;
    ElemPageMap m_elemPages;
    std::string m_firstPage;
    std::string m_currentPage;
};

WidgetPageNav::PageNavs WidgetPageNav::m_instances;

WidgetPageNav::WidgetPageNav(ISciterUI & sciterUI) :
    m_sciterUI(sciterUI)
{
}

bool WidgetPageNav::Register(ISciterUI & SciterUI)
{
    const char * WidgetCss =
        "PageNav {"
        "    display-model: block-inside;"
        "    display: block;"
        "    behavior: PageNav;"
        "}"
        "Page {"
        "   display-model: block-inside;"
        "   display: block;"
        "}";

    return SciterUI.RegisterWidgetType("PageNav", WidgetPageNav::CreateWidget, WidgetCss);
}

void WidgetPageNav::Attached(SCITER_ELEMENT element, IBaseElement * baseElement)
{
    m_baseElement = baseElement;
    m_pageNavElem = element;
    SciterElement ElementRoot(m_pageNavElem.GetRoot());
    if (ElementRoot.IsValid())
    {
        m_pageNavElem.SelectElements((ISciterElementCallback *)this, "Page");
        m_targetFrame = SciterElement(ElementRoot.GetElementByID(m_pageNavElem.GetAttributeByName("target").c_str()));
        if (m_targetFrame.IsValid())
        {
            if (m_firstPage.length() != 0)
            {
                NavPages::iterator iter = m_pages.find(m_firstPage);
                if (iter != m_pages.end())
                {
                    CPage & Page = *iter->second;
                    TryToShowPage(Page.Elem);
                }
            }
        }
    }
}

void WidgetPageNav::Detached(SCITER_ELEMENT /*Element*/)
{
    NavPages::const_iterator CurrentPageItr = m_pages.find(m_currentPage);
    if (CurrentPageItr != m_pages.end())
    {
        CPage * Page = CurrentPageItr->second.get();
        Page->Content = nullptr;
    }
    m_baseElement = nullptr;
    m_elemPages.clear();
    m_pages.clear();
}

std::shared_ptr<void> WidgetPageNav::GetInterface(const char * riid)
{
    if (strcmp(riid, IID_IPAGENAV) == 0)
    {
        return std::static_pointer_cast<IPageNav>(shared_from_this());
    }
    return nullptr;
}

bool WidgetPageNav::ShouldChangePage(CPage * newPage, CPage * currentPage)
{
    if (newPage == nullptr || currentPage == newPage)
    {
        return false;
    }

    if (currentPage)
    {
        for (IPagesSinkSet::iterator itr = m_sinks.begin(); itr != m_sinks.end(); itr++)
        {
            if (!(*itr)->PageNavChangeFrom(currentPage->PageName, m_pageNavElem))
            {
                return false;
            }
        }
    }
    for (IPagesSinkSet::iterator itr = m_sinks.begin(); itr != m_sinks.end(); itr++)
    {
        if (!(*itr)->PageNavChangeTo(newPage->PageName, m_pageNavElem))
        {
            return false;
        }
    }
    return true;
}

void WidgetPageNav::HideCurrentPage(CPage * page)
{
    if (page == nullptr)
    {
        return;
    }
    if (page->Content.IsValid())
    {
        page->Content.Detach();
    }
    m_currentPage.clear();
    page->Elem.SetState(0, SciterElement::STATE_CURRENT | SciterElement::STATE_VISITED, true);
}

void WidgetPageNav::TryToShowPage(SCITER_ELEMENT element)
{
    std::string pageId = SciterElement(element).GetAttribute("data-page_id");
    if (!pageId.empty())
    {
        element = (SCITER_ELEMENT)std::stoull(pageId.c_str(), nullptr, 16);
    }
    ElemPageMap::iterator iter = m_elemPages.find(element);
    if (iter == m_elemPages.end())
    {
        return;
    }
    CPage * currentPage = nullptr;
    CPage * newPage = iter->second;
    if (m_currentPage.length() > 0)
    {
        NavPages::const_iterator CurrentPageItr = m_pages.find(m_currentPage);
        if (CurrentPageItr != m_pages.end())
        {
            currentPage = CurrentPageItr->second.get();
        }
    }

    if (!ShouldChangePage(newPage, currentPage))
    {
        return;
    }
    HideCurrentPage(currentPage);
    ShowPage(newPage);
}

void WidgetPageNav::ShowPage(CPage * page)
{
    if (page->Content.IsValid())
    {
        m_targetFrame.Insert(page->Content, m_targetFrame.GetChildCount());
        page->Content.SetStyleAttribute("display", "block");
    }
    else
    {
        SciterElement& Content = page->Content;
        Content.Create("div", "");
        m_targetFrame.Insert(Content, m_targetFrame.GetChildCount());
        Content.SetStyleAttribute("width", "100%%");
        Content.SetStyleAttribute("height", "100%%");
        if (page->PageExternal)
        {
            m_sciterUI.SetElementHtmlFromResource(page->Content, page->PageContents.c_str());
        }
        else
        {
            Content.SetHTML((const uint8_t*)page->PageContents.c_str(), page->PageContents.length(), SciterElement::SIH_REPLACE_CONTENT);
        }
        m_sciterUI.UpdateWindow(m_pageNavElem.GetElementHwnd(true));
        for (IPagesSinkSet::iterator itr = m_sinks.begin(); itr != m_sinks.end(); itr++)
        {
            (*itr)->PageNavCreatedPage(page->PageName, page->Content);
        }
    }
    m_currentPage = page->PageName;
    page->Elem.SetState(SciterElement::STATE_CURRENT | SciterElement::STATE_VISITED, 0, true);
    for (IPagesSinkSet::iterator itr = m_sinks.begin(); itr != m_sinks.end(); itr++)
    {
        (*itr)->PageNavPageChanged(page->PageName, m_pageNavElem);
    }
}

bool WidgetPageNav::OnSciterElement(SCITER_ELEMENT element)
{
    std::unique_ptr<CPage> Page(new CPage(element));
    if (Page.get() == nullptr || !Page->Elem.IsValid())
    {
        return false;
    }
    Page->PageName = Page->Elem.GetAttributeByName("name");
    if (Page->PageName.length() == 0)
    {
        Page->PageName = stdstr_f("Page_%d", m_pages.size() + 1);
    }
    Page->PageLabel = Page->Elem.GetAttributeByName("label");
    Page->PageContents = Page->Elem.GetAttributeByName("contents");
    Page->PageExternal = true;
    if (Page->PageContents.length() == 0)
    {
        Page->PageExternal = false;
        Page->PageContents = Page->Elem.GetHTML(false);
    }
    Page->Elem.SetHTML((const uint8_t *)Page->PageLabel.c_str(), (int)Page->PageLabel.length(), SciterElement::SIH_REPLACE_CONTENT);
    Page->Elem.SetAttribute("data-page_id", stdstr_f("%llx", (SCITER_ELEMENT)Page->Elem).c_str());
    m_sciterUI.AttachHandler(Page->Elem, IID_ICLICKSINK, (IClickSink *)this);
    std::pair<NavPages::iterator, bool> Item = m_pages.insert(NavPages::value_type(Page->PageName, std::move(Page)));
    if (Item.second)
    {
        m_elemPages.insert(ElemPageMap::value_type(element, Item.first->second.get()));
        if (m_firstPage.empty())
        {
            m_firstPage = Item.first->second->PageName;
        }
    }
    return false;
}

bool WidgetPageNav::OnClick(SCITER_ELEMENT Element, SCITER_ELEMENT /*SourceElem*/, uint32_t /*EventReason*/)
{
    TryToShowPage(Element);
    return false;
}

std::string WidgetPageNav::GetCurrentPage()
{
    return m_currentPage;
}

void WidgetPageNav::AddSink(IPagesSink * sink)
{
    m_sinks.insert(sink);
    for (NavPages::const_iterator itr = m_pages.begin(); itr != m_pages.end(); itr++)
    {
        const CPage * Page = itr->second.get();
        if (Page->Content.IsValid())
        {
            sink->PageNavCreatedPage(Page->PageName, Page->Content);
        }
    }
}

void WidgetPageNav::RemoveSink(IPagesSink * Sink)
{
    IPagesSinkSet::iterator itr = m_sinks.find(Sink);
    if (itr != m_sinks.end())
    {
        m_sinks.erase(itr);
    }
}

IWidget * __stdcall WidgetPageNav::CreateWidget(ISciterUI & sciterUI)
{
    std::shared_ptr<WidgetPageNav> instance(new WidgetPageNav(sciterUI));
    IWidget* widget = (IWidget*)instance.get();
    WidgetPageNav * pagenav = instance.get();
    m_instances.insert(PageNavs::value_type(pagenav, std::move(instance)));
    return widget;
}

}

bool Register_WidgetPageNav(ISciterUI& sciterUI)
{
    return SciterUI::WidgetPageNav::Register(sciterUI);
}