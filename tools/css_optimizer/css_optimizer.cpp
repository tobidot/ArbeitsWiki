#include <iostream>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <fstream>
#include <sstream>
#include <memory>

using namespace std;

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
    virtual ~CSS_Parent(){};
    virtual std::string get_full_selection_chain() const = 0;

    void add(const CSS_Selector &selection)
    {
        my_selections.emplace_back(selection);
    }
};

class CSS_Selector : public CSS_Parent
{
  private:
    CSS_Parent *my_parent;
    std::string my_data;
    std::string my_selection_symbol;

  public:
    CSS_Selector(std::string selectionSymbol, CSS_Parent *parent = nullptr)
    {
        my_parent = parent;
        my_selection_symbol = selectionSymbol;
    }
    virtual ~CSS_Selector(){};

    std::string get_data() const { return my_data; }
    std::string add_data(std::string data) { my_data += data; }
    std::stringstream as_css() const
    {
        std::stringstream ss;
        ss << get_full_selection_chain() << " {" << std::endl;
        ss << my_data << "}" << std::endl
           << std::endl;
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
    vector<CSS_Selector *> selections;

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
    CSS_Selector a("div", &css);
    css.add(a);
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
        out = fout = new ofstream(targetFile, std::ios_base::out);
    else
        out = &std::cout;
    for (auto selector : css.selections)
    {
        *out << selector->as_css().str() << std::endl;
    }
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

            std::cerr << "PRE ";
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