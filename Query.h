#ifndef __QUERY_H__
#define __QUERY_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <memory>

namespace Query{

using line_no = std::vector<std::string>::size_type;

// 保存一个query结果
class QueryResult {
public:
    QueryResult(std::string p_sought, 
                std::shared_ptr<std::set<line_no>> p_lines,
                std::shared_ptr<std::vector<std::string>> p_file) :
                sought(p_sought), lines(p_lines), file(p_file) { }
    void print(std::ostream&);
    std::shared_ptr<std::vector<std::string>> get_file() { return file; }
    std::set<line_no>::iterator get_lines_begin() { return lines->begin(); }
    std::set<line_no>::iterator get_lines_end() { return lines->end(); }
private:
    // 查找单词
    std::string sought;
    // 出现的行号
    std::shared_ptr<std::set<line_no>> lines;
    // 输入文件
    std::shared_ptr<std::vector<std::string>> file;
};



class TextQuery {
public:
    // 读入给定文件，构建查找图
    TextQuery(std::ifstream &s);
    // query操作：接收string，返回QueryResult对象
    QueryResult query(const std::string&) const;
private:
    // 输入文件
    std::shared_ptr<std::vector<std::string>> file;
    // 每个单词到所在行号集合的映射
    std::map<std::string, std::shared_ptr<std::set<line_no>>> word_map;

};


// 抽象基类，定义纯虚函数
class Query_base {
    // Query需要调用Query_base的虚函数
    friend class Query;
protected:
    virtual ~Query_base() = default;
private:
    // 返回查询到的QueryResult
    virtual QueryResult eval(const TextQuery& ) const = 0;
    // 查找的内容
    virtual std::string rep() const = 0;
};

// Query作为管理Querybase继承体系的接口类
class Query {
    // 声明为友元：运算符需要访问私有的构造函数
    friend Query operator~(const Query&);
    friend Query operator|(const Query&, const Query&);
    friend Query operator&(const Query&, const Query&);
public:
    // 构建一个新的WordQuery
    Query(const std::string&);
    QueryResult eval(const TextQuery& tq) const { return query->eval(tq); }
    // 查找的内容
    std::string rep() const { return query->rep(); }
    void operator<<(std::ofstream&);
private:
    // 声明为私有：不希望用户代码能随便定义Query_base对象
    Query(std::shared_ptr<Query_base> q) : query(q) { }
    std::shared_ptr<Query_base> query;
};

class WordQuery : public Query_base {
    // Query需要调用WordQuery的构造函数    
    friend class Query;
    WordQuery(const std::string &s) : query_word(s) { }
    QueryResult eval(const TextQuery& t) const { return t.query(query_word); }
    std::string rep() const { return query_word; }
    std::string query_word;
};

inline Query::Query(const std::string &s) : query(new WordQuery(s)) {};

class NotQuery : public Query_base {
    friend Query operator~(const Query &);
    NotQuery(const Query &s) : query(s) { }
    QueryResult eval(const TextQuery&) const;
    std::string rep() const { return "~(" + query.rep() + ")"; }
    // 需要对其取反的原Query对象
    Query query;
};

inline Query operator~(const Query& operand) {
    // 隐式使用Query接收std::shared_ptr<Query_base>为参数的构造函数
    return std::shared_ptr<Query_base>(new NotQuery(operand));
};

// 同样是抽象基类,用于操作两个对象的逻辑
class BinaryQuery : public Query_base{
protected:
    BinaryQuery(const Query &l, const Query &r, std::string s)
        : lhs(l), rhs(r), opStr(s) { }
    // 注意：并没有定义eval函数，而是继承了该纯虚函数，因此也是一个抽象基类，不能创建BinaryQuery类型对象
    std::string rep() const {
        return "(" + lhs.rep() + " " + opStr + " " + rhs.rep() + ")";
    }
    // 两侧的运算对象
    Query lhs, rhs;
    // 运算符的名字
    std::string opStr;
};

class AndQuery : public BinaryQuery {
    friend Query operator&(const Query&, const Query&);
    // 使用BinaryQuery的构造函数
    AndQuery(const Query &left, const Query &right) : BinaryQuery(left, right, "&") { }
    // 覆盖eval函数，继承rep函数
    QueryResult eval(const TextQuery& ) const;
};

inline Query operator&(const Query &l_operand, const Query &r_operand) {
    return std::shared_ptr<Query_base>(new AndQuery(l_operand, r_operand));
};

class OrQuery : public BinaryQuery {
    friend Query operator|(const Query&, const Query&);
    OrQuery(const Query &left, const Query &right) : BinaryQuery(left, right, "|") { }
    QueryResult eval(const TextQuery& ) const;
};

inline Query operator|(const Query &l_operand, const Query &r_operand) {
    return std::shared_ptr<Query_base>(new OrQuery(l_operand, r_operand));
};

}
#endif