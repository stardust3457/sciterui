#include "std_string.h"
#include <map>
#include <memory>
#include <string>
#include <sciter_element.h>
#include <sciter_handler.h>
#include <sciter_ui.h>
#include <set>
#include <widgets/menubar.h>

#include <value.h>
#include <sciter-x-behavior.h>
#include <sciter-x-api.h>

class WidgetMenuBar :
    public std::enable_shared_from_this<WidgetMenuBar>,
    public IWidget,
    public IMenuBar,
    public IClickSink,
    public IKeySink,
    public IEventSink,
    public ISciterElementCallback
{
    typedef std::map<IWidget *, std::shared_ptr<WidgetMenuBar>> MenuBars;
    typedef std::set<IMenuBarSink *> IMenuBarSinkSet;

public:
    static void Register(ISciterUI& sciterUI);

    //IMenuBar
    void SetMenuContent(MenuBarItemList& items) const;
    void AddSink(IMenuBarSink* sink);
    void RemoveSink(IMenuBarSink* sink);

private:
    WidgetMenuBar(ISciterUI& SciterUI);

    WidgetMenuBar(void) = delete;
    WidgetMenuBar(const WidgetMenuBar&) = delete;
    WidgetMenuBar& operator=(const WidgetMenuBar&) = delete;

    // IWidget
    void Attached(SCITER_ELEMENT element, IBaseElement* baseElement);
    void Detached(SCITER_ELEMENT element);
    std::shared_ptr<void>  GetInterface(const char* riid);

    // ISciterElementCallback
    bool OnSciterElement(SCITER_ELEMENT he);

    // IClickSink
    bool OnClick(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t reason);

    // IKeySink
    bool OnKeyDown(SCITER_ELEMENT element, SCITER_ELEMENT item, SciterKeys keyCode, uint32_t keyboardState) override;
    bool OnKeyUp(SCITER_ELEMENT element, SCITER_ELEMENT item, SciterKeys keyCode, uint32_t keyboardState) override;
    bool OnKeyChar(SCITER_ELEMENT element, SCITER_ELEMENT item, SciterKeys keyCode, uint32_t keyboardState) override;
    void SyncMenuMnemonicsAttribute();

    // IEventSink
    bool OnEvent(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t event_code, uint64_t reason) override;

    void HideShownMenuPopup() const;
    void NotifySinksMenuItem(int32_t id, SCITER_ELEMENT item) const;
    static bool ElementIsUnderMainMenuWidget(const SciterElement & hit, const SciterElement & mainMenuElem);

    static std::string MenuItemHtml(const MenuBarItem & item, uint32_t indent);
    static IWidget * __stdcall CreateWidget(ISciterUI & sciterUI);
    static void __stdcall ReleaseWidget(IWidget * widget);

    static MenuBars m_instances;

    ISciterUI & m_sciterUI;
    IBaseElement * m_baseElement;
    SciterElement m_menuBarElem;
    SciterElement m_keySinkRoot;
    IMenuBarSinkSet m_sinks;
    bool m_leftAltDown;
    bool m_rightAltDown;
};

WidgetMenuBar::MenuBars WidgetMenuBar::m_instances;

static const uint32_t kMenuItemClickEvent = (uint32_t)MENU_ITEM_CLICK;
static const uint64_t kClickReasonByKey = (uint64_t)BY_KEY_CLICK;

static std::string PrepareMenuTitleForMnemonics(const char * title)
{
    SciterUI::stdstr t(title ? title : "");
    t.Replace("...", "\xE2\x80\xA6");
    return std::string(t.c_str());
}

static void AppendMenuTitleHtml(const std::string & in, std::string & out, char & accesskey)
{
    accesskey = 0;
    for (size_t i = 0; i < in.size(); )
    {
        const unsigned char c = (unsigned char)in[i];
        if (c == '&' && i + 1 < in.size())
        {
            const unsigned char n = (unsigned char)in[i + 1];
            if (n == '&')
            {
                out += '&';
                i += 2;
                continue;
            }
            if (!accesskey && std::isalnum(n))
            {
                accesskey = (char)std::toupper(n);
                out += "<u>";
                out += (char)n;
                out += "</u>";
                i += 2;
                continue;
            }
            out += "&amp;";
            i += 1;
            continue;
        }
        if (c == '<')
        {
            out += "&lt;";
        }
        else if (c == '>')
        {
            out += "&gt;";
        }
        else
        {
            out += (char)c;
        }
        i += 1;
    }
}

static std::string MenuBarAcceleratorKeyLabel(uint32_t key)
{
    if (key >= (uint32_t)SCITER_KEY_A && key <= (uint32_t)SCITER_KEY_Z)
    {
        char buf[2] = { (char)('A' + (key - (uint32_t)SCITER_KEY_A)), '\0' };
        return std::string(buf);
    }
    if (key >= (uint32_t)SCITER_KEY_0 && key <= (uint32_t)SCITER_KEY_9)
    {
        char buf[2] = { (char)('0' + (key - (uint32_t)SCITER_KEY_0)), '\0' };
        return std::string(buf);
    }
    if (key >= (uint32_t)SCITER_KEY_F1 && key <= (uint32_t)SCITER_KEY_F12)
    {
        return SciterUI::stdstr_f("F%u", (unsigned)(1u + (key - (uint32_t)SCITER_KEY_F1)));
    }
    if (key == (uint32_t)SCITER_KEY_ESCAPE)
    {
        return "Esc";
    }
    if (key == (uint32_t)SCITER_KEY_ENTER)
    {
        return "Enter";
    }
    if (key == (uint32_t)SCITER_KEY_TAB)
    {
        return "Tab";
    }
    if (key == (uint32_t)SCITER_KEY_SPACE)
    {
        return "Space";
    }
    return SciterUI::stdstr_f("%u", (unsigned)key);
}

std::string MenuBarAccelerator::Format() const
{
    if (IsNone())
    {
        return std::string();
    }
    std::string s;
    if (ctrl)
    {
        s += "Ctrl+";
    }
    if (shift)
    {
        s += "Shift+";
    }
    if (alt)
    {
        s += "Alt+";
    }
    s += MenuBarAcceleratorKeyLabel(key);
    return s;
}

MenuBarItem::MenuBarItem(int32_t id, const char * title, MenuBarItemList * subMenu, const MenuBarAccelerator * shortcutAccel)
{
    Reset(id, title, subMenu, shortcutAccel);
}

void MenuBarItem::Reset(int32_t id, const char * title, MenuBarItemList * subMenu, const MenuBarAccelerator * shortcutAccel)
{
    m_id = id;
    m_title = title;
    m_subMenu = subMenu;
    if (shortcutAccel != nullptr)
    {
        m_shortcutAccel = *shortcutAccel;
    }
    else
    {
        m_shortcutAccel = {};
    }
}

int MenuBarItem::ID() const
{
    return m_id;
}
    
const char * MenuBarItem::Title() const
{
    return m_title.c_str();
}

const MenuBarItemList * MenuBarItem::SubMenu() const
{
    return m_subMenu;
}

const MenuBarAccelerator & MenuBarItem::ShortcutAccel() const
{
    return m_shortcutAccel;
}

void WidgetMenuBar::Register(ISciterUI & sciterUI)
{
    const char * WidgetCss =
        "mainmenu {"
        "    display-model: block-inside;"
        "    display: block;"
        "    behavior: MainMenu;"
        "}";
    sciterUI.RegisterWidgetType("MainMenu", WidgetMenuBar::CreateWidget, WidgetMenuBar::ReleaseWidget, WidgetCss);
}

void WidgetMenuBar::SetMenuContent(MenuBarItemList & items) const
{
    std::string html("<ul id=\"menu-bar\">\n");
    for (MenuBarItemList::iterator itr = items.begin(); itr != items.end(); itr++)
    {
        html += MenuItemHtml(*itr, 2);
    }
    html += "</ul>";
    m_menuBarElem.SetHTML((const uint8_t *)html.data(), html.size(), SciterElement::SIH_REPLACE_CONTENT);
}

void WidgetMenuBar::AddSink(IMenuBarSink * sink)
{
    m_sinks.insert(sink);
}

void WidgetMenuBar::RemoveSink(IMenuBarSink * sink)
{
    IMenuBarSinkSet::iterator itr = m_sinks.find(sink);
    if (itr != m_sinks.end())
    {
        m_sinks.erase(itr);
    }
}

void WidgetMenuBar::Attached(SCITER_ELEMENT element, IBaseElement * baseElement)
{
    m_baseElement = baseElement;
    m_menuBarElem = element;
    m_keySinkRoot = SciterElement();
    HWINDOW hwnd = (HWINDOW)m_menuBarElem.GetElementHwnd(true);
    if (hwnd != nullptr)
    {
        HELEMENT hRoot = 0;
        if (SciterGetRootElement((HWND)hwnd, &hRoot) == SCDOM_OK && hRoot != 0)
        {
            m_keySinkRoot = SciterElement(hRoot);
        }
    }
    if (!m_keySinkRoot.IsValid())
    {
        m_keySinkRoot = m_menuBarElem.GetRoot();
    }
    if (m_keySinkRoot.IsValid())
    {
        m_sciterUI.AttachHandler(m_keySinkRoot, IID_IKEYSINK, (IKeySink *)this);
        m_sciterUI.AttachHandler(m_keySinkRoot, IID_EVENTSINK, (IEventSink *)this);
    }
    m_sciterUI.AttachHandler(m_menuBarElem, IID_ICLICKSINK, (IClickSink*)this);
}

void WidgetMenuBar::Detached(SCITER_ELEMENT /*element*/)
{
    if (m_keySinkRoot.IsValid())
    {
        m_sciterUI.DetachHandler(m_keySinkRoot, IID_EVENTSINK, (IEventSink *)this);
        m_sciterUI.DetachHandler(m_keySinkRoot, IID_IKEYSINK, (IKeySink *)this);
        m_keySinkRoot = nullptr;
    }
    m_sciterUI.DetachHandler(m_menuBarElem, IID_ICLICKSINK, (IClickSink*)this);
    m_baseElement = nullptr;
    m_menuBarElem = nullptr;
}

std::shared_ptr<void> WidgetMenuBar::GetInterface(const char * riid)
{
    if (strcmp(riid, IID_IMENUBAR) == 0)
    {
        return std::static_pointer_cast<IMenuBar>(shared_from_this());
    }
    return nullptr;
}

bool WidgetMenuBar::OnSciterElement(SCITER_ELEMENT he)
{
    std::string menuIdvalue = SciterElement(he).GetAttribute("data-menu_id");
    if (!menuIdvalue.empty())
    {
        m_sciterUI.AttachHandler(he, IID_ICLICKSINK, (IClickSink *)this);
    }
    return false;
}

void WidgetMenuBar::SyncMenuMnemonicsAttribute()
{
    const bool on = m_leftAltDown || m_rightAltDown;
    if (m_keySinkRoot.IsValid())
    {
        if (on)
        {
            m_keySinkRoot.SetAttribute("data-menu-mnemonics", "1");
        }
        else
        {
            m_keySinkRoot.RemoveAttribute("data-menu-mnemonics");
        }
    }
    else if (m_menuBarElem.IsValid())
    {
        if (on)
        {
            m_menuBarElem.SetAttribute("data-menu-mnemonics", "1");
        }
        else
        {
            m_menuBarElem.RemoveAttribute("data-menu-mnemonics");
        }
    }
    else
    {
        return;
    }
    SciterElement menuUl = m_menuBarElem.IsValid() ? m_menuBarElem.FindFirst("ul#menu-bar") : SciterElement();
    if (menuUl.IsValid())
    {
        menuUl.Update(true);
    }
}

bool WidgetMenuBar::OnKeyDown(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys keyCode, uint32_t /*keyboardState*/)
{
    if (keyCode == SCITER_KEY_LEFT_ALT)
    {
        if (!m_leftAltDown)
        {
            m_leftAltDown = true;
            SyncMenuMnemonicsAttribute();
        }
    }
    else if (keyCode == SCITER_KEY_RIGHT_ALT)
    {
        if (!m_rightAltDown)
        {
            m_rightAltDown = true;
            SyncMenuMnemonicsAttribute();
        }
    }
    return false;
}

bool WidgetMenuBar::OnKeyUp(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys keyCode, uint32_t /*keyboardState*/)
{
    if (keyCode == SCITER_KEY_LEFT_ALT)
    {
        m_leftAltDown = false;
        SyncMenuMnemonicsAttribute();
    }
    else if (keyCode == SCITER_KEY_RIGHT_ALT)
    {
        m_rightAltDown = false;
        SyncMenuMnemonicsAttribute();
    }
    return false;
}

bool WidgetMenuBar::OnKeyChar(SCITER_ELEMENT /*element*/, SCITER_ELEMENT /*item*/, SciterKeys /*keyCode*/, uint32_t /*keyboardState*/)
{
    return false;
}

void WidgetMenuBar::HideShownMenuPopup() const
{
    SciterElement menu = m_menuBarElem.FindFirst("menu[window-state=\"shown\"]");
    if (menu.IsValid())
    {
        menu.HidePopup();
    }
}

void WidgetMenuBar::NotifySinksMenuItem(int32_t id, SCITER_ELEMENT item) const
{
    for (IMenuBarSinkSet::iterator itr = m_sinks.begin(); itr != m_sinks.end(); itr++)
    {
        (*itr)->OnMenuItem(id, item);
    }
}

bool WidgetMenuBar::ElementIsUnderMainMenuWidget(const SciterElement & hit, const SciterElement & mainMenuElem)
{
    SciterElement e(hit);
    for (int depth = 0; depth < 64 && e.IsValid(); depth++)
    {
        if (e == mainMenuElem)
        {
            return true;
        }
        e = e.GetParent();
    }
    return false;
}

bool WidgetMenuBar::OnEvent(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t event_code, uint64_t reason)
{
    if (event_code != kMenuItemClickEvent || reason != kClickReasonByKey)
    {
        return false;
    }
    if (!m_menuBarElem.IsValid())
    {
        return false;
    }
    SciterElement item(source);
    if (!item.IsValid() || !ElementIsUnderMainMenuWidget(item, m_menuBarElem))
    {
        return false;
    }
    for (uint32_t i = 0; i < 5; i++)
    {
        std::string menuIdvalue = item.GetAttribute("data-menu_id");
        if (!menuIdvalue.empty())
        {
            HideShownMenuPopup();
            NotifySinksMenuItem(std::stoi(menuIdvalue), item);
            return true;
        }
        item = item.GetParent();
        if (!item.IsValid())
        {
            break;
        }
    }
    return false;
}

bool WidgetMenuBar::OnClick(SCITER_ELEMENT element, SCITER_ELEMENT source, uint32_t /*reason*/)
{
    SciterElement item(source);
    for (uint32_t i = 0; i < 5; i++)
    {
        std::string menuIdvalue = item.GetAttribute("data-menu_id");
        if (!menuIdvalue.empty() || element == item)
        {
            break;
        }
        item = item.GetParent();
    }
    std::string menuIdvalue = item.GetAttribute("data-menu_id");
    if (!menuIdvalue.empty())
    {
        HideShownMenuPopup();
        NotifySinksMenuItem(std::stoi(menuIdvalue), item);
    }
    return false;
}

std::string WidgetMenuBar::MenuItemHtml(const MenuBarItem & item, uint32_t indent)
{
    if (item.ID() == MenuBarItem::SPLITER)
    {
        return SciterUI::stdstr_f("%*s<hr />\n", indent, "");
    }
    const std::string rawTitle = PrepareMenuTitleForMnemonics(item.Title());
    char accesskey = 0;
    std::string labelHtml;
    AppendMenuTitleHtml(rawTitle, labelHtml, accesskey);
    const std::string accesskeyAttr = accesskey ? SciterUI::stdstr_f(" accesskey=\"%c\"", accesskey) : std::string();
    SciterUI::stdstr title;
    std::string submenu;
    if (item.SubMenu() != nullptr && item.SubMenu()->size() > 0)
    {
        for (MenuBarItemList::const_iterator itr = item.SubMenu()->begin(); itr != item.SubMenu()->end(); itr++)
        {
            submenu += MenuItemHtml(*itr, indent + 4);
        }
    }
    if (!submenu.empty())
    {
        submenu = SciterUI::stdstr_f("\n%*s<menu>\n%s%*s</menu>\n%*s", indent + 2, "", submenu.c_str(), indent + 2, "", indent, "");
        title = SciterUI::stdstr_f("<li%s>\n%*s<caption>%s</caption>", accesskeyAttr.c_str(), indent + 2, "", labelHtml.c_str());
    }
    else
    {
        title = SciterUI::stdstr_f("<li data-menu_id=\"%d\"%s><span class='menu-item-label'>%s</span><span class='menu-accelerator'>%s</span>", item.ID(), accesskeyAttr.c_str(), labelHtml.c_str(), item.ShortcutAccel().Format().c_str());
    }
    return SciterUI::stdstr_f("%*s%s%s</li>\n", indent, "", title.c_str(), submenu.c_str());
}

IWidget * WidgetMenuBar::CreateWidget(ISciterUI & sciterUI)
{
    std::shared_ptr<WidgetMenuBar> instance(new WidgetMenuBar(sciterUI));
    IWidget * widget = (IWidget *)instance.get();
    m_instances.insert(MenuBars::value_type(widget, std::move(instance)));
    return widget;
}

void WidgetMenuBar::ReleaseWidget(IWidget * widget)
{
    MenuBars::iterator it = m_instances.find(widget);
    if (it != m_instances.end())
    {
        m_instances.erase(it);
    }
}

WidgetMenuBar::WidgetMenuBar(ISciterUI & sciterUI) :
    m_sciterUI(sciterUI),
    m_baseElement(nullptr),
    m_leftAltDown(false),
    m_rightAltDown(false)
{
}

void Register_WidgetMenuBar(ISciterUI& sciterUI)
{
    WidgetMenuBar::Register(sciterUI);
}