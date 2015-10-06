#ifndef WIDGET_H
#define WIDGET_H 

#include "mh/base/defs.h"
#include "mh/base/imports.h"

#include <string>

namespace mh
{

class Widget
{
public:
    Widget(int width=LP_DEFAULT_WIDGET_WIDTH, int height=LP_DEFAULT_WIDGET_HEIGHT, std::string title=LP_DEFAULT_WIDGET_TITLE)
        : m_enabled(false), m_width(width), m_height(height), m_title(title) {}
    virtual ~Widget() {}

    void         draw        (void);

protected:
    virtual bool predraw     (void);
    virtual void drawContent (void) = 0;
    virtual void postdraw    (void);

private:
    bool        m_enabled;
    int         m_width;
    int         m_height;
    std::string m_title;

}; // class Widget

} // namespace mh

#endif /* WIDGET_H */
