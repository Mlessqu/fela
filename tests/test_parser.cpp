#include <doctest/doctest.h>
#include <string>
#include <vector>

#include "AbstractSyntaxTree.h++"
#include "Lexer.h++"
#include "Parser.h++"
#include "SemanticChecker.h++"

constexpr int TEST_LITERAL_ZERO = 0;
constexpr int TEST_LITERAL_ONE = 1;
constexpr int TEST_LITERAL_TWO = 2;
constexpr int TEST_LITERAL_THREE = 3;
constexpr int TEST_LITERAL_FOUR = 4;
constexpr int TEST_LITERAL_FIVE = 5;
constexpr int TEST_LITERAL_TEN = 10;
constexpr int TEST_LITERAL_ANSWER = 42;

constexpr size_t EXPECTED_ONE_NODE = 1;
constexpr size_t EXPECTED_TWO_PARAMS = 2;
constexpr size_t EXPECTED_THREE_NODES = 3;

static std::vector<fela::Token> tokenize(std::string _source)
{
    std::vector<fela::Token> tokens;
    fela::Lexer lexer;
    lexer.load_source(std::move(_source));
    while (true)
    {
        fela::Token tok = lexer.next_token();
        tokens.push_back(tok);
        if (tok.type_ == fela::TokenType::eof)
        {
            break;
        }
    }
    return tokens;
}

static std::unique_ptr<fela::AstProgram> parse(std::string _source)
{
    std::vector<fela::Token> tokens = tokenize(std::move(_source));
    fela::Parser parser(tokens);
    return parser.parse_program();
}

TEST_CASE("parser parses variable declarations")
{
    auto program = parse("int a = 42; bool flag = true; int b;");
    REQUIRE(program != nullptr);
    REQUIRE(program->nodes_.size() == EXPECTED_THREE_NODES);

    auto* var_a = dynamic_cast<fela::AstVariableDeclaration*>(program->nodes_[0].get());
    REQUIRE(var_a != nullptr);
    CHECK(var_a->identifier_ == "a");
    CHECK(var_a->type_ == fela::DataType::int_type);
    auto* val_a = dynamic_cast<fela::AstLiteralExpression*>(var_a->init_value_.get());
    REQUIRE(val_a != nullptr);
    CHECK(std::get<int>(val_a->value_) == TEST_LITERAL_ANSWER);

    auto* var_flag = dynamic_cast<fela::AstVariableDeclaration*>(program->nodes_[1].get());
    REQUIRE(var_flag != nullptr);
    CHECK(var_flag->identifier_ == "flag");
    CHECK(var_flag->type_ == fela::DataType::bool_type);
    auto* val_flag = dynamic_cast<fela::AstLiteralExpression*>(var_flag->init_value_.get());
    REQUIRE(val_flag != nullptr);
    CHECK(std::get<bool>(val_flag->value_) == true);

    auto* var_b = dynamic_cast<fela::AstVariableDeclaration*>(program->nodes_[2].get());
    REQUIRE(var_b != nullptr);
    CHECK(var_b->identifier_ == "b");
    CHECK(var_b->type_ == fela::DataType::int_type);
    CHECK(var_b->init_value_ == nullptr);
}

TEST_CASE("parser parses function declarations")
{
    auto program = parse("void foo(); int add(int a, bool b);");
    REQUIRE(program != nullptr);
    REQUIRE(program->nodes_.size() == EXPECTED_TWO_PARAMS);

    auto* fn_foo = dynamic_cast<fela::AstFunctionDeclaration*>(program->nodes_[0].get());
    REQUIRE(fn_foo != nullptr);
    CHECK(fn_foo->identifier_ == "foo");
    CHECK(fn_foo->return_type_ == fela::DataType::void_type);
    CHECK(fn_foo->parameters_.empty());

    auto* fn_add = dynamic_cast<fela::AstFunctionDeclaration*>(program->nodes_[1].get());
    REQUIRE(fn_add != nullptr);
    CHECK(fn_add->identifier_ == "add");
    CHECK(fn_add->return_type_ == fela::DataType::int_type);
    REQUIRE(fn_add->parameters_.size() == EXPECTED_TWO_PARAMS);
    CHECK(fn_add->parameters_[0].name_ == "a");
    CHECK(fn_add->parameters_[0].type_ == fela::DataType::int_type);
    CHECK(fn_add->parameters_[1].name_ == "b");
    CHECK(fn_add->parameters_[1].type_ == fela::DataType::bool_type);
}

TEST_CASE("parser parses function definition")
{
    auto program = parse("void main() { return; }");
    REQUIRE(program != nullptr);
    REQUIRE(program->nodes_.size() == EXPECTED_ONE_NODE);

    auto* fn_main = dynamic_cast<fela::AstFunctionDefinition*>(program->nodes_[0].get());
    REQUIRE(fn_main != nullptr);
    CHECK(fn_main->identifier_ == "main");
    CHECK(fn_main->return_type_ == fela::DataType::void_type);
}

TEST_CASE("parser enforces multiplication precedence over addition")
{
    auto program = parse("int x = 2 + 3 * 4;");
    REQUIRE(program != nullptr);
    REQUIRE(program->nodes_.size() == EXPECTED_ONE_NODE);

    auto* var = dynamic_cast<fela::AstVariableDeclaration*>(program->nodes_[0].get());
    REQUIRE(var != nullptr);

    auto* root_bin = dynamic_cast<fela::AstBinaryExpression*>(var->init_value_.get());
    REQUIRE(root_bin != nullptr);
    CHECK(root_bin->operator_ == fela::TokenType::plus);

    auto* lhs_lit = dynamic_cast<fela::AstLiteralExpression*>(root_bin->lhs_.get());
    REQUIRE(lhs_lit != nullptr);
    CHECK(std::get<int>(lhs_lit->value_) == TEST_LITERAL_TWO);

    auto* rhs_bin = dynamic_cast<fela::AstBinaryExpression*>(root_bin->rhs_.get());
    REQUIRE(rhs_bin != nullptr);
    CHECK(rhs_bin->operator_ == fela::TokenType::multiply_op);

    auto* mul_lhs = dynamic_cast<fela::AstLiteralExpression*>(rhs_bin->lhs_.get());
    REQUIRE(mul_lhs != nullptr);
    CHECK(std::get<int>(mul_lhs->value_) == TEST_LITERAL_THREE);

    auto* mul_rhs = dynamic_cast<fela::AstLiteralExpression*>(rhs_bin->rhs_.get());
    REQUIRE(mul_rhs != nullptr);
    CHECK(std::get<int>(mul_rhs->value_) == TEST_LITERAL_FOUR);
}

TEST_CASE("parser enforces left-associativity for binary operations")
{
    auto program = parse("int x = 10 - 5 - 2;");
    REQUIRE(program != nullptr);
    REQUIRE(program->nodes_.size() == EXPECTED_ONE_NODE);

    auto* var = dynamic_cast<fela::AstVariableDeclaration*>(program->nodes_[0].get());
    REQUIRE(var != nullptr);

    auto* root_bin = dynamic_cast<fela::AstBinaryExpression*>(var->init_value_.get());
    REQUIRE(root_bin != nullptr);
    CHECK(root_bin->operator_ == fela::TokenType::minus);

    auto* rhs_lit = dynamic_cast<fela::AstLiteralExpression*>(root_bin->rhs_.get());
    REQUIRE(rhs_lit != nullptr);
    CHECK(std::get<int>(rhs_lit->value_) == TEST_LITERAL_TWO);

    auto* lhs_bin = dynamic_cast<fela::AstBinaryExpression*>(root_bin->lhs_.get());
    REQUIRE(lhs_bin != nullptr);
    CHECK(lhs_bin->operator_ == fela::TokenType::minus);

    auto* sub_lhs = dynamic_cast<fela::AstLiteralExpression*>(lhs_bin->lhs_.get());
    REQUIRE(sub_lhs != nullptr);
    CHECK(std::get<int>(sub_lhs->value_) == TEST_LITERAL_TEN);

    auto* sub_rhs = dynamic_cast<fela::AstLiteralExpression*>(lhs_bin->rhs_.get());
    REQUIRE(sub_rhs != nullptr);
    CHECK(std::get<int>(sub_rhs->value_) == TEST_LITERAL_FIVE);
}

TEST_CASE("parser parses unary expressions")
{
    auto program = parse("int a = -5; bool b = !flag;");
    REQUIRE(program != nullptr);
    REQUIRE(program->nodes_.size() == EXPECTED_TWO_PARAMS);

    auto* var_a = dynamic_cast<fela::AstVariableDeclaration*>(program->nodes_[0].get());
    REQUIRE(var_a != nullptr);
    auto* unary_a = dynamic_cast<fela::AstUnaryExpression*>(var_a->init_value_.get());
    REQUIRE(unary_a != nullptr);
    CHECK(unary_a->operator_ == fela::TokenType::minus);
    auto* lit_a = dynamic_cast<fela::AstLiteralExpression*>(unary_a->rhs_.get());
    REQUIRE(lit_a != nullptr);
    CHECK(std::get<int>(lit_a->value_) == TEST_LITERAL_FIVE);

    auto* var_b = dynamic_cast<fela::AstVariableDeclaration*>(program->nodes_[1].get());
    REQUIRE(var_b != nullptr);
    auto* unary_b = dynamic_cast<fela::AstUnaryExpression*>(var_b->init_value_.get());
    REQUIRE(unary_b != nullptr);
    CHECK(unary_b->operator_ == fela::TokenType::negation_op);
    auto* var_expr = dynamic_cast<fela::AstVariableExpression*>(unary_b->rhs_.get());
    REQUIRE(var_expr != nullptr);
    CHECK(var_expr->identifier_ == "flag");
}

TEST_CASE("parser parses logical and relational expressions")
{
    auto program = parse("bool res = a > 0 && b == false;");
    REQUIRE(program != nullptr);
    REQUIRE(program->nodes_.size() == EXPECTED_ONE_NODE);

    auto* var = dynamic_cast<fela::AstVariableDeclaration*>(program->nodes_[0].get());
    REQUIRE(var != nullptr);

    auto* root_and = dynamic_cast<fela::AstBinaryExpression*>(var->init_value_.get());
    REQUIRE(root_and != nullptr);
    CHECK(root_and->operator_ == fela::TokenType::and_op);

    auto* lhs_rel = dynamic_cast<fela::AstBinaryExpression*>(root_and->lhs_.get());
    REQUIRE(lhs_rel != nullptr);
    CHECK(lhs_rel->operator_ == fela::TokenType::greater_op);

    auto* rhs_eq = dynamic_cast<fela::AstBinaryExpression*>(root_and->rhs_.get());
    REQUIRE(rhs_eq != nullptr);
    CHECK(rhs_eq->operator_ == fela::TokenType::equal_op);
}

TEST_CASE("parser parses function call with arguments")
{
    auto program = parse("int res = add(1, 2);");
    REQUIRE(program != nullptr);
    REQUIRE(program->nodes_.size() == EXPECTED_ONE_NODE);

    auto* var = dynamic_cast<fela::AstVariableDeclaration*>(program->nodes_[0].get());
    REQUIRE(var != nullptr);

    auto* call = dynamic_cast<fela::AstFunctionCall*>(var->init_value_.get());
    REQUIRE(call != nullptr);
    CHECK(call->identifier_ == "add");
    REQUIRE(call->arguments_.size() == EXPECTED_TWO_PARAMS);

    auto* arg1 = dynamic_cast<fela::AstLiteralExpression*>(call->arguments_[0].get());
    REQUIRE(arg1 != nullptr);
    CHECK(std::get<int>(arg1->value_) == TEST_LITERAL_ONE);

    auto* arg2 = dynamic_cast<fela::AstLiteralExpression*>(call->arguments_[1].get());
    REQUIRE(arg2 != nullptr);
    CHECK(std::get<int>(arg2->value_) == TEST_LITERAL_TWO);
}

TEST_CASE("parser parses control flow statements")
{
    const std::string source =
        "void run(int x) {"
        "    while (x > 0) {"
        "        x = x - 1;"
        "    }"
        "    if (x == 0) {"
        "        return;"
        "    } else {"
        "        return;"
        "    }"
        "}";

    auto program = parse(source);
    REQUIRE(program != nullptr);
    REQUIRE(program->nodes_.size() == EXPECTED_ONE_NODE);
    auto* fn = dynamic_cast<fela::AstFunctionDefinition*>(program->nodes_[0].get());
    REQUIRE(fn != nullptr);
    CHECK(fn->identifier_ == "run");
}
