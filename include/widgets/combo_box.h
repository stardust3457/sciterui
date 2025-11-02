#pragma once
#include <sciter_ui.h>
#include <stdint.h>

static const char * IID_ICOMBOBOX = "2981D907-6EA1-43B4-B4FC-DBC23980A15D";

__interface IComboBox
{
    int32_t AddItem(const char * item, const char * value) = 0;
    void ClearContents() = 0;
    uint32_t GetCount() const = 0;
    int32_t CurrentIndex() const = 0;
    SCITER_ELEMENT GetSelectedItem() const = 0;
    bool SelectItem(int32_t index) = 0;
};

bool Register_WidgetComboBox(ISciterUI & SciterUI);