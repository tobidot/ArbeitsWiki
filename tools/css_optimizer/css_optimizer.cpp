#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <sstream>
#include <memory>

using namespace std;

std::string tab_symbol = "    ";

enum CSS_SELECTOR_TYPES
{
    ELEMENT,
    CONTAINS,
    OR
};
class CSS_Selector;

class CSS_Parent
{
  private:
    std::vector<CSS_Selector> my_selections;

  public:
    CSS_Parent() {}
    CSS_Parent(const CSS_Parent &cpy)
    {
        my_selections.insert(my_selections.begin(), cpy.my_selections.begin(), cpy.my_selections.end());
    }
    virtual ~CSS_Parent(){};
    virtual std::string get_full_selection_chain() const = 0;
    virtual std::stringstream as_css() const;

    const std::vector<CSS_Selector> &selections() const { return my_selections; }
    std::vector<CSS_Selector> &selections() { return my_selections; }
    CSS_Selector &add(const CSS_Selector &selection);
};

class CSS_Selector : public CSS_Parent
{
  private:
    CSS_Parent *my_parent;
    std::vector<std::string> my_data;
    std::string my_selection_symbol;

  public:
    CSS_Selector(std::string selectionSymbol, const std::vector<std::string> &data = std::vector<std::string>())
    {
        my_parent = nullptr;
        my_data = data;
        my_selection_symbol = selectionSymbol;
        if (my_parent)
            my_parent->add(*this);
    }
    virtual ~CSS_Selector(){};

    std::vector<std::string> get_data() const { return my_data; }
    std::string add_data(std::string data) { my_data.emplace_back(data); }
    CSS_Selector &set_parent(CSS_Parent *parent) { my_parent = parent; }
    virtual std::stringstream as_css() const
    {
        std::stringstream ss;
        if (my_data.empty())
            return ss;
        ss << get_full_selection_chain() << " {" << std::endl;
        for (const auto &line : my_data)
        {
            ss << tab_symbol << line << std::endl;
        }
        ss << "}" << std::endl
           << std::endl;
        ss << CSS_Parent::as_css().str();
        return ss;
    }
    virtual std::string get_full_selection_chain() const
    {
        return (my_parent) ? my_parent->get_full_selection_chain() + my_selection_symbol : my_selection_symbol;
    };
};

class CSS_Base : public CSS_Parent
{
  public:
    CSS_Base()
    {
    }
    std::string get_full_selection_chain() const
    {
        return "";
    }
};

CSS_Base parse(const string sourceFile)
{
    CSS_Base css;
    CSS_Selector a("div", {"width:100%;"});
    CSS_Selector b(":after", {"color:white;"});
    css.add(a).add(b).add(a);
    return css;
}

std::ostream &operator<<(std::ostream &os, const CSS_Selector &selector)
{
    os << selector.as_css().str();
}

void write(const string targetFile, const CSS_Base &css)
{
    ostream *out = nullptr;
    ofstream *fout = nullptr;
    if (targetFile.empty() == false)
    {
        std::cout << "Writing to File " << targetFile << std::endl;
        out = fout = new ofstream(targetFile, std::ios_base::out);
    }
    else
    {
        std::cout << "Writing to Stdout " << targetFile << std::endl;
        out = &std::cout;
    }
    *out << "/* Optimized Css generated with CSS_Optimizer (created by Tobias Gepp) */ " << endl;
    *out << css.as_css().str();
    out->flush();
    if (fout)
    {
        fout->close();
        delete fout;
    }
}

int main(int argc, char **argv)
{

    if (argc <= 1) // first arg is the program itself
    {
        cout << "USAGE : css_optimizer <SourceFile.css> [TargetFile.css]" << endl;
        return 0;
    }
    else if (argc == 2 || argc == 3)
    {
        try
        {
            string source(argv[1]);
            string target(argv[2] ? argv[2] : "");
            CSS_Base css = parse(source);
            write(target, css);
        }
        catch (std::exception &e)
        {
            std::cerr << "Exception : " << e.what() << std::endl;
            return -1;
        }
        catch (...)
        {
            std::cerr << "Exception : unknown" << std::endl;
            throw;
        }
        return 0;
    }
    else
    {
        cout << "ERROR : Too many arguments" << endl;
        return -1;
    }
    return 0;
}

std::stringstream CSS_Parent::as_css() const
{
    std::stringstream ss;
    cout << "Selected " << my_selections.size() << endl;
    for (const auto &selector : my_selections)
    {
        ss << selector.as_css().str() << std::endl;
    }
    return ss;
}

CSS_Selector &CSS_Parent::add(const CSS_Selector &selection)
{
    my_selections.emplace_back(selection);
    my_selections.back().set_parent(this);
    return my_selections.back();
}