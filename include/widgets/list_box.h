#pragma once
#include <sciter_ui.h>
#include <stdint.h>

static const char * IID_ILISTBOX = "B334D118-EFBA-4200-8A95-5E2866874D4C";

__interface IListBox
{
    int32_t AddItem(const char * item, const char * value) = 0;
    bool RemoveItem(int32_t index) = 0;
    void ClearContents() = 0;
    uint32_t GetCount() const = 0;
    int32_t CurrentIndex() const = 0;
    SCITER_ELEMENT GetItem(uint32_t index) const = 0;
    SCITER_ELEMENT GetSelectedItem() const = 0;
    bool SelectItem(int32_t index) = 0;
};

bool Register_WidgetListBox(ISciterUI & SciterUI);