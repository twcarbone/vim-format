#pragma once

#include <string_view>
#include <vector>

#include "DocVisitor.h"

namespace doc
{

struct Doc
{
    virtual ~Doc()
    {
        for (const doc::Doc* pChild : lChildren)
        {
            delete pChild;
        }

        lChildren.clear();
    };

    virtual std::string to_string() const = 0;
    virtual void accept(DocVisitor& visitor) const = 0;

    void push(const Doc* doc)
    {
        lChildren.push_back(doc);
        nWidth += doc->nWidth;
    }

    std::vector<const Doc*> lChildren;
    size_t nWidth = 0;

protected:
    std::string width_str() const
    {
        return "+width:" + std::to_string(nWidth);
    }
};

struct Text : Doc
{
    // Static, unbreakable string ('let', 'let', 'my_var')

    virtual ~Text() = default;

    Text(std::string text) :
        sText { text }
    {
        nWidth = sText.size();
    }

    virtual std::string to_string() const override
    {
        return "Text " + sText + " " + width_str();
    }

    virtual void accept(DocVisitor& visitor) const override
    {
        visitor.visit(this);
    }

    std::string sText;
};

struct Line : Doc
{
    // A potential break; ' ' or '\n'

    virtual ~Line() = default;

    Line(size_t width)
    {
        nWidth = width;
    }

    virtual std::string to_string() const override
    {
        return "Line " + width_str();
    }

    virtual void accept(DocVisitor& visitor) const override
    {
        visitor.visit(this);
    }
};

struct Break : Doc
{
    // Forced line break

    virtual ~Break() = default;

    virtual std::string to_string() const override
    {
        return "Break " + width_str();
    }

    virtual void accept(DocVisitor& visitor) const override
    {
        visitor.visit(this);
    }
};

struct Group : Doc
{
    virtual ~Group() = default;

    virtual std::string to_string() const override
    {
        return "Group " + width_str();
    }

    virtual void accept(DocVisitor& visitor) const override
    {
        visitor.visit(this);
    }
};

struct Nest : Doc
{
    virtual ~Nest() = default;

    virtual std::string to_string() const override
    {
        return "Nest " + width_str();
    }

    virtual void accept(DocVisitor& visitor) const override
    {
        visitor.visit(this);
    }
};

};
