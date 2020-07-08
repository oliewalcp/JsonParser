#include <iostream>
#include <fstream>
#include <string>
#include "jsonobj.h"
#include "json_parser.h"

using if_iterator = std::istreambuf_iterator<char>;

static void test_read()
{
    std::ifstream file("test.json");
    if(!file)
    {
        std::cout << "cannot open file test.json" << std::endl;
        return;
    }
    if_iterator beg(file);
    if_iterator end;
    std::string file_content(beg, end);
    file.close();

    ParseResult pr;
    JObj j = JsonParser::parse(file_content, pr);
    if(pr.code != JsonParser::ErrorCode::SUCCESS)
    {
        std::cout << JsonParser::result_msg(pr) << std::endl;
        return;
    }

    JsonObject &jo1 = j.object_cast();
    JsonNumber &jn1 = jo1->value("arg1").number_cast();
    std::cout << "arg1: " << jn1->value() << std::endl;

    std::cout << "arg2: ";
    JsonList &jl1 = jo1->value("arg2").list_cast();
    JsonList::iterator it = jl1->begin();
    for(; it != jl1->end(); ++it)
    {
        JObj &j = *it;
        if(j->type() == JsonObjectType::NUMBER)
        {
            std::cout << j.number_cast()->value() << " ";
        }
        else if(j->type() == JsonObjectType::OBJECT)
        {
            JsonObject &jo2 = j.object_cast();
            std::cout << std::endl;
            std::cout << "child1-node --> " << jo2->value("child1-node").string_cast()->value() << std::endl;
            std::cout << "wtf --> " << jo2->value("wtf").string_cast()->value() << std::endl; 
        }
    }
    std::cout << std::endl;

    JsonObject &jo3 = jo1->value("???").object_cast();
    JObj &jj = jo3->value("child2_node");
    std::cout << "child2_node is list ? " << std::boolalpha << (jj->type() == JsonObjectType::LIST) << std::endl;

    std::cout << "child2_node: ";
    JsonList &jl2 = jj.list_cast();
    it = jl2->begin();
    for(; it != jl2->end(); ++it)
    {
        JsonString &js = it->string_cast();
        std::cout << js->value() << " ";
    }
    std::cout << std::endl;

    std::cout << "child3_node: " << jo3->value("child3_node").string_cast()->value() << std::endl;
}

static void test_write()
{
    JsonObject jo;
    jo->set_value("hahaha", JObj());

    JsonBool jb;
    jb->set(false);
    jo->set_value("arg1", jb);

    JsonNumber jn;
    jn->set(5.036);
    jo->set_value("arg2", jn);

    JsonObject jo2;
    jo2->set_value("null", JObj());
    jo->set_value("child", jo2);

    JsonNumber jn2;
    jn2->set(10);

    JsonBool jb2;
    jb2->set(true);

    JsonList jl;
    jl->append(jn2);
    jl->append(JObj());
    jl->append(jb2);
    jo->set_value("num_list?", jl);

    std::ofstream ofile("emmm.json");
    std::string str = JsonParser::to_string(jo);
    ofile.write(str.c_str(), str.size());
    ofile.close();
}

int main()
{
    std::cout << "*************  test read  ************" << std::endl;
    test_read();
    std::cout << "*************  test write  ************" << std::endl;
    test_write();
    system("pause");
    return 0;
}