#pragma once
#include <sciter_ui.h>
#include <string>

__interface IPagesSink
{
    bool PageNavChangeFrom(const std::string & pageName, SCITER_ELEMENT pageNav);
    bool PageNavChangeTo(const std::string & pageName, SCITER_ELEMENT pageNav);
    void PageNavCreatedPage(const std::string & pageName, SCITER_ELEMENT page);
    void PageNavPageChanged(const std::string & pageName, SCITER_ELEMENT pageNav);
};

static const char * IID_IPAGENAV = "A1FD4FA4-6BEE-4166-AD9D-D7BF867B0B3E";

__interface IPageNav
{
    std::string GetCurrentPage();
    bool SetCurrentPage(const char * pageName);
    void AddSink(IPagesSink * sink);
    void RemoveSink(IPagesSink * sink);
};

bool Register_WidgetPageNav(ISciterUI & sciterUI);