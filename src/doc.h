#pragma once

#include <string>
#include <vector>

#include "DocVisitor.h"
#include "util.h"

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
        return "width=" + std::to_string(nWidth);
    }
};

struct Text : Doc
{
    virtual ~Text() = default;

    Text(std::string text) :
        sText { text }
    {
        nWidth = sText.size();
    }

    Text(char c, size_t count) :
        sText(count, c)
    {
        nWidth = count;
    }

    virtual std::string to_string() const override
    {
        return "Text '" + vf::sanitize(sText) + "' " + width_str();
    }

    virtual void accept(DocVisitor& visitor) const override
    {
        visitor.visit(this);
    }

    std::string sText;
};

struct Line : Doc
{
    virtual ~Line() = default;

    Line(size_t width, bool add_trailing_whitespace = false)
    {
        nWidth = width;
        bAddTrailingWhitespace = add_trailing_whitespace;
    }

    virtual std::string to_string() const override
    {
        std::string tmp = "Line " + width_str();

        if (bAddTrailingWhitespace)
        {
            tmp += " +trailing_ws";
        }

        return tmp;
    }

    virtual void accept(DocVisitor& visitor) const override
    {
        visitor.visit(this);
    }

    bool bAddTrailingWhitespace = false;
};

struct Break : Doc
{
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
